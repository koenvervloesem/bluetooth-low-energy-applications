"""Show device information for a BLE device.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio
import sys

import bleak

DEVICE_NAME_UUID = "00002a00-0000-1000-8000-00805f9b34fb"
MODEL_NUMER_STRING_UUID = "00002a24-0000-1000-8000-00805f9b34fb"
MANUFACTURER_NAME_STRING_UUID = "00002a29-0000-1000-8000-00805f9b34fb"


async def main(address):
    """Connect to device and show some information."""
    try:
        async with bleak.BleakClient(address) as client:

            try:
                device_name = await client.read_gatt_char(
                    DEVICE_NAME_UUID
                )
                print(f"Device name : {device_name.decode()}")
            except bleak.exc.BleakError:
                pass

            try:
                model_number = await client.read_gatt_char(
                    MODEL_NUMER_STRING_UUID
                )
                print(f"Model number: {model_number.decode()}")
            except bleak.exc.BleakError:
                pass

            try:
                manufacturer_name = await client.read_gatt_char(
                    MANUFACTURER_NAME_STRING_UUID
                )
                print(f"Manufacturer: {manufacturer_name.decode()}")
            except bleak.exc.BleakError:
                pass

    except asyncio.exceptions.TimeoutError:
        print(f"Can't connect to device {address}.")


if __name__ == "__main__":

    if len(sys.argv) == 2:
        address = sys.argv[1]
        asyncio.run(main(address))
    else:
        print("Please specify the Bluetooth address.")
