"""Read BME280 sensor values from BLE advertisement data.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio

from construct import Int16sl, Int16ul, Struct
from construct.core import StreamError

from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData

bme280_format = Struct(
    "temperature" / Int16sl,
    "pressure" / Int16ul,
    "humidity" / Int16ul,
)


def device_found(
    device: BLEDevice, advertisement_data: AdvertisementData
):
    """Decode BME280 sensor values from advertisement data."""
    try:
        data = advertisement_data.manufacturer_data[0xffff]
        sensor_data = bme280_format.parse(data)
        print(f"Device     : {device.name}")
        print(f"Temperature: {sensor_data.temperature / 100} °C")
        print(f"Humidity   : {sensor_data.humidity / 100} %")
        print(
            f"Pressure   : {(sensor_data.pressure + 50000) / 100} hPa"
        )
        print(24 * "-")
    except KeyError:
        # Test company ID (0xffff) not found
        pass
    except StreamError:
        # Wrong format
        pass


async def scan():
    """Scan for devices"""
    scanner = BleakScanner()
    scanner.register_detection_callback(device_found)

    while True:
        await scanner.start()
        await asyncio.sleep(1.0)
        await scanner.stop()


asyncio.run(scan())
