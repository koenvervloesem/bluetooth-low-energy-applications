"""Blink the LED of the Xiaomi Mi Flora plant sensor.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio
import sys

import bleak

DEVICE_MODE_CHANGE_UUID = "00001a00-0000-1000-8000-00805f9b34fb"
BLINK_COMMAND = bytes([0xfd, 0xff])


async def main(address):
    """Connect to Mi Flora and blink the LED."""
    try:
        async with bleak.BleakClient(address) as client:

            try:
                await client.write_gatt_char(
                    DEVICE_MODE_CHANGE_UUID, BLINK_COMMAND
                )
            except bleak.exc.BleakError:
                print(f"Can't blink device {address}")

    except asyncio.exceptions.TimeoutError:
        print(f"Can't connect to device {address}.")


if __name__ == "__main__":

    if len(sys.argv) == 2:
        address = sys.argv[1]
        asyncio.run(main(address))
    else:
        print("Please specify the Bluetooth address.")
