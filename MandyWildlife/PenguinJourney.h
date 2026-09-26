#pragma once

// ============================================================
// Luna's Year — a King Penguin's annual cycle
// ------------------------------------------------------------
// An ordered ~365-day sequence of waypoints describing a realistic
// year in the life of a King Penguin (Aptenodytes patagonicus)
// breeding at the Crozet Islands. Rather than one short loop, this
// covers the whole annual cycle so the story unfolds over a year
// and never feels like the same trip on repeat:
//
//   - Courtship & early incubation at the colony (ashore)
//   - Several breeding foraging trips south toward the Polar Front
//   - The long winter voyage (far-ranging, weeks at sea)
//   - Return, then the annual moult (a stationary fast ashore)
//
// Distances, bearings and durations are in the real range for
// Crozet King Penguins. Presented honestly as "Luna's year" — a
// King Penguin's life, replayed on the device over time.
//
// Each waypoint carries:
//   lat, lng      : position
//   dayOffset     : days since the year began (0 .. YEAR_DAYS)
//   phase         : which part of the annual cycle (for the display)
//   atColony      : true when she is ashore at the colony (not at sea)
// ============================================================

enum PenguinPhase : uint8_t {
    PHASE_COURTSHIP = 0,   // ashore, pairing/incubation
    PHASE_FORAGING  = 1,   // breeding foraging trips
    PHASE_WINTER    = 2,   // the long winter voyage
    PHASE_MOULT     = 3,   // ashore, moulting fast
};

struct JourneyPoint {
    double        lat;
    double        lng;
    double        dayOffset;
    PenguinPhase  phase;
    bool          atColony;
};

// The colony Luna belongs to.
static const char JOURNEY_COLONY_NAME[] = "Crozet Islands";
static const double JOURNEY_COLONY_LAT  = -46.4300;
static const double JOURNEY_COLONY_LNG  =  51.8600;

// Human-readable phase labels for the display.
static const char* const PHASE_LABELS[] = {
    "At the colony",     // courtship / incubation
    "Foraging at sea",   // breeding trips
    "Winter voyage",     // long-range winter journey
    "Moulting ashore",   // annual moult
};

// A friendly one-line explanation for each phase — especially useful
// during the stationary ashore phases so it's clear WHY she isn't
// moving. Kept short to fit the panel.
static const char* const PHASE_EXPLAIN[] = {
    "Ashore at the colony, courting and tending the nest.",
    "Out at sea, hunting fish beyond the shelf.",
    "Far south on her long winter journey.",
    "Ashore for her yearly moult, fasting as new feathers grow.",
};

// ------------------------------------------------------------
// The year. Day 0 starts in the austral summer breeding season.
// ~365 days total. Colony is (-46.43, 51.86).
// ------------------------------------------------------------
#define C_LAT -46.4300
#define C_LNG  51.8600

