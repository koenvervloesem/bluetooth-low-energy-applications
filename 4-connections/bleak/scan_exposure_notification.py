"""Scan for Exposure Notifications.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio

from construct import Array, Byte, Struct

from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData

EXPOSURE_NOTIFICATION_UUID = "0000fd6f-0000-1000-8000-00805f9b34fb"

exposure_notification_format = Struct(
    "rpi" / Array(16, Byte), "aem" / Array(4, Byte)
)


def device_found(
    device: BLEDevice, advertisement_data: AdvertisementData
):
    """Show exposure notification data."""
    try:
        exposure_notification_data = advertisement_data.service_data[
            EXPOSURE_NOTIFICATION_UUID
        ]
        en = exposure_notification_format.parse(
            exposure_notification_data
        )
        print(f"Address                       : {device.address}")
        print(
            f"Rolling Proximity Identifier  : 0x{bytes(en.rpi).hex()}"
        )
        print(
            f"Associated Encryption Metadata: 0x{bytes(en.aem).hex()}"
        )
        print(66 * "-")
    except KeyError:
        # No Exposure Notification service data
        pass


async def main():
    """Register detection callback and scan for devices."""
    scanner = BleakScanner()
    scanner.register_detection_callback(device_found)

    while True:
        await scanner.start()
        await asyncio.sleep(1.0)
        await scanner.stop()


asyncio.run(main())
