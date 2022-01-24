"""Service explorer for BLE devices

Copyright (c) 2022 Koen Vervloesem

SPDX-License-Identifier: MIT
"""
import asyncio
import sys

from bleak import BleakClient


async def main(address):
    """Scan for devices and show their services."""
    async with BleakClient(address) as client:
        for service in client.services:
            print(f"- Description: {service.description}")
            print(f"  UUID: {service.uuid}")
            print(f"  Handle: {service.handle}")

            for char in service.characteristics:
                value = None
                if "read" in char.properties:
                    try:
                        value = bytes(
                            await client.read_gatt_char(char)
                        )
                    except Exception as error:
                        value = error
                print(f"  - Description: {char.description}")
                print(f"    UUID: {char.uuid}")
                print(f"    Handle: {char.handle}")
                print(f"    Properties: {', '.join(char.properties)}")
                print(f"    Value: {value}")

                for descriptor in char.descriptors:
                    desc_value = None
                    try:
                        desc_value = bytes(
                            await client.read_gatt_descriptor(
                                descriptor
                            )
                        )
                    except Exception as error:
                        desc_value = error
                    print(
                        f"    - Description: {descriptor.description}"
                    )
                    print(f"      UUID: {descriptor.uuid}")
                    print(f"      Handle: {descriptor.handle}")
                    print(f"      Value: {desc_value}")


if __name__ == "__main__":

    if len(sys.argv) == 2:
        address = sys.argv[1]
        asyncio.run(main(address))
    else:
        print("Please specify the Bluetooth address.")
