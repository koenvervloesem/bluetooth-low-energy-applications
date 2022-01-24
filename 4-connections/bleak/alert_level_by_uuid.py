"""Read the Alert Level characteristic of a BLE device by its UUID.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio
import sys

import bleak

ALERT_LEVEL_UUID = "00002a06-0000-1000-8000-00805f9b34fb"


async def main(address):
    """Connect to device and read its Alert Level characteristic."""
    try:
        async with bleak.BleakClient(address) as client:

            alert_level = await client.read_gatt_char(
                ALERT_LEVEL_UUID
            )
            print(f"Alert level: {alert_level.decode()}")

    except asyncio.exceptions.TimeoutError:
        print(f"Can't connect to device {address}.")


if __name__ == "__main__":

    if len(sys.argv) == 2:
        address = sys.argv[1]
        asyncio.run(main(address))
    else:
        print("Please specify the Bluetooth address.")
