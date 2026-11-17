#include "task_ui.h"
#include "../config.h"
#include "../app_types.h"
#include "../drivers/display_driver.h"
#include "../tasks/task_timekeeper.h"
#include "../ui/screen_face.h"
#include "../ui/screen_clock.h"
#include "../ui/screen_menu.h"
#include "../ui/screen_setting.h"
#include "../ui/screen_alarm_ring.h"

void taskUI(void *pvParameters) {
    if (!initDisplay()) {
        Serial.println("[ERROR] OLED tidak terdeteksi!");
        vTaskDelete(NULL);
    }

    AppState currentState = STATE_FACE_WALLPAPER;
    uint8_t currentFaceStyle = 0;
    uint8_t menuIndex = 0;
    
    // Variabel sementara untuk editing Jam/Alarm
    uint8_t editHour = 0, editMin = 0, cursorField = 0;
    bool editAlarmOn = false;
    bool isTestBeamMode = false;

    uint32_t lastActivityTime = millis();
    uint32_t lastBlinkTime = millis();
    bool isBlinking = false;

    ButtonEvent incomingBtn;
    FxCommand fxCmd;

    for (;;) {
        uint32_t now = millis();

        // 0. Override tertinggi: Jika Timekeeper memicu Alarm
        if (gAlarmTriggered && currentState != STATE_ALARM_RINGING) {
            isTestBeamMode = false;
            currentState = STATE_ALARM_RINGING;
        }

        // 1. Proses input dari xButtonQueue
        if (xQueueReceive(xButtonQueue, &incomingBtn, 0) == pdTRUE) {
            lastActivityTime = now;

            // Jika sedang bunyi Alarm / Tes Beam, tombol OK atau MENU mematikan efek
            if (currentState == STATE_ALARM_RINGING) {
                if (incomingBtn == BTN_OK_PRESS || incomingBtn == BTN_MENU_PRESS) {
                    gAlarmTriggered = false;
                    isTestBeamMode = false;
                    fxCmd = FX_STOP_ALL;
                    xQueueSend(xFxQueue, &fxCmd, 0);
                    oled.invertDisplay(false); // Kembalikan polaritas layar normal
                    currentState = STATE_CLOCK_VIEW;
                }
            } else {
                // Bunyi beep klik pendek untuk setiap tombol
                fxCmd = FX_BEEP_CLICK;
                xQueueSend(xFxQueue, &fxCmd, 0);

                if (incomingBtn == BTN_FACE_PRESS) {
                    if (currentState == STATE_FACE_WALLPAPER) {
                        currentFaceStyle = (currentFaceStyle + 1) % 3;
                    } else {
                        currentState = STATE_FACE_WALLPAPER;
                    }
                }
                else if (incomingBtn == BTN_CLOCK_PRESS) {
                    currentState = STATE_CLOCK_VIEW;
                }
                else if (incomingBtn == BTN_MENU_PRESS) {
                    if (currentState == STATE_MAIN_MENU) {
                        currentState = STATE_CLOCK_VIEW;
                    } else if (currentState == STATE_SET_TIME || currentState == STATE_SET_ALARM) {
                        currentState = STATE_MAIN_MENU; // Batal edit -> balik ke menu
                    } else {
                        menuIndex = 0;
                        currentState = STATE_MAIN_MENU;
                    }
                }
                // Logika di dalam Daftar Menu
                else if (currentState == STATE_MAIN_MENU) {
                    if (incomingBtn == BTN_UP_PRESS) {
                        menuIndex = (menuIndex == 0) ? 2 : menuIndex - 1;
                    } else if (incomingBtn == BTN_DOWN_PRESS) {
                        menuIndex = (menuIndex + 1) % 3;
                    } else if (incomingBtn == BTN_OK_PRESS) {
                        if (menuIndex == 0) { // Masuk Set Jam
                            xSemaphoreTake(xDataMutex, portMAX_DELAY);
                            editHour = gCurrentTime.hour;
                            editMin  = gCurrentTime.minute;
                            xSemaphoreGive(xDataMutex);
                            cursorField = 0;
                            currentState = STATE_SET_TIME;
                        } else if (menuIndex == 1) { // Masuk Set Alarm
                            xSemaphoreTake(xDataMutex, portMAX_DELAY);
                            editHour    = gMainAlarm.hour;
                            editMin     = gMainAlarm.minute;
                            editAlarmOn = gMainAlarm.enabled;
                            xSemaphoreGive(xDataMutex);
                            cursorField = 0;
                            currentState = STATE_SET_ALARM;
                        } else if (menuIndex == 2) { // Tes Lampu & Buzzer
                            isTestBeamMode = true;
                            currentState = STATE_ALARM_RINGING;
                            fxCmd = FX_START_ALARM_BEAM;
                            xQueueSend(xFxQueue, &fxCmd, 0);
                        }
                    }
                }
                // Logika di dalam Sub-Menu Set Jam & Set Alarm
                else if (currentState == STATE_SET_TIME || currentState == STATE_SET_ALARM) {
                    if (incomingBtn == BTN_UP_PRESS) {
                        if (cursorField == 0) editHour = (editHour + 1) % 24;
                        else if (cursorField == 1) editMin = (editMin + 1) % 60;
                        else if (cursorField == 2) editAlarmOn = !editAlarmOn;
                    } else if (incomingBtn == BTN_DOWN_PRESS) {
                        if (cursorField == 0) editHour = (editHour == 0) ? 23 : editHour - 1;
                        else if (cursorField == 1) editMin = (editMin == 0) ? 59 : editMin - 1;
                        else if (cursorField == 2) editAlarmOn = !editAlarmOn;
                    } else if (incomingBtn == BTN_OK_PRESS) {
                        uint8_t maxField = (currentState == STATE_SET_TIME) ? 1 : 2;
                        if (cursorField < maxField) {
                            cursorField++; // Geser ke field berikutnya
                        } else {
                            // Simpan perubahan dengan Mutex
                            xSemaphoreTake(xDataMutex, portMAX_DELAY);
                            if (currentState == STATE_SET_TIME) {
                                gCurrentTime.hour   = editHour;
                                gCurrentTime.minute = editMin;
                                gCurrentTime.second = 0;
                            } else {
                                gMainAlarm.hour    = editHour;
                                gMainAlarm.minute  = editMin;
                                gMainAlarm.enabled = editAlarmOn;
                            }
                            xSemaphoreGive(xDataMutex);

                            fxCmd = FX_CHIRP_HAPPY;
                            xQueueSend(xFxQueue, &fxCmd, 0);
                            currentState = STATE_CLOCK_VIEW;
                        }
                    }
                }
            }
        }

        // 2. Auto-Return 10 Detik (kecuali saat Alarm sedang bunyi!)
        if (currentState != STATE_FACE_WALLPAPER && currentState != STATE_ALARM_RINGING) {
            if ((now - lastActivityTime) >= IDLE_RETURN_TIMEOUT_MS) {
                currentState = STATE_FACE_WALLPAPER;
            }
        }

        // 3. Animasi kedip mata
        if (!isBlinking && (now - lastBlinkTime >= 3500)) {
            isBlinking = true;
            lastBlinkTime = now;
        } else if (isBlinking && (now - lastBlinkTime >= 180)) {
            isBlinking = false;
            lastBlinkTime = now;
        }

        // 4. Ambil salinan data waktu secara aman untuk dirender
        TimeData snapTime;
        xSemaphoreTake(xDataMutex, portMAX_DELAY);
        snapTime = gCurrentTime;
        xSemaphoreGive(xDataMutex);

        // 5. Render layar aktif
        switch (currentState) {
            case STATE_FACE_WALLPAPER:
                renderFaceScreen(currentFaceStyle, isBlinking);
                break;
            case STATE_CLOCK_VIEW:
                renderClockScreen(snapTime);
                break;
            case STATE_MAIN_MENU:
                renderMenuScreen(menuIndex);
                break;
            case STATE_SET_TIME:
                renderSettingScreen(0, editHour, editMin, false, cursorField);
                break;
            case STATE_SET_ALARM:
                renderSettingScreen(1, editHour, editMin, editAlarmOn, cursorField);
                break;
            case STATE_ALARM_RINGING:
                renderAlarmRingScreen((now / 250) % 2 == 0, isTestBeamMode);
                break;
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}