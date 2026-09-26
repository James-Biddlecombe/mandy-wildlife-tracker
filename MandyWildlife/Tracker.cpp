#include "Tracker.h"
#include "config.h"
#include "Journey.h"

// NVS namespace + key for saving Luna's progress.
static const char* PREFS_NS  = "mandy";
static const char* PREFS_KEY = "voyageSec";

bool Tracker::begin() {
    Serial.println("[TRACKER] Offline King Penguin annual cycle");

    // Load saved progress (voyage-seconds) so she resumes where she was.
    prefs_.begin(PREFS_NS, false);
    journeySeconds_ = prefs_.getDouble(PREFS_KEY, 0.0);
    prefs_.end();

    lastTickMs_ = millis();
    lastSaveMs_ = millis();

    double startDay = fmod(journeySeconds_ / (double)REPLAY_SECONDS_PER_JOURNEY_DAY,
                           JOURNEY_TOTAL_DAYS);
    Serial.print("[TRACKER] Resuming at day ");
    Serial.print(startDay, 1);
    Serial.print(" / ");
    Serial.print((int)JOURNEY_TOTAL_DAYS);
    Serial.print("  (");
    Serial.print(Journey::totalDistanceKm(), 0);
    Serial.println(" km per year)");
    return true;
}

void Tracker::accrue() {
    unsigned long now = millis();
    unsigned long dtMs = now - lastTickMs_;
    lastTickMs_ = now;
    journeySeconds_ += dtMs / 1000.0;
}

void Tracker::save() {
    prefs_.begin(PREFS_NS, false);
    prefs_.putDouble(PREFS_KEY, journeySeconds_);
    prefs_.end();
    Serial.println("[TRACKER] Progress saved");
}

void Tracker::tick() {
    accrue();
    if (millis() - lastSaveMs_ >= PROGRESS_SAVE_INTERVAL_MS) {
        save();
        lastSaveMs_ = millis();
    }
}

double Tracker::currentDay() {
    double day = journeySeconds_ / (double)REPLAY_SECONDS_PER_JOURNEY_DAY;
    if (day > JOURNEY_TOTAL_DAYS) {
        if (REPLAY_LOOP) day = fmod(day, JOURNEY_TOTAL_DAYS);
        else             day = JOURNEY_TOTAL_DAYS;
    }
    return day;
}

bool Tracker::update(Animal& animal) {
    accrue();   // make sure the clock is current before we read it
    double day = currentDay();
    JourneyState s = Journey::at(day);

    animal.name    = LUNA_NAME;
    animal.species = LUNA_SPECIES;
    animal.description = "A King Penguin's year";

    animal.location.latitude     = s.lat;
    animal.location.longitude    = s.lng;
    animal.location.locationName = s.atColony ? JOURNEY_COLONY_NAME : "Southern Ocean";
    animal.location.timestamp    = millis();
    animal.location.valid        = true;

    animal.distanceTravelledKm = s.distanceTravelledKm;
    animal.journeyDay          = s.wholeDay;
    animal.journeyTotalDays    = (int)JOURNEY_TOTAL_DAYS;
    animal.atColony            = s.atColony;
    animal.phaseLabel          = s.phaseLabel;
    animal.phaseExplain        = s.phaseExplain;
    animal.phaseDay            = s.phaseDay;
    animal.phaseTotalDays      = s.phaseTotalDays;

    Serial.print("[TRACKER] Day ");
    Serial.print(s.wholeDay);
    Serial.print("/");
    Serial.print((int)JOURNEY_TOTAL_DAYS);
    Serial.print("  ");
    Serial.print(s.phaseLabel);
    Serial.print("  lat ");
    Serial.print(s.lat, 3);
    Serial.print(" lng ");
    Serial.print(s.lng, 3);
    Serial.print("  ");
    Serial.print(s.distanceTravelledKm, 0);
    Serial.println(" km");

    return true;
}
