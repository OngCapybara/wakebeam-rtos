#pragma once
#include <Arduino.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

// State Machine Menu Layar
enum AppState {
    STATE_FACE_WALLPAPER = 0,   // Default: animasi ekspresi wajah
    STATE_CLOCK_VIEW,           // Tampilan jam digital (auto-return 10s)
    STATE_MAIN_MENU,            // Daftar menu pilihan (Set Jam, Set Alarm, Tes Beam)
    STATE_SET_TIME,             // Sub-menu atur jam & menit
    STATE_SET_ALARM,            // Sub-menu atur jadwal alarm & ON/OFF
    STATE_ALARM_RINGING         // Override: Alarm aktif (buzzer + lampu bohlam kedip)
};

// Event dari 6 Tombol Fisik
enum ButtonEvent {
    BTN_NONE = 0,
    BTN_MENU_PRESS,
    BTN_UP_PRESS,
    BTN_DOWN_PRESS,
    BTN_OK_PRESS,
    BTN_FACE_PRESS,
    BTN_CLOCK_PRESS
};

// Jenis Efek Suara & Lampu
enum FxCommand {
    FX_STOP_ALL = 0,
    FX_BEEP_CLICK,
    FX_CHIRP_HAPPY,
    FX_START_ALARM_BEAM         // Buzzer kencang + Relay bohlam kedip
};

// Struktur Data Waktu & Alarm
struct TimeData {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

struct AlarmSlot {
    uint8_t hour;
    uint8_t minute;
    bool enabled;
};

// Paket Perintah dari Aplikasi Flutter via BLE
enum BleCmdType {
    CMD_SYNC_TIME,
    CMD_SET_ALARM,
    CMD_SET_FACE
};

struct BleCommandPacket {
    BleCmdType type;
    uint8_t param1;     // hour atau face_id atau alarm_id
    uint8_t param2;     // minute atau hour
    uint8_t param3;     // second atau minute
    bool flag;          // alarm enabled
};

// Global RTOS Handles (didefinisikan di WakeBeam.ino)
extern QueueHandle_t xButtonQueue;
extern QueueHandle_t xFxQueue;
extern QueueHandle_t xBleCmdQueue;
extern SemaphoreHandle_t xDataMutex;