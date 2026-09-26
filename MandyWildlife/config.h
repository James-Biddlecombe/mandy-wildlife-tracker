#pragma once

// ============================================================
// Mandy Wildlife - Configuration
// ============================================================

// Device
#define DEVICE_NAME "Mandy's Wildlife"

// How often the display re-renders Luna's position.
// Matched to the LIFELIKE pace below (1 journey-day per real day):
// at that speed she moves slowly, so refreshing every 3 hours gives a
// clear, satisfying step each time without needless flicker. We also
// skip the redraw when her position hasn't visibly changed (see App),
// so the panel only refreshes when there's something new to show.
// If you speed the pace up for a demo, lower this too.
constexpr unsigned long TRACKING_UPDATE_INTERVAL_MS =
    3UL * 60UL * 60UL * 1000UL; // 3 hours

// ============================================================
// Journey replay
// ------------------------------------------------------------
// Luna's position is replayed from an embedded real-istic King
// Penguin voyage (see PenguinJourney.h). No live feed / no API.
//
// Luna lives out a full ~365-day King Penguin YEAR (see PenguinJourney.h):
// courtship at the colony, breeding foraging trips, the long winter
// voyage, and the annual moult. It loops each year, like real seasons.
//
// REPLAY_SECONDS_PER_JOURNEY_DAY controls how fast her year unfolds:
//   86400 = 1 real day  per journey day  (LIFELIKE — a full real year)
//    3600 = 1 real hour per journey day  (whole year in ~15 days)
//     120 = 2 real min  per journey day  (fast demo — whole year in ~12 h)
// Her progress is saved to flash, so unplugging doesn't reset her.
// ============================================================
constexpr unsigned long REPLAY_SECONDS_PER_JOURNEY_DAY = 86400UL; // lifelike
constexpr bool REPLAY_LOOP = true;

// How often to save Luna's progress to flash (NVS). Saving too often
// wears flash; every 10 min is plenty given the slow pace.
constexpr unsigned long PROGRESS_SAVE_INTERVAL_MS = 10UL * 60UL * 1000UL;

// Legacy flag kept for the tracker; mock is now the offline replay.
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
//   BUSY  -> GPIO6   (moved from GPIO4 — the GPIO4 line was flaky
//                     through the ribbon adapter; GPIO6 verified working)
//   PWR   -> GPIO5
//
// These reflect the ACTUAL physical wiring on the device and
// take precedence over any library/example defaults.
// The driver reads its pins from epd/DEV_Config.h — keep the two
// in sync. Do NOT use GPIO8 or GPIO14.
// ============================================================
constexpr int EPD_DIN  = 11;  // MOSI / SDI
constexpr int EPD_CLK  = 12;  // SCLK
constexpr int EPD_CS   = 10;  // Chip select
constexpr int EPD_DC   = 13;  // Data/Command
constexpr int EPD_RST  = 9;   // Reset
constexpr int EPD_BUSY = 6;   // Busy (active HIGH on this 5.83" G panel)
constexpr int EPD_PWR  = 5;   // Panel power enable

// Serial debugging
constexpr unsigned long SERIAL_BAUD = 115200;
