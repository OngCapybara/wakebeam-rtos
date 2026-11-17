#include "screen_setting.h"
#include "../drivers/display_driver.h"

void renderSettingScreen(uint8_t mode, uint8_t editHour, uint8_t editMin, bool alarmOn, uint8_t cursorField) {
    oled.clearDisplay();
    oled.setTextSize(1);
    oled.setCursor(0, 0);
    oled.print(mode == 0 ? "=== SET JAM ===" : "=== SET ALARM ===");
    oled.drawFastHLine(0, 10, 128, SSD1306_WHITE);

    // Tampilkan angka Jam & Menit besar
    char buf[8];
    oled.setTextSize(3);
    
    // Jam
    snprintf(buf, sizeof(buf), "%02d", editHour);
    oled.setCursor(18, 22);
    oled.print(buf);
    if (cursorField == 0) oled.fillRect(18, 47, 34, 3, SSD1306_WHITE); // Garis bawah kursor

    oled.setCursor(55, 22);
    oled.print(":");

    // Menit
    snprintf(buf, sizeof(buf), "%02d", editMin);
    oled.setCursor(74, 22);
    oled.print(buf);
    if (cursorField == 1) oled.fillRect(74, 47, 34, 3, SSD1306_WHITE);

    // Status ON/OFF khusus mode Set Alarm
    oled.setTextSize(1);
    if (mode == 1) {
        oled.setCursor(18, 54);
        oled.print("Status: ");
        oled.print(alarmOn ? "[ ON ]" : "[ OFF ]");
        if (cursorField == 2) oled.drawRect(64, 52, 46, 11, SSD1306_WHITE);
    } else {
        oled.setCursor(10, 54);
        oled.print("UP/DN:Ubah  OK:Lanjut");
    }

    oled.display();
}