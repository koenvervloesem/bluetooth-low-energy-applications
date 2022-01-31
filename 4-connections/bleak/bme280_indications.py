"""Subscribe to indications of a BME280 sensor.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio
import sys

from construct import Int16sl, Int16ul, Struct
from construct.core import StreamError

import bleak

bme280_format = Struct(
    "temperature" / Int16sl,
    "pressure" / Int16ul,
    "humidity" / Int16ul,
)

BME280_SENSOR_UUID = "63bf0b19-2b9c-473c-9e0a-2cfcaf03a771"


def sensor_value_changed(handle: int, data: bytearray):
    """Show sensor values for an indication."""
    try:
        sensor_data = bme280_format.parse(data)
        print(f"Temperature: {sensor_data.temperature / 100} °C")
        print(f"Humidity   : {sensor_data.humidity / 100} %")
        print(
            f"Pressure   : {(sensor_data.pressure + 50000) / 100} hPa"
        )
        print(24 * "-")
    except StreamError:
        # Wrong format
        pass


async def main(address):
    """Connect to BME280 sensor and subscribe to indications."""
    try:
        async with bleak.BleakClient(address) as client:
            print(f"Connected to {address}")

            await client.start_notify(
                BME280_SENSOR_UUID, sensor_value_changed
            )
            print("Indications started...")
            while True:
                await asyncio.sleep(1)

    except asyncio.exceptions.TimeoutError:
        print(f"Can't connect to device {address}.")


if __name__ == "__main__":

    if len(sys.argv) == 2:
        address = sys.argv[1]
        asyncio.run(main(address))
    else:
        print("Please specify the Bluetooth address.")
