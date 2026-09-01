#pragma once

#include <Arduino.h>

class MandyWiFiManager {
public:

    bool begin();

    bool connected();

    String ipAddress();

    void maintain();

private:

    bool connectSavedNetwork();
};