static const JourneyPoint JOURNEY[] = {
    // --- Courtship & incubation at the colony (days 0-24, ashore) ---
    { C_LAT, C_LNG,   0.0,  PHASE_COURTSHIP, true },
    { C_LAT, C_LNG,  12.0,  PHASE_COURTSHIP, true },
    { C_LAT, C_LNG,  24.0,  PHASE_COURTSHIP, true },

    // --- Breeding foraging trip 1: short, south to shelf (25-40) ---
    { -46.90, 51.66, 25.0,  PHASE_FORAGING, false },
    { -48.10, 51.10, 28.0,  PHASE_FORAGING, false },
    { -49.20, 50.70, 31.0,  PHASE_FORAGING, false },
    { -49.60, 51.00, 33.0,  PHASE_FORAGING, false },   // feeding
    { -48.60, 51.40, 36.0,  PHASE_FORAGING, false },
    { -47.30, 51.72, 39.0,  PHASE_FORAGING, false },
    { C_LAT, C_LNG,  40.0,  PHASE_FORAGING, true  },   // back to feed chick

    // --- Short colony spell (40-46) ---
    { C_LAT, C_LNG,  46.0,  PHASE_COURTSHIP, true },

    // --- Breeding foraging trip 2: further to Polar Front (47-70) ---
    { -47.20, 51.40, 47.0,  PHASE_FORAGING, false },
    { -48.80, 50.90, 51.0,  PHASE_FORAGING, false },
    { -50.40, 50.40, 55.0,  PHASE_FORAGING, false },
    { -51.60, 50.00, 59.0,  PHASE_FORAGING, false },   // Polar Front
    { -51.90, 50.60, 62.0,  PHASE_FORAGING, false },
    { -51.10, 51.10, 65.0,  PHASE_FORAGING, false },
    { -49.40, 51.50, 68.0,  PHASE_FORAGING, false },
    { C_LAT, C_LNG,  70.0,  PHASE_FORAGING, true  },

    // --- Colony spell (70-80) ---
    { C_LAT, C_LNG,  80.0,  PHASE_COURTSHIP, true },

    // --- Breeding foraging trip 3: east-south sweep (81-108) ---
    { -47.60, 52.60, 82.0,  PHASE_FORAGING, false },
    { -49.20, 53.60, 87.0,  PHASE_FORAGING, false },
    { -50.90, 54.10, 92.0,  PHASE_FORAGING, false },
    { -52.00, 53.40, 97.0,  PHASE_FORAGING, false },
    { -51.60, 52.30,102.0,  PHASE_FORAGING, false },
    { -49.80, 52.00,106.0,  PHASE_FORAGING, false },
    { C_LAT, C_LNG, 108.0,  PHASE_FORAGING, true  },

    // --- Colony spell before winter (108-120) ---
    { C_LAT, C_LNG, 120.0,  PHASE_COURTSHIP, true },

    // --- THE LONG WINTER VOYAGE (121-250): far-ranging, weeks at sea ---
    { -48.00, 50.80,121.0,  PHASE_WINTER, false },
    { -50.50, 49.50,130.0,  PHASE_WINTER, false },
    { -53.00, 48.00,142.0,  PHASE_WINTER, false },
    { -55.20, 47.20,156.0,  PHASE_WINTER, false },   // deep south
    { -56.50, 49.00,170.0,  PHASE_WINTER, false },   // furthest point
    { -56.00, 52.00,184.0,  PHASE_WINTER, false },
    { -54.50, 54.50,198.0,  PHASE_WINTER, false },
    { -52.50, 55.50,212.0,  PHASE_WINTER, false },
    { -50.50, 54.50,226.0,  PHASE_WINTER, false },
    { -48.50, 53.00,240.0,  PHASE_WINTER, false },
    { C_LAT, C_LNG, 250.0,  PHASE_WINTER, true  },   // home from winter

    // --- The annual moult: a stationary fast ashore (250-282) ---
    { C_LAT, C_LNG, 266.0,  PHASE_MOULT, true },
    { C_LAT, C_LNG, 282.0,  PHASE_MOULT, true },

    // --- Post-moult recovery trips (283-320) ---
    { -47.40, 51.30,283.0,  PHASE_FORAGING, false },
    { -49.00, 50.80,289.0,  PHASE_FORAGING, false },
    { -50.20, 50.90,295.0,  PHASE_FORAGING, false },
    { -49.30, 51.60,301.0,  PHASE_FORAGING, false },
    { -47.60, 51.80,307.0,  PHASE_FORAGING, false },
    { C_LAT, C_LNG, 312.0,  PHASE_FORAGING, true  },
    { C_LAT, C_LNG, 320.0,  PHASE_COURTSHIP, true },

    // --- Late-season foraging trip (321-352) ---
    { -47.80, 51.20,321.0,  PHASE_FORAGING, false },
    { -49.80, 50.40,328.0,  PHASE_FORAGING, false },
    { -51.40, 50.10,335.0,  PHASE_FORAGING, false },
    { -50.60, 51.00,341.0,  PHASE_FORAGING, false },
    { -48.70, 51.50,347.0,  PHASE_FORAGING, false },
    { C_LAT, C_LNG, 352.0,  PHASE_FORAGING, true  },

    // --- Return to courtship, closing the year (352-365) ---
    { C_LAT, C_LNG, 365.0,  PHASE_COURTSHIP, true },
};

#undef C_LAT
#undef C_LNG

static const int    JOURNEY_POINTS     = sizeof(JOURNEY) / sizeof(JOURNEY[0]);
static const double JOURNEY_TOTAL_DAYS  = 365.0;

// Luna's identity (honest: real species, a real King Penguin's year).
static const char LUNA_NAME[]    = "LUNA";
static const char LUNA_SPECIES[] = "King Penguin";
