"""Scan for BLE devices with a detection callback.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio

from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData


def device_found(
    device: BLEDevice, advertisement_data: AdvertisementData
):
    """Show advertisement data on detection of a BLE device."""
    print(
        device.address,
        "| RSSI:",
        device.rssi,
        "|",
        advertisement_data,
    )


async def main():
    """Register detection callback and scan for devices."""
    scanner = BleakScanner()
    scanner.register_detection_callback(device_found)

    await scanner.start()
    await asyncio.sleep(5.0)
    await scanner.stop()


asyncio.run(main())
