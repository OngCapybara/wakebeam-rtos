#include "relay_driver.h"

void initRelay() {
    pinMode(PIN_RELAY_LAMP, OUTPUT);
    setRelayLamp(false); // Pastikan mati saat booting
}

void setRelayLamp(bool on) {
    if (RELAY_ACTIVE_LOW) {
        digitalWrite(PIN_RELAY_LAMP, on ? LOW : HIGH);
    } else {
        digitalWrite(PIN_RELAY_LAMP, on ? HIGH : LOW);
    }
}