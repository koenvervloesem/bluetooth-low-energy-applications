/*
 * Read BME280 sensor data.
 *
 * Copyright (c) 2021 Koen Vervloesem
 *
 * SPDX-License-Identifier: MIT
 */

#include <device.h>
#include <devicetree.h>
#include <drivers/sensor.h>
#include <sys/printk.h>
#include <zephyr/types.h>

/*
 * Get a device structure from a devicetree node with compatible
 * "bosch,bme280". (If there are multiple, just pick one.)
 */
const struct device *bme280_get_device(void) {
  const struct device *dev = DEVICE_DT_GET_ANY(bosch_bme280);

  if (dev == NULL) {
    /* No such node, or the node does not have status "okay". */
    printk("\nError: no device found.\n");
    return NULL;
  }

  if (!device_is_ready(dev)) {
    printk("\nError: Device \"%s\" is not ready; "
           "check the driver initialization logs for errors.\n",
           dev->name);
    return NULL;
  }

  printk("Found device \"%s\", getting sensor data\n", dev->name);
  return dev;
}

void bme280_fetch_sample(const struct device *dev) {
  sensor_sample_fetch(dev);
}

int16_t bme280_get_temperature(const struct device *dev) {
  struct sensor_value temperature;

  sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temperature);
  return (int16_t)(temperature.val1 * 100 + temperature.val2 / 10000);
}

uint16_t bme280_get_pressure(const struct device *dev) {
  struct sensor_value pressure;
  uint32_t p; // Pressure without offset

  sensor_channel_get(dev, SENSOR_CHAN_PRESS, &pressure);
  p = (uint32_t)(pressure.val1 * 1000 + pressure.val2 / 10000);
  return (uint16_t)(p - 50000);
}

uint16_t bme280_get_humidity(const struct device *dev) {
  struct sensor_value humidity;

  sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);
  return (uint16_t)(humidity.val1 * 100 + humidity.val2 / 10000);
}
