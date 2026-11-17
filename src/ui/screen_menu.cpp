#include "screen_menu.h"
#include "assets_icons.h"
#include "../drivers/display_driver.h"

struct MenuOption {
    const char* label;
    const unsigned char* icon;
};

static const MenuOption menuOptions[] = {
    {"SET JAM",    ICON_SET_CLOCK_32x32},
    {"SET ALARM",  ICON_SET_ALARM_32x32},
    {"TES BEAM",   ICON_TEST_BEAM_32x32}
};
static const uint8_t MENU_COUNT = 3;

void renderMenuScreen(uint8_t selectedIndex) {
    oled.clearDisplay();
    oled.setTextSize(1);

    // 1. Header Atas + Indikator Titik Halaman (o * o)
    oled.setCursor(2, 1);
    oled.print("MENU");

    for (uint8_t i = 0; i < MENU_COUNT; i++) {
        int dotX = 104 + (i * 8);
        if (i == selectedIndex) {
            oled.fillCircle(dotX, 4, 2, SSD1306_WHITE);
        } else {
            oled.drawCircle(dotX, 4, 2, SSD1306_WHITE);
        }
    }

    // 2. Hitung indeks menu kiri (prev) & kanan (next) untuk efek carousel
    uint8_t prevIdx = (selectedIndex == 0) ? (MENU_COUNT - 1) : (selectedIndex - 1);
    uint8_t nextIdx = (selectedIndex + 1) % MENU_COUNT;

    // Gambar ikon tetangga di pinggir kiri & kanan (ngintip sebagian)
    oled.drawBitmap(-16, 14, menuOptions[prevIdx].icon, 32, 32, SSD1306_WHITE);
    oled.drawBitmap(112, 14, menuOptions[nextIdx].icon, 32, 32, SSD1306_WHITE);

    // Panah petunjuk geser (< dan >)
    oled.setCursor(24, 26);
    oled.print("<");
    oled.setCursor(98, 26);
    oled.print(">");

    // 3. Bingkai Fokus Tengah + Ikon Utama 32x32 px
    oled.drawRoundRect(43, 11, 42, 38, 6, SSD1306_WHITE);
    oled.drawBitmap(48, 14, menuOptions[selectedIndex].icon, 32, 32, SSD1306_WHITE);

    // 4. Label Nama Menu di Bawah (Pill Badge Rata Tengah)
    const char* currentLabel = menuOptions[selectedIndex].label;
    int textWidth = strlen(currentLabel) * 6;
    int pillWidth = textWidth + 12;
    int pillX = (128 - pillWidth) / 2;

    oled.fillRoundRect(pillX, 52, pillWidth, 11, 3, SSD1306_WHITE);
    oled.setTextColor(SSD1306_BLACK);
    oled.setCursor(pillX + 6, 54);
    oled.print(currentLabel);
    oled.setTextColor(SSD1306_WHITE);

    oled.display();
}