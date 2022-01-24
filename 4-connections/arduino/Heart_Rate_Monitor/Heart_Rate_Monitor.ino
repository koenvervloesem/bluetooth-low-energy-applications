/* Monitor readings from a Bluetooth heart rate sensor
 *
 * Copyright (C) 2021 Koen Vervloesem (koen@vervloesem.eu)
 *
 * SPDX-License-Identifier: MIT
 *
 * Based on the NimBLE_Client example from H2zero
 */
#include <NimBLEDevice.h>

#define UUID_SERVICE "180D"
#define UUID_CHARACTERISTIC "2A37"

static NimBLEAdvertisedDevice *advDevice;

static bool doConnect = false;
static uint32_t scanTime = 0; // 0 = scan forever

class ClientCallbacks : public NimBLEClientCallbacks {
  void onConnect(NimBLEClient *pClient) {
    Serial.println("Connected");
    pClient->updateConnParams(120, 120, 0, 60);
  }

  void onDisconnect(NimBLEClient *pClient) {
    Serial.print(pClient->getPeerAddress().toString().c_str());
    Serial.println(" Disconnected - Starting scan");
    NimBLEDevice::getScan()->start(scanTime, nullptr);
  }

  /* Called when the peripheral requests a change to the connection
   * parameters.
   * Return true to accept and apply them or false to reject and keep
   * the currently used parameters. Default will return true.
   */
  bool onConnParamsUpdateRequest(NimBLEClient *pClient,
                                 const ble_gap_upd_params *params) {
    if (params->itvl_min < 24) { // 1.25ms units
      return false;
    } else if (params->itvl_max > 40) { // 1.25ms units
      return false;
    } else if (params->latency > 2) { // Intervals allowed to skip
      return false;
    } else if (params->supervision_timeout > 100) { // 10ms units
      return false;
    }

    return true;
  }
};

/* Define a class to handle the callbacks when advertisements are
 * received */
class AdvertisedDeviceCallbacks
    : public NimBLEAdvertisedDeviceCallbacks {
  void onResult(NimBLEAdvertisedDevice *advertisedDevice) {
    Serial.print("Advertised Device found: ");
    Serial.println(advertisedDevice->toString().c_str());
    if (advertisedDevice->isAdvertisingService(
            NimBLEUUID(UUID_SERVICE))) {
      Serial.println("Found Our Service");
      // Stop scan before connecting
      NimBLEDevice::getScan()->stop();
      // Save the device reference in a global for the client to use
      advDevice = advertisedDevice;
      // Ready to connect now
      doConnect = true;
    }
  }
};

/* Notification / Indication receiving handler callback */
void notifyCB(NimBLERemoteCharacteristic *pRemoteCharacteristic,
              uint8_t *pData, size_t length, bool isNotify) {
  if (length) {
    uint16_t heart_rate_measurement = pData[1];
    if (pData[0] & 1) {
      heart_rate_measurement += (pData[2] << 8);
    }
    Serial.println(heart_rate_measurement);
  }
}

/* Create a single global instance of the callback class to be used by
 * all clients
 */
static ClientCallbacks clientCB;

/* Handles the provisioning of clients and connects / interfaces with
 * the server
 */
bool connectToServer() {
  NimBLEClient *pClient = nullptr;

  // Check if we have a client we should reuse first
  if (NimBLEDevice::getClientListSize()) {
    /* Special case when we already know this device, we send false as
     * the second argument in connect() to prevent refreshing the
     * service database. This saves considerable time and power.
     */
    pClient =
        NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
    if (pClient) {
      if (!pClient->connect(advDevice, false)) {
        Serial.println("Reconnect failed");
        return false;
      }
      Serial.println("Reconnected client");
    } else {
      /* We don't already have a client that knows this device,
       * we will check for a client that is disconnected that we can
       * use.
       */
      pClient = NimBLEDevice::getDisconnectedClient();
    }
  }

  // No client to reuse? Create a new one.
  if (!pClient) {
    if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS) {
      Serial.println(
          "Max clients reached. No more connections possible");
      return false;
    }

    pClient = NimBLEDevice::createClient();

    Serial.println("New client created");

    pClient->setClientCallbacks(&clientCB, false);
    pClient->setConnectionParams(40, 56, 0, 51);
    pClient->setConnectTimeout(5);

    if (!pClient->connect(advDevice)) {
      /* Created a client but failed to connect, don't need to keep it
       * as it has no data
       */
      NimBLEDevice::deleteClient(pClient);
      Serial.println("Failed to connect, deleted client");
      return false;
    }
  }

  if (!pClient->isConnected()) {
    if (!pClient->connect(advDevice)) {
      Serial.println("Failed to connect");
      return false;
    }
  }

  Serial.print("Connected to: ");
  Serial.println(pClient->getPeerAddress().toString().c_str());
  Serial.print("RSSI: ");
  Serial.println(pClient->getRssi());

  /* Now we can read/write/subscribe the charateristics of the
   * services we are interested in
   */
  NimBLERemoteService *pSvc = nullptr;
  NimBLERemoteCharacteristic *pChr = nullptr;

  pSvc = pClient->getService(UUID_SERVICE);
  if (pSvc) { // Make sure it's not null
    pChr = pSvc->getCharacteristic(UUID_CHARACTERISTIC);
  }

  if (pChr) { // Make sure it's not null
    if (pChr->canNotify()) {
      if (!pChr->subscribe(true, notifyCB)) {
        // Disconnect if subscribe failed
        pClient->disconnect();
        return false;
      }
    } else if (pChr->canIndicate()) {
      if (!pChr->subscribe(false, notifyCB)) {
        // Disconnect if subscribe failed
        pClient->disconnect();
        return false;
      }
    }
  } else {
    Serial.println(
        "Heart Rate Measurement characteristic not found.");
  }

  Serial.println("Done with this device!");
  return true;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting NimBLE Client");

  NimBLEDevice::init("");

  NimBLEScan *pScan = NimBLEDevice::getScan();

  pScan->setAdvertisedDeviceCallbacks(
      new AdvertisedDeviceCallbacks());
  pScan->setInterval(60);
  pScan->setWindow(30);
  pScan->setActiveScan(true);
  pScan->start(scanTime, nullptr);
}

void loop() {
  // Loop here until we find a device we want to connect to
  while (!doConnect) {
    delay(1);
  }

  doConnect = false;

  // Found a device we want to connect to, do it now
  if (connectToServer()) {
    Serial.println("Success, scanning for more...");
  } else {
    Serial.println("Failed to connect, starting scan...");
  }

  NimBLEDevice::getScan()->start(scanTime, nullptr);
}
