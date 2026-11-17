#pragma once
#include <Arduino.h>

// Identitas Perangkat BLE (untuk dicari oleh aplikasi Flutter)
#define DEVICE_NAME             "WakeBeam-Clock"
#define BLE_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_CHAR_RX_UUID        "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Pin I2C OLED 0.96"
#define PIN_I2C_SDA             21
#define PIN_I2C_SCL             22
#define OLED_WIDTH              128
#define OLED_HEIGHT             64
#define OLED_I2C_ADDR           0x3C

// Pin Output (Buzzer & Relay Lampu Bohlam)
#define PIN_BUZZER              18
#define PIN_RELAY_LAMP          19
#define RELAY_ACTIVE_LOW        true   // Ubah ke false jika modul relay aktif HIGH

// Pin 6 Tombol Fisik (Menggunakan Internal Pull-Up -> sambung ke GND)
#define PIN_BTN_MENU            13     // Buka Menu / Kembali (Back)
#define PIN_BTN_UP              14     // Navigasi Atas / Nilai (+)
#define PIN_BTN_DOWN            27     // Navigasi Bawah / Nilai (-)
#define PIN_BTN_OK              26     // Pilih / Konfirmasi / Stop Alarm
#define PIN_BTN_FACE            25     // Pintas ke Wajah / Ganti Ekspresi
#define PIN_BTN_CLOCK           33     // Pintas ke Jam / Snooze Alarm

// Parameter Sistem RTOS
#define IDLE_RETURN_TIMEOUT_MS  10000  // Otomatis kembali ke wallpaper wajah setelah 10 detik
#define DEBOUNCE_DELAY_MS       50
#define MAX_ALARMS              5      // Maksimal slot alarm yang bisa diset dari Flutter