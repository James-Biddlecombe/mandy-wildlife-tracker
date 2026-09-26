#pragma once

#include <Arduino.h>
#include "PenguinJourney.h"

// ============================================================
// Journey — interpolates Luna's position along her annual cycle,
// based on how many "journey days" have elapsed (0 .. 365).
// Reports the current phase (courtship / foraging / winter / moult)
// and whether she's ashore at the colony or out at sea.
// Pure math; no I/O.
// ============================================================

struct JourneyState {
    double        lat;
    double        lng;
    double        dayIndex;            // fractional day within the year
    int           wholeDay;            // 1..365, for "Day N of the year"
    double        distanceTravelledKm; // cumulative over the year so far
    PenguinPhase  phase;
    bool          atColony;
    const char*   phaseLabel;
    const char*   phaseExplain;        // friendly one-liner for this phase
    int           phaseDay;            // 1..N — day within the current phase
    int           phaseTotalDays;      // length of the current phase (days)
};

class Journey {
public:
    static double haversineKm(double lat1, double lng1, double lat2, double lng2) {
        const double R = 6371.0;
        double dLat = radians(lat2 - lat1);
        double dLng = radians(lng2 - lng1);
        double a = sin(dLat / 2) * sin(dLat / 2) +
                   cos(radians(lat1)) * cos(radians(lat2)) *
                   sin(dLng / 2) * sin(dLng / 2);
        return R * 2 * atan2(sqrt(a), sqrt(1 - a));
    }

    // Total distance over the whole year (all legs), km.
    static double totalDistanceKm() {
        double d = 0;
        for (int i = 1; i < JOURNEY_POINTS; i++) {
            d += haversineKm(JOURNEY[i - 1].lat, JOURNEY[i - 1].lng,
                             JOURNEY[i].lat, JOURNEY[i].lng);
        }
        return d;
    }

    // Position + stats at a given fractional day within the year.
    static JourneyState at(double day) {
        JourneyState s;
        if (day < 0) day = 0;
        if (day > JOURNEY_TOTAL_DAYS) day = JOURNEY_TOTAL_DAYS;

        // Find the leg [i-1, i] that contains `day`.
        int i = 1;
        while (i < JOURNEY_POINTS && JOURNEY[i].dayOffset < day) i++;
        if (i >= JOURNEY_POINTS) i = JOURNEY_POINTS - 1;

        const JourneyPoint& a = JOURNEY[i - 1];
        const JourneyPoint& b = JOURNEY[i];
        double span = b.dayOffset - a.dayOffset;
        double t = (span > 0) ? (day - a.dayOffset) / span : 0.0;
        if (t < 0) t = 0;
        if (t > 1) t = 1;

        s.lat = a.lat + (b.lat - a.lat) * t;
        s.lng = a.lng + (b.lng - a.lng) * t;
        s.dayIndex = day;
        s.wholeDay = (int)day + 1;
        if (s.wholeDay > (int)JOURNEY_TOTAL_DAYS) s.wholeDay = (int)JOURNEY_TOTAL_DAYS;

        // Phase / at-colony come from the leg we're travelling along
        // (the segment's starting waypoint defines the current activity).
        s.phase        = a.phase;
        s.atColony     = a.atColony && b.atColony;   // ashore only if both ends are
        s.phaseLabel   = PHASE_LABELS[(int)a.phase];
        s.phaseExplain = PHASE_EXPLAIN[(int)a.phase];

        // How long the CURRENT contiguous phase run lasts, and how far
        // into it she is. A "run" is consecutive waypoints sharing the
        // same phase. Used for the "Day N of moult" style counter.
        {
            int startIdx = i - 1;
            while (startIdx > 0 && JOURNEY[startIdx - 1].phase == a.phase)
                startIdx--;
            int endIdx = i - 1;
            while (endIdx < JOURNEY_POINTS - 1 && JOURNEY[endIdx + 1].phase == a.phase)
                endIdx++;
            double phaseStartDay = JOURNEY[startIdx].dayOffset;
            double phaseEndDay   = JOURNEY[endIdx].dayOffset;
            s.phaseTotalDays = (int)(phaseEndDay - phaseStartDay + 0.5);
            if (s.phaseTotalDays < 1) s.phaseTotalDays = 1;
            int pd = (int)(day - phaseStartDay) + 1;
            if (pd < 1) pd = 1;
            if (pd > s.phaseTotalDays) pd = s.phaseTotalDays;
            s.phaseDay = pd;
        }

        // Distance travelled so far this year.
        double dist = 0;
        for (int k = 1; k <= i - 1; k++) {
            dist += haversineKm(JOURNEY[k - 1].lat, JOURNEY[k - 1].lng,
                                JOURNEY[k].lat, JOURNEY[k].lng);
        }
        dist += haversineKm(a.lat, a.lng, s.lat, s.lng);
        s.distanceTravelledKm = dist;

        return s;
    }

    // Index of the last waypoint reached by `day` (for drawing trails).
    static int waypointsReached(double day) {
        int n = 0;
        for (int i = 0; i < JOURNEY_POINTS; i++) {
            if (JOURNEY[i].dayOffset <= day) n = i;
        }
        return n;
    }

    // First/last waypoint index of the CURRENT trip (contiguous run of
    // at-sea points around `day`). Used to draw just this trip's track,
    // so the map isn't cluttered with the whole year at once.
    static void currentTripRange(double day, int& firstIdx, int& lastIdx) {
        int cur = waypointsReached(day);
        // Walk back to the colony departure (previous atColony point).
        int start = cur;
        while (start > 0 && !JOURNEY[start].atColony) start--;
        // Walk forward to the next colony arrival.
        int end = cur;
        while (end < JOURNEY_POINTS - 1 && !JOURNEY[end].atColony) end++;
        firstIdx = start;
        lastIdx = end;
    }
};
