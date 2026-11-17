#include "screen_clock.h"
#include "../drivers/display_driver.h"
#include "../tasks/task_timekeeper.h"

// Helper gambar ikon status bar kecil (Area Kuning y: 0..14)
static void drawSignalIcon(int x, int y) {
    // Antena Y
    oled.drawLine(x, y + 2, x + 3, y + 5, SSD1306_WHITE);
    oled.drawLine(x + 6, y + 2, x + 3, y + 5, SSD1306_WHITE);
    oled.drawFastVLine(x + 3, y + 5, 6, SSD1306_WHITE);
    // 4 Bar sinyal bertingkat
    oled.drawFastVLine(x + 7,  y + 9, 2, SSD1306_WHITE);
    oled.drawFastVLine(x + 9,  y + 7, 4, SSD1306_WHITE);
    oled.drawFastVLine(x + 11, y + 5, 6, SSD1306_WHITE);
    oled.drawFastVLine(x + 13, y + 3, 8, SSD1306_WHITE);
}

static void drawWifiMiniIcon(int x, int y) {
    oled.drawPixel(x + 4, y + 9, SSD1306_WHITE);
    oled.drawLine(x + 2, y + 7, x + 4, y + 5, SSD1306_WHITE);
    oled.drawLine(x + 4, y + 5, x + 6, y + 7, SSD1306_WHITE);
    oled.drawLine(x,     y + 5, x + 4, y + 1, SSD1306_WHITE);
    oled.drawLine(x + 4, y + 1, x + 8, y + 5, SSD1306_WHITE);
}

static void drawAlarmMiniIcon(int x, int y) {
    oled.fillTriangle(x + 4, y + 1, x + 1, y + 8, x + 7, y + 8, SSD1306_WHITE);
    oled.drawFastHLine(x, y + 9, 9, SSD1306_WHITE);
    oled.drawPixel(x + 4, y + 10, SSD1306_WHITE);
}

static void drawBatteryIcon(int x, int y, uint8_t levelBars) {
    // Kepala baterai di kiri (persis seperti di gambar)
    oled.fillRect(x, y + 3, 2, 5, SSD1306_WHITE);
    // Badan baterai
    oled.drawRect(x + 2, y + 1, 18, 9, SSD1306_WHITE);
    // Isi bar baterai (maks 4 bar)
    for (uint8_t i = 0; i < levelBars && i < 4; i++) {
        oled.fillRect(x + 4 + (i * 4), y + 3, 3, 5, SSD1306_WHITE);
    }
}

void renderClockScreen(const TimeData &nowTime) {
    oled.clearDisplay();
    oled.setTextColor(SSD1306_WHITE);

    // ==========================================
    // 1. ZONA ATAS / STATUS BAR (y = 0 .. 14)
    // ==========================================
    drawSignalIcon(1, 1);

    oled.setTextSize(1);
    oled.setCursor(18, 3);
    oled.print("WAKE"); // Nama "operator" WakeBeam

    // Ikon Wi-Fi & Ikon Alarm di tengah atas
    drawWifiMiniIcon(56, 2);
    if (gMainAlarm.enabled) {
        drawAlarmMiniIcon(72, 1);
    }

    // Ikon Baterai di pojok kanan atas
    drawBatteryIcon(106, 2, 4);

    // ==========================================
    // 2. ZONA TENGAH / JAM & SUB-INFO (y = 18..48)
    // ==========================================
    char timeBuf[8];
    // Titik dua berkedip setiap detik genap/ganjil
    char sep = (nowTime.second % 2 == 0) ? ':' : ' ';
    snprintf(timeBuf, sizeof(timeBuf), "%02d%c%02d", nowTime.hour, sep, nowTime.minute);

    oled.setTextSize(3);
    // Ukuran teks size 3: 5 karakter x 18px = 90px -> rata tengah x = (128 - 90) / 2 = 19
    oled.setCursor(19, 18);
    oled.print(timeBuf);

    // Sub-teks kecil di bawah jam (tampilkan jadwal alarm aktif atau tanggal)
    char subBuf[16];
    if (gMainAlarm.enabled) {
        snprintf(subBuf, sizeof(subBuf), "ALM %02d:%02d", gMainAlarm.hour, gMainAlarm.minute);
    } else {
        snprintf(subBuf, sizeof(subBuf), "2026.11.16");
    }
    oled.setTextSize(1);
    int subWidth = strlen(subBuf) * 6;
    oled.setCursor((128 - subWidth) / 2, 43);
    oled.print(subBuf);

    // ==========================================
    // 3. ZONA BAWAH / SOFTKEYS (y = 54..63)
    // ==========================================
    oled.setTextSize(1);
    oled.setCursor(2, 55);
    oled.print("MENU");

    // Ikon wajah mini di tengah bawah (sebagai indikator shortcut)
    oled.drawRoundRect(59, 54, 10, 9, 2, SSD1306_WHITE);
    oled.drawPixel(62, 57, SSD1306_WHITE);
    oled.drawPixel(65, 57, SSD1306_WHITE);
    oled.drawFastHLine(62, 60, 4, SSD1306_WHITE);

    oled.setCursor(102, 55);
    oled.print("FACE");

    oled.display();
}