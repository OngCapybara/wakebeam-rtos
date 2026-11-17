#include "task_input.h"
#include "../drivers/button_driver.h"
#include "../app_types.h"

void taskInput(void *pvParameters) {
    initButtons();
    for (;;) {
        ButtonEvent ev = scanButtons();
        if (ev != BTN_NONE) {
            xQueueSend(xButtonQueue, &ev, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(15)); // Polling tiap 15ms, ringan untuk CPU
    }
}