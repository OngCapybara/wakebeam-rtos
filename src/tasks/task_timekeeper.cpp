#include "task_timekeeper.h"
#include "../config.h"

TimeData gCurrentTime = {7, 30, 0};
AlarmSlot gMainAlarm  = {7, 31, false}; // Default set 1 menit setelah jam awal buat gampang ngetes
volatile bool gAlarmTriggered = false;

void taskTimekeeper(void *pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    int8_t lastTriggeredMinute = -1;

    for (;;) {
        // vTaskDelayUntil menjamin interval tepat 1000ms tanpa drift
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(1000));

        if (xSemaphoreTake(xDataMutex, pdMS_TO_TICKS(50)) == pdTRUE) {
            // 1. Tambah 1 detik
            if (++gCurrentTime.second >= 60) {
                gCurrentTime.second = 0;
                if (++gCurrentTime.minute >= 60) {
                    gCurrentTime.minute = 0;
                    gCurrentTime.hour = (gCurrentTime.hour + 1) % 24;
                }
            }

            // 2. Cek kecocokan jadwal Alarm (tepat di detik ke-0)
            if (gMainAlarm.enabled &&
                gCurrentTime.hour == gMainAlarm.hour &&
                gCurrentTime.minute == gMainAlarm.minute &&
                gCurrentTime.second == 0 &&
                lastTriggeredMinute != gCurrentTime.minute) {
                
                lastTriggeredMinute = gCurrentTime.minute;
                gAlarmTriggered = true;

                FxCommand fx = FX_START_ALARM_BEAM;
                xQueueSend(xFxQueue, &fx, 0);
            }

            // Reset guard menit setelah menit alarm berlalu
            if (gCurrentTime.minute != lastTriggeredMinute) {
                lastTriggeredMinute = -1;
            }

            xSemaphoreGive(xDataMutex);
        }
    }
}