######
Errata
######

These are corrections of errors found in the book `Develop your own Bluetooth Low Energy Applications for Raspberry Pi, ESP32 and nRF52 with Python, Arduino and Zephyr <https://koen.vervloesem.eu/books/develop-your-own-bluetooth-low-energy-applications/>`_ only after its publication:

************************************************
Chapter 3: Broadcasting data with advertisements
************************************************

* page 53: Bleak 0.15.0 has added support for passive scanning in the BlueZ backend. So now you can also use the ``scanner = BleakScanner(scanning_mode="passive")`` example in your Python code on Linux if you want your scanner to just listen to advertising packets without sending a ``SCAN_REQ`` packet. This requires BlueZ >= 5.56 and Linux kernel >= 5.10. Note that Bleak still doesn't support passive scanning on macOS.
