#pragma once

#include <Arduino.h>

struct AnimalLocation {
    double latitude;
    double longitude;

    String locationName;

    unsigned long timestamp;

    bool valid;
};

struct Animal {
    String name;
    String species;
    String description;

    AnimalLocation location;

    double distanceTravelledKm;
};

struct TrackingStatus {
    bool connected;
    bool dataAvailable;

    String lastError;

    unsigned long lastUpdate;
};
