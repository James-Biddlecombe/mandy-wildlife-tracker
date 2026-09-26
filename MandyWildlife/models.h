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

    // Journey-replay info (offline King Penguin annual cycle).
    int    journeyDay;        // 1..365 — day within her year
    int    journeyTotalDays;  // total days in the year (365)
    bool   atColony;          // true when she's ashore at the colony
    String phaseLabel;        // e.g. "Winter voyage", "At the colony"
    String phaseExplain;      // friendly one-liner explaining the phase
    int    phaseDay;          // day within the current phase (1..N)
    int    phaseTotalDays;    // length of the current phase (days)
};

struct TrackingStatus {
    bool connected;
    bool dataAvailable;

    String lastError;

    unsigned long lastUpdate;
};
