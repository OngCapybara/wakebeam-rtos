#include "task_alarm_fx.h"
#include "../app_types.h"
#include "../drivers/buzzer_driver.h"
#include "../drivers/relay_driver.h"

void taskAlarmFx(void *pvParameters) {
    initBuzzer();
    initRelay();

    FxCommand cmd;
    bool isBeaming = false;
    bool beamPhase = false;
    uint32_t lastBeatTime = 0;

    for (;;) {
        // Cek apakah ada perintah baru masuk ke antrean FX
        if (xQueueReceive(xFxQueue, &cmd, pdMS_TO_TICKS(20)) == pdTRUE) {
            switch (cmd) {
                case FX_BEEP_CLICK:
                    if (!isBeaming) {
                        playToneHz(2200, 35); // Klik pendek renyah
                    }
                    break;

                case FX_CHIRP_HAPPY:
                    if (!isBeaming) {
                        playToneHz(1500, 60);
                        vTaskDelay(pdMS_TO_TICKS(70));
                        playToneHz(2400, 90);
                    }
                    break;

                case FX_START_ALARM_BEAM:
                    isBeaming = true;
                    beamPhase = true;
                    lastBeatTime = 0; // Langsung eksekusi beat pertama
                    break;

                case FX_STOP_ALL:
                    isBeaming = false;
                    stopBuzzer();
                    setRelayLamp(false);
                    break;
            }
        }

        // Pola Kedip Lampu Bohlam + Sirine Alarm (berjalan paralel di Core 0)
        if (isBeaming) {
            uint32_t now = millis();
            if (now - lastBeatTime >= 250) { // Kedip tiap 250ms (4x per detik)
                lastBeatTime = now;
                beamPhase = !beamPhase;

                if (beamPhase) {
                    setRelayLamp(true);
                    playToneHz(2600, 220); // Nada tinggi kencang
                } else {
                    setRelayLamp(false);
                    playToneHz(1900, 220); // Nada kedua (efek sirine)
                }
            }
        }
    }
}