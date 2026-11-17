#include "screen_alarm_ring.h"
#include "../drivers/display_driver.h"

void renderAlarmRingScreen(bool animToggle, bool isTestMode) {
    oled.clearDisplay();

    // Efek layar berkedip (invert) bergantian seirama dengan lampu bohlam!
    oled.invertDisplay(animToggle);

    // Wajah kaget / panik (mata melotot + mulut teriak O)
    oled.drawCircle(40, 22, 12, SSD1306_WHITE);
    oled.fillCircle(40, 22, animToggle ? 5 : 3, SSD1306_WHITE);

    oled.drawCircle(88, 22, 12, SSD1306_WHITE);
    oled.fillCircle(88, 22, animToggle ? 5 : 3, SSD1306_WHITE);

    // Mulut berteriak
    oled.fillRoundRect(54, 34, 20, 14, 6, SSD1306_WHITE);

    oled.setTextSize(1);
    oled.setCursor(12, 54);
    oled.print(isTestMode ? "TES BEAM! [OK:STOP]" : "BANGUN!! [OK:STOP]");

    oled.display();
}