#include "Tracker.h"
#include "config.h"

bool Tracker::begin() {
    Serial.println("[TRACKER] Initialising tracker");
    return true;
}

bool Tracker::update(Animal& animal) {
    if (MOCK_TRACKING) {
        return getMockAnimal(animal);
    } else {
        return getMovebankAnimal(animal);
    }
}

bool Tracker::getMockAnimal(Animal& animal) {
    animal.name = "Luna";
    animal.species = "King Penguin";

    animal.description =
        "A tracked wildlife friend";

    animal.location.latitude = -54.8123;
    animal.location.longitude = 158.4231;

    animal.location.locationName =
        "Southern Ocean";

    animal.location.timestamp =
        millis();

    animal.location.valid = true;
    animal.distanceTravelledKm = 1284.7;

    return true;
}

bool Tracker::getMovebankAnimal(Animal& animal) {
    // Movebank implementation will go here.

    return false;
}
