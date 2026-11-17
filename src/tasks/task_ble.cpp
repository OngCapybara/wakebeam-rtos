#include "task_ble.h"
#include "../app_types.h"
#include "../drivers/ble_driver.h"
#include "../tasks/task_timekeeper.h"

volatile int8_t gOverrideFaceStyle = -1;

void taskBle(void *pvParameters) {
    initBleServer();
    BleCommandPacket pkt;

    for (;;) {
        if (xQueueReceive(xBleCmdQueue, &pkt, portMAX_DELAY) == pdTRUE) {
            if (pkt.type == CMD_SYNC_TIME) {
                xSemaphoreTake(xDataMutex, portMAX_DELAY);
                gCurrentTime.hour   = pkt.param1 % 24;
                gCurrentTime.minute = pkt.param2 % 60;
                gCurrentTime.second = pkt.param3 % 60;
                xSemaphoreGive(xDataMutex);
                Serial.printf("[BLE] Jam disinkronkan: %02d:%02d:%02d\n", pkt.param1, pkt.param2, pkt.param3);
            } 
            else if (pkt.type == CMD_SET_ALARM) {
                xSemaphoreTake(xDataMutex, portMAX_DELAY);
                gMainAlarm.hour    = pkt.param2 % 24;
                gMainAlarm.minute  = pkt.param3 % 60;
                gMainAlarm.enabled = pkt.flag;
                xSemaphoreGive(xDataMutex);
                Serial.printf("[BLE] Alarm diset: %02d:%02d (%s)\n", pkt.param2, pkt.param3, pkt.flag ? "ON" : "OFF");
            } 
            else if (pkt.type == CMD_SET_FACE) {
                gOverrideFaceStyle = pkt.param1 % 3;
                Serial.printf("[BLE] Ekspresi wajah diubah ke: %d\n", gOverrideFaceStyle);
            }

            // Kasih feedback bunyi senang dari buzzer setiap terima setting dari HP
            FxCommand fx = FX_CHIRP_HAPPY;
            xQueueSend(xFxQueue, &fx, 0);
        }
    }
}