#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "models.h"

// ============================================================
// Tracker — supplies Luna's current position.
//
// Data source is an OFFLINE replay of an embedded King Penguin
// ANNUAL CYCLE (see PenguinJourney.h / Journey.h). Her position
// advances with real time across a ~365-day year.
//
// Progress is PERSISTED to flash (NVS), so losing power or moving
// the device does not reset her — she resumes right where she was.
// (Time only advances while powered; we don't try to "catch up"
// missed wall-clock time while unplugged.)
// ============================================================

class Tracker {
public:
    bool begin();
    bool update(Animal& animal);

    // Call regularly from the main loop to accrue time + save progress.
    void tick();

private:
    Preferences prefs_;
    double         journeySeconds_ = 0.0;   // elapsed voyage-seconds
    unsigned long  lastTickMs_ = 0;
    unsigned long  lastSaveMs_ = 0;

    double currentDay();
    void   accrue();
    void   save();
};
