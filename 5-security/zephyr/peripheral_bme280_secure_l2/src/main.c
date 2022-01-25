/*
 * Indications for BME280 sensor data in a custom BLE service,
 * using Security Mode 1 Level 2
 *
 * Copyright (c) 2021 Koen Vervloesem
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <sys/byteorder.h>
#include <sys/printk.h>
#include <zephyr.h>
#include <zephyr/types.h>

#include <settings/settings.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/conn.h>
#include <bluetooth/gatt.h>
#include <bluetooth/hci.h>
#include <bluetooth/uuid.h>

#include "bme280.h"

// Define BLE service 63bf0b19-2b9c-473c-9e0a-2cfcaf03a770
#define BT_UUID_CUSTOM_SERVICE_VAL                                   \
  BT_UUID_128_ENCODE(0x63bf0b19, 0x2b9c, 0x473c, 0x9e0a,             \
                     0x2cfcaf03a770)

static struct bt_uuid_128 service_uuid =
    BT_UUID_INIT_128(BT_UUID_CUSTOM_SERVICE_VAL);

// Define BLE characteristic 63bf0b19-2b9c-473c-9e0a-2cfcaf03a771
static struct bt_uuid_128 char_uuid =
    BT_UUID_INIT_128(BT_UUID_128_ENCODE(0x63bf0b19, 0x2b9c, 0x473c,
                                        0x9e0a, 0x2cfcaf03a771));

// Initialize characteristic value (temperature, pressure, humidity)
static uint8_t char_value[6] = {0, 0, 0, 0, 0, 0};

const struct device *bme280 = NULL;

// Update data with BME280 sensor measurement
// Returns 1 if at least one sensor measurement changed compared to
// the previously stored value.
uint8_t update_data_bme280(const struct device *dev) {
  int16_t temperature;
  uint16_t pressure, humidity;
  uint8_t changed = 0;

  bme280_fetch_sample(dev);

  temperature = bme280_get_temperature(dev);
  if (memcmp(&(char_value[0]), &temperature, 2)) {
    memcpy(&(char_value[0]), &temperature, 2);
    changed = 1;
  }

  pressure = bme280_get_pressure(dev);
  if (memcmp(&(char_value[2]), &pressure, 2)) {
    memcpy(&(char_value[2]), &pressure, 2);
    changed = 1;
  }

  humidity = bme280_get_humidity(dev);
  if (memcmp(&(char_value[4]), &humidity, 2)) {
    memcpy(&(char_value[4]), &humidity, 2);
    changed = 1;
  }

  return changed;
}

// Callback function for reading characteristic
static ssize_t read_characteristic(struct bt_conn *conn,
                                   const struct bt_gatt_attr *attr,
                                   void *buf, uint16_t len,
                                   uint16_t offset) {
  update_data_bme280(bme280);
  const char *value = attr->user_data;

  return bt_gatt_attr_read(conn, attr, buf, len, offset, value,
                           sizeof(char_value));
}

static uint8_t indicate;
static uint8_t indicating;
static struct bt_gatt_indicate_params ind_params;

static void ccc_cfg_changed(const struct bt_gatt_attr *attr,
                            uint16_t value) {
  indicate = (value == BT_GATT_CCC_INDICATE) ? 1 : 0;
}

// Primary Service Declaration
BT_GATT_SERVICE_DEFINE(
    service, BT_GATT_PRIMARY_SERVICE(&service_uuid),
    BT_GATT_CHARACTERISTIC(&char_uuid.uuid,
                           BT_GATT_CHRC_READ | BT_GATT_CHRC_INDICATE,
                           BT_GATT_PERM_READ_ENCRYPT,
                           read_characteristic, NULL, char_value),
    BT_GATT_CCC(ccc_cfg_changed, BT_GATT_PERM_READ_ENCRYPT |
                                     BT_GATT_PERM_WRITE_ENCRYPT), );

// Advertising data
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS,
                  (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID128_ALL, BT_UUID_CUSTOM_SERVICE_VAL),
};

// Indicate callbacks
static void indicate_cb(struct bt_conn *conn,
                        struct bt_gatt_indicate_params *params,
                        uint8_t err) {
  printk("Indication %s\n", err != 0U ? "fail" : "success");
}

static void indicate_destroy(struct bt_gatt_indicate_params *params) {
  printk("Indication complete\n");
  indicating = 0U;
}

// GATT callbacks
void mtu_updated(struct bt_conn *conn, uint16_t tx, uint16_t rx) {
  printk("Updated MTU: TX: %d RX: %d bytes\n", tx, rx);
}

static struct bt_gatt_cb gatt_callbacks = {.att_mtu_updated =
                                               mtu_updated};

// Connection callbacks
static void connected(struct bt_conn *conn, uint8_t err) {
  char addr[BT_ADDR_LE_STR_LEN];

  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

  if (err) {
    printk("Failed to connect to %s (%u)\n", addr, err);
    return;
  }

  printk("Connected %s\n", addr);

  if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
    printk("Failed to set security\n");
  }
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
  char addr[BT_ADDR_LE_STR_LEN];

  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

  printk("Disconnected from %s (reason 0x%02x)\n", addr, reason);
}

static void security_changed(struct bt_conn *conn,
                             bt_security_t level,
                             enum bt_security_err err) {
  char addr[BT_ADDR_LE_STR_LEN];

  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

  if (!err) {
    printk("Security changed: %s level %u\n", addr, level);
  } else {
    printk("Security failed: %s level %u err %d\n", addr, level, err);
  }
}

static struct bt_conn_cb conn_callbacks = {
    .connected = connected,
    .disconnected = disconnected,
    .security_changed = security_changed,
};

// Bluetooth initialization
static void bt_ready(void) {
  int err;

  printk("Bluetooth initialized\n");

  if (IS_ENABLED(CONFIG_SETTINGS)) {
    settings_load();
  }

  err = bt_le_adv_start(BT_LE_ADV_CONN_NAME, ad, ARRAY_SIZE(ad), NULL,
                        0);
  if (err) {
    printk("Advertising failed to start (err %d)\n", err);
    return;
  }

  printk("Advertising successfully started\n");
}

void main(void) {
  int err;

  printk("Starting firmware...\n");

  // Initialize BME280
  bme280 = bme280_get_device();

  if (bme280 == NULL) {
    return;
  }

  // Initialize the Bluetooth subsystem
  err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return;
  }

  bt_ready();

  // Clear all bonds for debugging purposes
  // bt_unpair(BT_ID_DEFAULT, BT_ADDR_LE_ANY);

  // Register GATT and connection callbacks
  bt_gatt_cb_register(&gatt_callbacks);
  bt_conn_cb_register(&conn_callbacks);

  // Implement indications every second
  while (1) {
    k_sleep(K_SECONDS(1));

    if (indicate) {
      if (indicating) {
        continue;
      }

      if (update_data_bme280(bme280)) {
        ind_params.attr = &service.attrs[2];
        ind_params.func = indicate_cb;
        ind_params.destroy = indicate_destroy;
        ind_params.data = char_value;
        ind_params.len = sizeof(char_value);

        if (bt_gatt_indicate(NULL, &ind_params) == 0) {
          indicating = 1U;
        }
      }
    }
  }
}
