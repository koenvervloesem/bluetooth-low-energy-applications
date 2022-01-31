"""Subscribe to heart rate notifications of a fitness tracker.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio
import sys

import bleak

HEART_RATE_MEASUREMENT_UUID = "00002a37-0000-1000-8000-00805f9b34fb"


def heart_rate_changed(handle: int, data: bytearray):
    """Show heart rate."""
    print(f"Heart rate: {data[1]}")


async def main(address):
    """Subscribe to heart rate notifications."""
    try:
        async with bleak.BleakClient(address) as client:
            print(f"Connected to {address}")

            await client.start_notify(
                HEART_RATE_MEASUREMENT_UUID, heart_rate_changed
            )
            print("Notifications started...")
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
