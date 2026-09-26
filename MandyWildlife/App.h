#pragma once

#include "Tracker.h"
#include "DisplayManager.h"

class App {

public:

    void begin();

    void loop();

private:

    Tracker tracker;
    DisplayManager display;

    Animal animal;

    unsigned long lastTrackingUpdate = 0;

    // Track what we last drew so we can skip a pointless e-paper
    // refresh when Luna hasn't visibly moved.
    bool   hasDrawn_ = false;
    double lastDrawnLat_ = 999.0;
    double lastDrawnLng_ = 999.0;
    bool   lastDrawnAtColony_ = false;
    int    lastDrawnPhaseDay_ = -1;

    void updateTracking();
};
