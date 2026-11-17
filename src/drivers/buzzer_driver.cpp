#include "buzzer_driver.h"

void initBuzzer() {
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_BUZZER, LOW);
}

void playToneHz(uint32_t freq, uint32_t durationMs) {
    tone(PIN_BUZZER, freq, durationMs);
}

void stopBuzzer() {
    noTone(PIN_BUZZER);
    digitalWrite(PIN_BUZZER, LOW);
}