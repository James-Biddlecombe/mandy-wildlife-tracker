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

// Serial debugging
constexpr unsigned long SERIAL_BAUD = 115200;
