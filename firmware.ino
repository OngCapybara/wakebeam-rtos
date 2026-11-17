#include "src/config.h"
#include "src/app_types.h"

// Include header tasks (aktif bertahap)
#include "src/tasks/task_timekeeper.h"
#include "src/tasks/task_input.h"
#include "src/tasks/task_ui.h"
#include "src/tasks/task_alarm_fx.h"
// #include "src/tasks/task_ble.h"

// Inisialisasi Handle RTOS Global
QueueHandle_t xButtonQueue = NULL;
QueueHandle_t xFxQueue     = NULL;
QueueHandle_t xBleCmdQueue = NULL;
SemaphoreHandle_t xDataMutex = NULL;

void setup() {
    Serial.begin(115200);
    Serial.println("\n=== Booting WakeBeam RTOS ===");

    // 1. Buat Mutex untuk proteksi data waktu & alarm
    xDataMutex = xSemaphoreCreateMutex();

    // 2. Buat Antrean (Queue) Komunikasi Antar-Task
    xButtonQueue = xQueueCreate(10, sizeof(ButtonEvent));
    xFxQueue     = xQueueCreate(5, sizeof(FxCommand));
    xBleCmdQueue = xQueueCreate(5, sizeof(BleCommandPacket));

    if (xDataMutex == NULL || xButtonQueue == NULL || xFxQueue == NULL || xBleCmdQueue == NULL) {
        Serial.println("[ERROR] Gagal alokasi memori RTOS!");
        while (1);
    }

    // 3. Spawn RTOS Tasks (Core 1: Real-time UI & Input)
    xTaskCreatePinnedToCore(taskTimekeeper, "Timekeeper", 2048, NULL, 3, NULL, 1);
    xTaskCreatePinnedToCore(taskInput,      "InputBtn",   2048, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(taskUI,         "UI_OLED",    4096, NULL, 2, NULL, 1);
    xTaskCreatePinnedToCore(taskAlarmFx,    "AlarmFX",    2048, NULL, 2, NULL, 0);
    // xTaskCreatePinnedToCore(taskBle,        "BLE_Server", 4096, NULL, 1, NULL, 0);

    Serial.println("[OK] Kernel RTOS WakeBeam siap!");

    // Hapus loopTask bawaan Arduino karena semua pekerjaan sudah ditangani RTOS Tasks
    vTaskDelete(NULL);
}

void loop() {
    // Kosong (tidak akan pernah dieksekusi karena vTaskDelete di akhir setup)
}