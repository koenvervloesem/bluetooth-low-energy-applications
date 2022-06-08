####################################################################################################################
Develop your own Bluetooth Low Energy Applications for Raspberry Pi, ESP32 and nRF52 with Python, Arduino and Zephyr
####################################################################################################################

.. image:: https://github.com/koenvervloesem/bluetooth-low-energy-applications/workflows/Build/badge.svg
   :target: https://github.com/koenvervloesem/bluetooth-low-energy-applications/actions
   :alt: Continuous integration build

.. image:: https://github.com/koenvervloesem/bluetooth-low-energy-applications/workflows/Check/badge.svg
   :target: https://github.com/koenvervloesem/bluetooth-low-energy-applications/actions
   :alt: Continuous integration check

.. image:: https://img.shields.io/github/license/koenvervloesem/bluetooth-low-energy-applications.svg
   :target: https://github.com/koenvervloesem/bluetooth-low-energy-applications/blob/main/LICENSE
   :alt: License

This repository contains the code discussed in the book `Develop your own Bluetooth Low Energy Applications for Raspberry Pi, ESP32 and nRF52 with Python, Arduino and Zephyr <https://koen.vervloesem.eu/books/develop-your-own-bluetooth-low-energy-applications/>`_, published by `Elektor International Media <https://www.elektor.com>`_, as well as `a list of errors and their corrections <ERRATA.rst>`_ and some `additional tips and references <ADDITIONS.rst>`_.

**************
About the book
**************

Bluetooth Low Energy (BLE) radio chips are ubiquitous from Raspberry Pi to lightbulbs. BLE is an elaborate technology with a comprehensive specification, but the basics are quite accessible.

A progressive and systematic approach will lead you far in mastering this wireless communication technique, which is essential for working in low power scenarios.

In this book, you’ll learn how to:

* discover BLE devices in the neighborhood by listening to their advertisements
* create your own BLE devices advertising data
* connect to BLE devices such as heart rate monitors and proximity reporters
* create secure connections to BLE devices with encryption and authentication
* understand BLE service and profile specifications and implement them
* reverse engineer a BLE device with a proprietary implementation and control it with your own software
* make your BLE devices use as little power as possible

This book shows you the ropes of BLE programming with Python and the Bleak library on a Raspberry Pi or PC, with C++ and NimBLE-Arduino on Espressif's ESP32 development boards, and with C on one of the development boards supported by the Zephyr real-time operating system, such as Nordic Semiconductor's nRF52 boards.

Starting with a very little amount of theory, you'll develop code right from the beginning. After you've completed this book, you'll know enough to create your own BLE applications.

+----------------------+-------------------------------------+
| **Title**            | Develop your own Bluetooth Low Energy Applications for Raspberry Pi, ESP32 and nRF52 with Python, Arduino and Zephyr        |
+----------------------+-------------------------------------+
| **Author**           | Koen Vervloesem                     |
+----------------------+-------------------------------------+
| **Publication date** | 2022-06-08                          |
+----------------------+-------------------------------------+
| **Number of pages**  | 258                                 |
+----------------------+-------------------------------------+
| **Price**            | € 34.95                             |
+----------------------+-------------------------------------+
| **ISBN-13**          | 978-3-89576-500-1                   |
+----------------------+-------------------------------------+
| **Publisher**        | Elektor International Media (EIM)   |
+----------------------+-------------------------------------+

*************
Included code
*************

All example code from this book is included in this repository, stored in a directory for each chapter. For each chapter directory, the example code is subdivided into subdirectorues for NimBLE-Arduino code, Python/Bleak code, and C/Zephyr code.

*****************
Download the code
*****************

You can download the code all at once with `Git <https://git-scm.com/>`_:

.. code-block:: shell

  git clone https://github.com/koenvervloesem/bluetooth-low-energy-applications.git

The code is then downloaded into the directory ``bluetooth-low-energy-applications``.

You can also download a ZIP file of all code by clicking on the green button **Code** at the top right of this page and then on **Download ZIP**.

Just selecting and copying code from the GitHub web page of a specific file you're interested in and pasting it in an editor may work, but is not recommended. Especially with Python code the whitespace can become mixed up, which results in invalid code.

******
Errata
******

See the `ERRATA <ERRATA.rst>`_ document for the list of errors in the book and their corrections.

*********
Additions
*********

See the `ADDITIONS <ADDITIONS.rst>`_ document for some additional tips and references to interesting projects not mentioned in the book.

*******
License
*******

All code is provided by `Koen Vervloesem <http://koen.vervloesem.eu>`_ as open source software with the MIT license, unless noted otherwise in the license header of a file. See the `LICENSE <LICENSE>`_ for more information.
