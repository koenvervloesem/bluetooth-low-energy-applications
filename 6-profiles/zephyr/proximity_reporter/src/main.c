/*
 * Minimal Proximity Reporter implementing the Link Loss service.
 *
 * Copyright (c) 2021 Koen Vervloesem
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <stddef.h>
#include <string.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/printk.h>
#include <zephyr/kernel.h>
#include <zephyr/types.h>

#include <zephyr/settings/settings.h>

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/uuid.h>

uint8_t alert_level = 0;

void alert_on_link_loss() {
  switch (alert_level) {
  case 1:
    printk("Mild Alert\n");
    break;
  case 2:
    printk("High Alert\n");
    break;
  }
}

// Callback function for reading characteristic
static ssize_t read_alert_level(struct bt_conn *conn,
                                const struct bt_gatt_attr *attr,
                                void *buf, uint16_t len,
                                uint16_t offset) {
  uint8_t level = alert_level;

  return bt_gatt_attr_read(conn, attr, buf, len, offset, &level,
                           sizeof(level));
}

// Callback function for writing characteristic
static ssize_t write_alert_level(struct bt_conn *conn,
                                 const struct bt_gatt_attr *attr,
                                 const void *buf, uint16_t len,
                                 uint16_t offset, uint8_t flags) {
  uint8_t *value = attr->user_data;
  alert_level = *value;
  memcpy(value, buf, len);
  return len;
}

// Primary Service Declaration
BT_GATT_SERVICE_DEFINE(
    service_lls, BT_GATT_PRIMARY_SERVICE(BT_UUID_LLS),
    BT_GATT_CHARACTERISTIC(
        BT_UUID_ALERT_LEVEL, BT_GATT_CHRC_READ | BT_GATT_CHRC_WRITE,
        BT_GATT_PERM_READ_ENCRYPT | BT_GATT_PERM_WRITE_ENCRYPT,
        read_alert_level, write_alert_level, &alert_level), );

// Advertising data
static const struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS,
                  (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    BT_DATA_BYTES(BT_DATA_UUID16_ALL,
                  BT_UUID_16_ENCODE(BT_UUID_LLS_VAL)),
};

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

  // Use unauthenticated pairing with encryption
  if (bt_conn_set_security(conn, BT_SECURITY_L2)) {
    printk("Failed to set security\n");
  }
}

static void disconnected(struct bt_conn *conn, uint8_t reason) {
  char addr[BT_ADDR_LE_STR_LEN];

  bt_addr_le_to_str(bt_conn_get_dst(conn), addr, sizeof(addr));

  printk("Disconnected from %s (reason 0x%02x)\n", addr, reason);
  alert_on_link_loss();
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
}
