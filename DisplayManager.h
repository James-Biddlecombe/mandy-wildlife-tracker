#pragma once

#include <Arduino.h>
#include "models.h"

class DisplayManager {
public:

    bool begin();

    void clear();

    void showBootScreen();

    void showWiFiConnecting();

    void showWiFiConnected(const String& ip);

    void showAnimal(const Animal& animal);

    void showError(const String& message);

    void sleep();

private:

    void drawHeader();
    void drawMap(const AnimalLocation& location);
    void drawAnimalInfo(const Animal& animal);
};
