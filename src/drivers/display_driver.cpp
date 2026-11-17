#include "display_driver.h"

Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);

bool initDisplay() {
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    if (!oled.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDR)) {
        return false;
    }
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);
    oled.display();
    return true;
}