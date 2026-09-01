#pragma once

#include "WiFiManager.h"
#include "Tracker.h"
#include "DisplayManager.h"

class App {

public:

    void begin();

    void loop();

private:

    MandyWiFiManager wifi;
    Tracker tracker;
    DisplayManager display;

    Animal animal;

    unsigned long lastTrackingUpdate = 0;

    void updateTracking();
};
