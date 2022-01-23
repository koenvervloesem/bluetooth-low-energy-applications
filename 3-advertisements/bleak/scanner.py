"""Scan for BLE devices with Bleak."""
import asyncio

from bleak import BleakScanner


async def main():
    """Scan for BLE devices."""
    devices = await BleakScanner.discover()
    for device in devices:
        print(device)


asyncio.run(main())
