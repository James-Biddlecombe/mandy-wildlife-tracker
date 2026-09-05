#include "App.h"
#include "config.h"

void App::begin()
{
    Serial.println();
    Serial.println("================================");
    Serial.println("       MANDY'S WILDLIFE");
    Serial.println("================================");
    Serial.println();

    display.begin();

    display.showBootScreen();

    tracker.begin();

    display.showWiFiConnecting();

    if (wifi.begin())
    {
        display.showWiFiConnected(
            wifi.ipAddress()
        );
    }
    else
    {
        display.showError(
            "Wi-Fi connection failed"
        );
    }

    updateTracking();
}

void App::loop()
{
    wifi.maintain();

    if (millis() - lastTrackingUpdate >=
        TRACKING_UPDATE_INTERVAL_MS)
    {
        updateTracking();
    }

    delay(100);
}

void App::updateTracking()
{
    Serial.println("[APP] Updating animal location...");

    Animal updatedAnimal;

    if (tracker.update(updatedAnimal))
    {
        animal = updatedAnimal;

        display.showAnimal(animal);

        lastTrackingUpdate = millis();

        Serial.println("[APP] Animal update successful");
    }
    else
    {
        display.showError(
            "Unable to update animal location"
        );
    }
}
