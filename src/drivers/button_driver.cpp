#include "button_driver.h"

struct BtnPinMap {
    uint8_t pin;
    ButtonEvent event;
    bool lastState;
    uint32_t lastDebounceTime;
};

static BtnPinMap buttons[] = {
    {PIN_BTN_MENU,  BTN_MENU_PRESS,  HIGH, 0},
    {PIN_BTN_UP,    BTN_UP_PRESS,    HIGH, 0},
    {PIN_BTN_DOWN,  BTN_DOWN_PRESS,  HIGH, 0},
    {PIN_BTN_OK,    BTN_OK_PRESS,    HIGH, 0},
    {PIN_BTN_FACE,  BTN_FACE_PRESS,  HIGH, 0},
    {PIN_BTN_CLOCK, BTN_CLOCK_PRESS, HIGH, 0}
};

void initButtons() {
    for (int i = 0; i < 6; i++) {
        pinMode(buttons[i].pin, INPUT_PULLUP);
    }
}

ButtonEvent scanButtons() {
    uint32_t now = millis();
    for (int i = 0; i < 6; i++) {
        bool reading = digitalRead(buttons[i].pin);
        if (reading != buttons[i].lastState) {
            buttons[i].lastDebounceTime = now;
        }
        if ((now - buttons[i].lastDebounceTime) > DEBOUNCE_DELAY_MS) {
            // Deteksi transisi dari HIGH ke LOW (saat tombol baru ditekan)
            static bool stableState[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
            if (reading != stableState[i]) {
                stableState[i] = reading;
                if (stableState[i] == LOW) {
                    buttons[i].lastState = reading;
                    return buttons[i].event;
                }
            }
        }
        buttons[i].lastState = reading;
    }
    return BTN_NONE;
}