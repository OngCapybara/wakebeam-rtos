#include "ble_driver.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <ArduinoJson.h>
#include "../config.h"
#include "../app_types.h"

volatile bool gBleConnected = false;
static BLEServer* pServer = nullptr;

class WakeBeamServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* server) override {
        gBleConnected = true;
        Serial.println("[BLE] Aplikasi Android/Flutter terhubung!");
    }

    void onDisconnect(BLEServer* server) override {
        gBleConnected = false;
        Serial.println("[BLE] Koneksi terputus, mulai advertising ulang...");
        server->getAdvertising()->start(); // Otomatis bisa discan lagi
    }
};

class WakeBeamRxCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic* pCharacteristic) override {
        std::string rxValue = pCharacteristic->getValue(); // Ganti String -> std::string
        if (rxValue.length() == 0) return;

        Serial.print("[BLE RX] ");
        Serial.println(rxValue.c_str()); // Tambahkan .c_str() untuk Serial.println

        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, rxValue); // ArduinoJson langsung support std::string
        if (err) {
            Serial.printf("[BLE] JSON Error: %s\n", err.c_str());
            return;
        }

        const char* cmd = doc["cmd"];
        if (!cmd) return;

        BleCommandPacket pkt;
        bool valid = false;

        if (strcmp(cmd, "SYNC_TIME") == 0) {
            pkt.type   = CMD_SYNC_TIME;
            pkt.param1 = doc["h"] | 0;
            pkt.param2 = doc["m"] | 0;
            pkt.param3 = doc["s"] | 0;
            valid = true;
        } 
        else if (strcmp(cmd, "SET_ALARM") == 0) {
            pkt.type   = CMD_SET_ALARM;
            pkt.param1 = doc["id"] | 0;
            pkt.param2 = doc["h"]  | 7;
            pkt.param3 = doc["m"]  | 0;
            pkt.flag   = (doc["on"] | 0) == 1;
            valid = true;
        } 
        else if (strcmp(cmd, "SET_FACE") == 0) {
            pkt.type   = CMD_SET_FACE;
            pkt.param1 = doc["id"] | 0;
            valid = true;
        }

        if (valid && xBleCmdQueue != NULL) {
            xQueueSend(xBleCmdQueue, &pkt, 0);
        }
    }
};

void initBleServer() {
    BLEDevice::init(DEVICE_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new WakeBeamServerCallbacks());

    BLEService* pService = pServer->createService(BLE_SERVICE_UUID);
    BLECharacteristic* pRxCharacteristic = pService->createCharacteristic(
        BLE_CHAR_RX_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR
    );
    pRxCharacteristic->setCallbacks(new WakeBeamRxCallbacks());

    pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    BLEDevice::startAdvertising();

    Serial.println("[BLE] Server aktif dengan nama: " DEVICE_NAME);
}