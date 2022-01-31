/** Scan for iBeacon advertisements.
 *
 * Copyright (c) 2021 Koen Vervloesem
 *
 * SPDX-License-Identifier: MIT
 *
 * Based on h2zero's example BLE_Beacon_Scanner.ino.
 *
 */
#include "NimBLEBeacon.h"
#include "NimBLEDevice.h"

#define ENDIAN_CHANGE_U16(x) ((((x)&0xff00) >> 8) + (((x)&0xff) << 8))

NimBLEScan *pBLEScan;

class MyAdvertisedDeviceCallbacks
    : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice *advertisedDevice) {
    if (advertisedDevice->haveManufacturerData() == true) {
      std::string strManufacturerData =
          advertisedDevice->getManufacturerData();

      if (strManufacturerData.length() == 25 &&
          strManufacturerData[0] == 0x4c &&
          strManufacturerData[1] == 0x00 &&
          strManufacturerData[2] == 0x02 &&
          strManufacturerData[3] == 0x15) {
        NimBLEBeacon iBeacon = NimBLEBeacon();
        iBeacon.setData(strManufacturerData);
        Serial.printf("UUID     : %s\n",
                      iBeacon.getProximityUUID().toString().c_str());
        Serial.printf("Major    : %d\n",
                      ENDIAN_CHANGE_U16(iBeacon.getMajor()));
        Serial.printf("Minor    : %d\n",
                      ENDIAN_CHANGE_U16(iBeacon.getMinor()));
        Serial.printf("TX power : %d dBm\n",
                      iBeacon.getSignalPower());
        Serial.printf("RSSI     : %d dBm\n",
                      advertisedDevice->getRSSI());
        Serial.println(
            "-----------------------------------------------");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning for iBeacons...");

  NimBLEDevice::init("");
  pBLEScan = NimBLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(
      new MyAdvertisedDeviceCallbacks(), true);
  pBLEScan->setInterval(97);
  pBLEScan->setWindow(37);
  pBLEScan->setMaxResults(0);
}

void loop() {
  if (pBLEScan->isScanning() == false) {
    pBLEScan->start(0);
  }

  delay(2000);
}
