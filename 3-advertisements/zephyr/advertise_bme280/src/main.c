/*
 * Advertise BME280 sensor data in BLE manufacturer-specific data
 *
 * Copyright (c) 2021 Koen Vervloesem
 *
 * SPDX-License-Identifier: MIT
 */

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/printk.h>
#include <sys/util.h>
#include <zephyr/types.h>

#include "bme280.h"

#define ADV_PARAM                                                    \
  BT_LE_ADV_PARAM(0, BT_GAP_ADV_SLOW_INT_MIN,                        \
                  BT_GAP_ADV_SLOW_INT_MAX, NULL)

static struct bt_data ad[] = {
    BT_DATA_BYTES(BT_DATA_FLAGS, BT_LE_AD_NO_BREDR),
    BT_DATA_BYTES(
        BT_DATA_MANUFACTURER_DATA, 0xff, 0xff, /* Test company ID */
        0x00, 0x00, /* Temperature, int16, little-endian */
        0x00, 0x00, /* Pressure - 50000, uint16, little-endian */
        0x00, 0x00) /* Humidity, uint16, little-endian */
};

void update_ad_bme280(const struct device *dev) {
  int16_t temperature;
  uint16_t pressure, humidity;

  bme280_fetch_sample(dev);

  temperature = bme280_get_temperature(dev);
  memcpy(&(ad[1].data[2]), &temperature, 2);

  pressure = bme280_get_pressure(dev);
  memcpy(&(ad[1].data[4]), &pressure, 2);

  humidity = bme280_get_humidity(dev);
  memcpy(&(ad[1].data[6]), &humidity, 2);
}

void main(void) {
  int err;

  printk("Starting firmware...\n");

  // Initialize BME280
  const struct device *bme280 = bme280_get_device();

  if (bme280 == NULL) {
    return;
  }

  // Initialize the Bluetooth subsystem
  err = bt_enable(NULL);
  if (err) {
    printk("Bluetooth init failed (err %d)\n", err);
    return;
  }

  printk("Bluetooth initialized\n");

  // Start advertising sensor values
  update_ad_bme280(bme280);
  err = bt_le_adv_start(ADV_PARAM, ad, ARRAY_SIZE(ad), NULL, 0);

  if (err) {
    printk("Advertising failed to start (err %d)\n", err);
    return;
  }

  while (1) {
    k_sleep(K_MSEC(980));
    // Update advertised sensor values
    update_ad_bme280(bme280);
    err = bt_le_adv_update_data(ad, ARRAY_SIZE(ad), NULL, 0);

    if (err) {
      printk("Advertising update failed (err %d)\n", err);
      return;
    }
  }
}
