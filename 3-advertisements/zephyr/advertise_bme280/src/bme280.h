/*
 * Read BME280 sensor data
 *
 * Copyright (c) 2021 Koen Vervloesem
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef BME280_H_
#define BME280_H_

const struct device *bme280_get_device(void);
void bme280_fetch_sample(const struct device *dev);
int16_t bme280_get_temperature(const struct device *dev);
uint16_t bme280_get_pressure(const struct device *dev);
uint16_t bme280_get_humidity(const struct device *dev);

#endif /* BME280_H_ */
