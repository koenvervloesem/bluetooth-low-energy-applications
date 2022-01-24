"""Scan for BLE service data.

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
    """Show service data on detection of a BLE device."""
    if advertisement_data.service_data:
        print(device.address)
        for service, data in advertisement_data.service_data.items():
            print(f"- {service}: 0x{data.hex()}")


async def main():
    """Register detection callback and scan for devices."""
    scanner = BleakScanner()
    scanner.register_detection_callback(device_found)

    await scanner.start()
    await asyncio.sleep(5.0)
    await scanner.stop()


asyncio.run(main())
