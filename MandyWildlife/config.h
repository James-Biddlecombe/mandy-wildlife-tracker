#pragma once

// ============================================================
// Mandy Wildlife - Configuration
// ============================================================

// Device
#define DEVICE_NAME "Mandy's Wildlife"

// Update interval
constexpr unsigned long TRACKING_UPDATE_INTERVAL_MS =
    30UL * 60UL * 1000UL; // 30 minutes

// Development mode
constexpr bool MOCK_TRACKING = true;

// Display
constexpr int DISPLAY_WIDTH  = 648;
constexpr int DISPLAY_HEIGHT = 480;

// ============================================================
// E-Paper wiring (AUTHORITATIVE)
// ------------------------------------------------------------
// Waveshare 5.83" e-Paper HAT (G)  ->  Waveshare ESP32-S3 Mini
//
//   VCC   -> 3V3
//   GND   -> GND
//   DIN   -> GPIO11
//   CLK   -> GPIO12
//   CS    -> GPIO10
//   DC    -> GPIO13
//   RST   -> GPIO9
//   BUSY  -> GPIO4
//   PWR   -> GPIO5
//
// These reflect the ACTUAL physical wiring on the device and
// take precedence over any library/example defaults.
// Do NOT change these to match a library example.
// Do NOT use GPIO8 or GPIO14.
// ============================================================
constexpr int EPD_DIN  = 11;  // MOSI / SDI
constexpr int EPD_CLK  = 12;  // SCLK
constexpr int EPD_CS   = 10;  // Chip select
constexpr int EPD_DC   = 13;  // Data/Command
constexpr int EPD_RST  = 9;   // Reset
constexpr int EPD_BUSY = 4;   // Busy (active LOW on G panels)
constexpr int EPD_PWR  = 5;   // Panel power enable

// Serial debugging
constexpr unsigned long SERIAL_BAUD = 115200;
