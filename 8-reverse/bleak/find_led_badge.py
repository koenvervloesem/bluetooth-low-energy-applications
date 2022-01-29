"""Find BLE LED badges.

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio

from bleak import BleakScanner

num_devices = 0


def device_found(device, advertisement_data):
    """Show device details if it's a BLE LED badge."""
    global num_devices
    if device.name.startswith("LSLED"):
        num_devices += 1
        print(
            f"{device.address} ({device.name}) - RSSI: {device.rssi}"
        )


async def main():
    """Scan for BLE devices."""
    print("Searching for LED badges...")
    scanner = BleakScanner()
    scanner.register_detection_callback(device_found)

    await scanner.start()
    await asyncio.sleep(5.0)
    await scanner.stop()

    if not num_devices:
        print("No devices found")


if __name__ == "__main__":
    asyncio.run(main())
