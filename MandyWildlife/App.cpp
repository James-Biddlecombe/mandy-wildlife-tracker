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

    // Fully offline — Luna's journey is replayed from flash. No Wi-Fi.
    updateTracking();
}

void App::loop()
{
    // Accrue Luna's voyage time and periodically persist it to flash.
    tracker.tick();

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

    if (!tracker.update(updatedAnimal))
    {
        display.showError("Unable to update animal location");
        return;
    }

    animal = updatedAnimal;
    lastTrackingUpdate = millis();

    // Decide whether a refresh is worthwhile:
    //  - At sea: only if she's moved enough to see (~0.03 deg, a few km),
    //    or her at-colony state changed. Avoids flickery no-op refreshes.
    //  - Ashore: refresh when the phase-day counter changes, so the
    //    "Day N of moult" ticks over daily even though she isn't moving.
    const double moveThreshold = 0.03;
    bool movedAtSea =
        fabs(animal.location.latitude  - lastDrawnLat_) > moveThreshold ||
        fabs(animal.location.longitude - lastDrawnLng_) > moveThreshold;
    bool phaseDayChanged = animal.phaseDay != lastDrawnPhaseDay_;

    bool shouldDraw =
        !hasDrawn_ ||
        animal.atColony != lastDrawnAtColony_ ||
        (animal.atColony ? phaseDayChanged : movedAtSea);

    if (!shouldDraw)
    {
        Serial.println("[APP] Nothing new to show — skipping refresh");
        return;
    }

    display.showAnimal(animal);
    hasDrawn_ = true;
    lastDrawnLat_ = animal.location.latitude;
    lastDrawnLng_ = animal.location.longitude;
    lastDrawnAtColony_ = animal.atColony;
    lastDrawnPhaseDay_ = animal.phaseDay;
    Serial.println("[APP] Display refreshed");
}
