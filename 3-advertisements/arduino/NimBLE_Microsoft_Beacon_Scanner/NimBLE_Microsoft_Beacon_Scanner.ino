#include "NimBLEDevice.h"

NimBLEScan *pBLEScan;

// See
// https://docs.microsoft.com/en-us/openspecs/windows_protocols/ms-cdp/77b446d0-8cea-4821-ad21-fabdf4d9a569
const char *deviceTypeMicrosoft[] = {"",
                                     "Xbox One",
                                     "",
                                     "",
                                     "",
                                     "",
                                     "Apple iPhone",
                                     "Apple iPad",
                                     "Android device",
                                     "Windows 10 Desktop",
                                     "",
                                     "Windows 10 Phone",
                                     "Linus device",
                                     "Windows IoT",
                                     "Surface Hub"};

class MyAdvertisedDeviceCallbacks
    : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice *advertisedDevice) {
    if (advertisedDevice->haveManufacturerData() == true) {
      std::string strManufacturerData =
          advertisedDevice->getManufacturerData();
      uint8_t deviceType;
      uint8_t salt[4];
      uint8_t deviceHash[16];

      if (strManufacturerData[0] == 0x06 &&
          strManufacturerData[1] == 0x00 &&
          strManufacturerData[2] == 0x01) {
        deviceType = strManufacturerData[3] & 0b00111111;
        memcpy(&salt, &strManufacturerData[6], 4);
        memcpy(&deviceHash, &strManufacturerData[10], 16);

        Serial.printf("Device type: %s (%d)\n",
                      deviceTypeMicrosoft[deviceType], deviceType);
        Serial.printf("RSSI       : %d dBm\n",
                      advertisedDevice->getRSSI());

        Serial.printf("Salt       : ");
        for (int i = 0; i < 4; i++) {
          Serial.printf("%02x", salt[i]);
        }
        Serial.println();

        Serial.printf("Device hash: ");
        for (int i = 0; i < 16; i++) {
          Serial.printf("%02x", deviceHash[i]);
        }
        Serial.println();
        Serial.println(
            "-----------------------------------------------");
      }
    }
  }
};

void setup() {
  Serial.begin(115200);
  Serial.println("Scanning for Microsoft advertising beacons...");

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
