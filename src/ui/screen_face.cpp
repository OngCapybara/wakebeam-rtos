#include "screen_face.h"
#include "../drivers/display_driver.h"

void renderFaceScreen(uint8_t faceStyle, bool isBlinking) {
    oled.clearDisplay();

    // Posisi dasar mata kiri & kanan
    int leftX = 40, rightX = 88, eyeY = 28;

    if (isBlinking) {
        // Garis mata merem saat berkedip
        oled.fillRect(leftX - 14, eyeY - 2, 28, 4, SSD1306_WHITE);
        oled.fillRect(rightX - 14, eyeY - 2, 28, 4, SSD1306_WHITE);
    } else {
        switch (faceStyle % 3) {
            case 0: // Ekspresi 1: Mata Bulat Lucu (Normal)
                oled.fillRoundRect(leftX - 14, eyeY - 14, 28, 28, 8, SSD1306_WHITE);
                oled.fillRoundRect(rightX - 14, eyeY - 14, 28, 28, 8, SSD1306_WHITE);
                // Kilau mata
                oled.fillCircle(leftX - 5, eyeY - 5, 4, SSD1306_BLACK);
                oled.fillCircle(rightX - 5, eyeY - 5, 4, SSD1306_BLACK);
                break;

            case 1: // Ekspresi 2: Mata Senyum (^ ^)
                oled.drawLine(leftX - 14, eyeY + 4, leftX, eyeY - 10, SSD1306_WHITE);
                oled.drawLine(leftX, eyeY - 10, leftX + 14, eyeY + 4, SSD1306_WHITE);
                oled.drawLine(rightX - 14, eyeY + 4, rightX, eyeY - 10, SSD1306_WHITE);
                oled.drawLine(rightX, eyeY - 10, rightX + 14, eyeY + 4, SSD1306_WHITE);
                break;

            case 2: // Ekspresi 3: Kacamata Kotak Cool
                oled.fillRect(leftX - 16, eyeY - 10, 32, 18, SSD1306_WHITE);
                oled.fillRect(rightX - 16, eyeY - 10, 32, 18, SSD1306_WHITE);
                oled.fillRect(leftX + 16, eyeY - 6, 16, 4, SSD1306_WHITE); // Bridge kacamata
                break;
        }
    }

    // Mulut kecil di bagian bawah
    oled.drawFastHLine(56, 52, 16, SSD1306_WHITE);
    oled.display();
}