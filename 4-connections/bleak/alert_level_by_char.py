"""Read the Alert Level characteristic of a BLE device by its
characteristic object.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio
import sys

import bleak

LINK_LOSS_UUID = "00001803-0000-1000-8000-00805f9b34fb"
ALERT_LEVEL_UUID = "00002a06-0000-1000-8000-00805f9b34fb"


async def main(address):
    """Connect to device and read its Alert Level characteristic."""
    try:
        async with bleak.BleakClient(address) as client:

            link_loss_service = client.services.get_service(
                LINK_LOSS_UUID
            )
            alert_level_characteristic = (
                link_loss_service.get_characteristic(ALERT_LEVEL_UUID)
            )
            alert_level = await client.read_gatt_char(
                alert_level_characteristic
            )
            print(f"Alert level: {int(alert_level[0])}")

    except asyncio.exceptions.TimeoutError:
        print(f"Can't connect to device {address}.")


if __name__ == "__main__":

    if len(sys.argv) == 2:
        address = sys.argv[1]
        asyncio.run(main(address))
    else:
        print("Please specify the Bluetooth address.")
