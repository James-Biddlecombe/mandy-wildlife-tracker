#include "DisplayManager.h"
#include "config.h"
#include "Journey.h"

// ============================================================
// Lifecycle
// ============================================================

bool DisplayManager::begin() {
    Serial.println("[DISPLAY] Init panel + framebuffer");

    if (!gfx_.begin()) {
        Serial.println("[DISPLAY] FATAL: framebuffer alloc failed");
        ready_ = false;
        return false;
    }

    DEV_Module_Init();     // pins + serial (already at 115200)
    EPD_5IN83G_Init();     // wake the panel
    ready_ = true;
    return true;
}

void DisplayManager::present() {
    if (!ready_) return;
    EPD_5IN83G_Display(gfx_.buffer());
}

void DisplayManager::clear() {
    if (!ready_) return;
    gfx_.clear(C_WHITE);
    present();
}

void DisplayManager::sleep() {
    if (!ready_) return;
    Serial.println("[DISPLAY] Sleep");
    EPD_5IN83G_Sleep();
}

// ============================================================
// Layout constants (device pixels) — mirror the simulator
// ============================================================

static const int BORDER   = 8;
static const int INSET    = BORDER + 12;
static const int HEADER_H = 58;
static const int FOOTER_H = 98;

// ============================================================
// Screens
// ============================================================

void DisplayManager::showBootScreen() {
    if (!ready_) return;
    Serial.println("[DISPLAY] Boot screen");
    gfx_.clear(C_WHITE);
    drawFrame();

    const int cx = EpdCanvas::W / 2;

    // Penguin above the title.
    drawPenguin(cx, EpdCanvas::H / 2 - 6);

    gfx_.textCentered(cx, EpdCanvas::H / 2 + 40, "MANDYS PENGUIN", C_BLACK, 4);
    drawHeart(cx, EpdCanvas::H / 2 + 92, 10);
    gfx_.textCentered(cx, EpdCanvas::H / 2 + 118, "Made for Mandy", C_RED, 2);

    present();
}

void DisplayManager::showError(const String& message) {
    if (!ready_) return;
    Serial.print("[DISPLAY] Error: ");
    Serial.println(message);

    gfx_.clear(C_WHITE);
    drawFrame();
    const int cx = EpdCanvas::W / 2;

    gfx_.fillRect(cx - 150, EpdCanvas::H / 2 - 60, 300, 40, C_RED);
    gfx_.textCentered(cx, EpdCanvas::H / 2 - 52, "SIGNAL LOST", C_WHITE, 3);

    gfx_.textCentered(cx, EpdCanvas::H / 2 + 4, "Couldnt reach Luna just now.", C_BLACK, 2);
    gfx_.textCentered(cx, EpdCanvas::H / 2 + 28, "Showing her last known location.", C_BLACK, 2);

    present();
}

void DisplayManager::showAnimal(const Animal& animal) {
    if (!ready_) return;
    Serial.println("[DISPLAY] Render Luna");

    gfx_.clear(C_WHITE);
    drawFrame();
    drawHeader(animal);

    const int mapX = INSET;
    const int mapY = INSET + HEADER_H + 4;
    const int mapW = EpdCanvas::W - 2 * INSET;
    const int mapH = (EpdCanvas::H - INSET - FOOTER_H) - mapY;
    drawScene(animal, mapX, mapY, mapW, mapH);

    drawFooter(animal);
    present();
}

// ============================================================
// Frame + header
// ============================================================

void DisplayManager::drawFrame() {
    // Heavy outer rule + fine inner rule + red corner diamonds.
    gfx_.rect(BORDER, BORDER, EpdCanvas::W - 2 * BORDER, EpdCanvas::H - 2 * BORDER, C_BLACK, 3);
    gfx_.rect(BORDER + 5, BORDER + 5, EpdCanvas::W - 2 * BORDER - 10, EpdCanvas::H - 2 * BORDER - 10, C_BLACK, 1);

    const int corners[4][2] = {
        {BORDER + 6, BORDER + 6},
        {EpdCanvas::W - BORDER - 6, BORDER + 6},
        {BORDER + 6, EpdCanvas::H - BORDER - 6},
        {EpdCanvas::W - BORDER - 6, EpdCanvas::H - BORDER - 6},
    };
    for (auto& c : corners) {
        for (int dy = -4; dy <= 4; dy++) {
            int w = 4 - abs(dy);
            gfx_.hline(c[0] - w, c[1] + dy, 2 * w + 1, C_RED);
        }
    }
}

void DisplayManager::drawHeader(const Animal& animal) {
    const int x = INSET;
    const int y = INSET;

    gfx_.text(x, y + 4, "MANDYS PENGUIN", C_BLACK, 3);
    int tw = gfx_.textWidth("MANDYS PENGUIN", 3);
    drawHeart(x + tw + 14, y + 12, 7);

    gfx_.text(x, y + 34, "following one small wanderer of the sea", C_BLACK, 1);

    // Species badge (red pill with white text).
    char sp[40];
    animal.species.toCharArray(sp, sizeof(sp));
    for (char* p = sp; *p; p++) *p = toupper(*p);
    int bw = gfx_.textWidth(sp, 2) + 20;
    int bx = EpdCanvas::W - INSET - bw;
    gfx_.fillRect(bx, y + 2, bw, 26, C_RED);
    gfx_.text(bx + 10, y + 9, sp, C_WHITE, 2);

    // Double rule.
    int ry = y + HEADER_H - 6;
    gfx_.fillRect(x, ry, EpdCanvas::W - 2 * INSET, 2, C_BLACK);
    gfx_.hline(x, ry + 4, EpdCanvas::W - 2 * INSET, C_BLACK);
}

// ============================================================
// The magical polar scene
// ============================================================

void DisplayManager::drawScene(const Animal& animal, int mx, int my, int mw, int mh) {
    // Map frame.
    gfx_.rect(mx, my, mw, mh, C_BLACK, 2);

    const int ix = mx + 2, iy = my + 2, iw = mw - 4, ih = mh - 4;

    // Sea: yellow wash.
    gfx_.fillRect(ix, iy, iw, ih, C_YELLOW);

    // Faint graticule (dotted).
    for (int i = 1; i < 6; i++) {
        int gx = ix + iw * i / 6;
        for (int yy = iy; yy < iy + ih; yy += 9) gfx_.pixel(gx, yy, C_BLACK);
    }
    for (int i = 1; i < 4; i++) {
        int gy = iy + ih * i / 4;
        for (int xx = ix; xx < ix + iw; xx += 9) gfx_.pixel(xx, gy, C_BLACK);
    }

    // Stars over the upper sea.
    const int starX[9] = {60, 150, 240, 320, 430, 500, 560, 110, 380};
    const int starY[9] = {40,  70,  46,  92,  56,  100, 44,  120, 130};
    for (int i = 0; i < 9; i++) drawSparkle(ix + starX[i] % (iw - 40) + 20, iy + starY[i], (i % 3 == 0) ? 6 : 4);

    // Wave crests over upper ~60%.
    int yMax = iy + (int)(ih * 0.6f);
    int row = 0;
    for (int y = iy + 34; y < yMax; y += 30) {
        int off = (row % 2) * 26;
        for (int x = ix + 22 + off; x < ix + iw - 48; x += 52) {
            gfx_.line(x, y, x + 6, y - 3, C_BLACK);
            gfx_.line(x + 6, y - 3, x + 12, y, C_BLACK);
            gfx_.line(x + 12, y, x + 18, y + 3, C_BLACK);
            gfx_.line(x + 18, y + 3, x + 24, y, C_BLACK);
        }
        row++;
    }

    // Ice shelf across the bottom (white with a wavy top edge).
    int iceTop = iy + (int)(ih * 0.66f);
    gfx_.fillRect(ix, iceTop, iw, iy + ih - iceTop, C_WHITE);
    // Wavy top edge: draw a black scalloped line and clear a little above.
    int prevX = ix, prevY = iceTop;
    bool up = true;
    for (int x = ix; x <= ix + iw; x += 46) {
        int yy = iceTop + (up ? -6 : 8);
        gfx_.line(prevX, prevY, x, yy, C_BLACK, 2);
        prevX = x; prevY = yy; up = !up;
    }
    // Gentle contour lines on the ice.
    for (int c = 0; c < 2; c++) {
        int yBase = iceTop + 22 + c * 18;
        int px = ix + 20, py = yBase;
        for (int x = ix + 20; x < ix + iw - 20; x += 10) {
            int yy = yBase + (int)(sinf((x + c * 40) * 0.03f) * 3);
            gfx_.line(px, py, x, yy, C_BLACK);
            px = x; py = yy;
        }
    }

    // A couple of drifting ice floes.
    gfx_.fillEllipse(ix + iw / 5, iy + ih / 4, 30, 15, C_WHITE);
    gfx_.ellipse(ix + iw / 5, iy + ih / 4, 30, 15, C_BLACK);
    gfx_.fillEllipse(ix + (iw * 3) / 4, iy + ih / 3, 26, 13, C_WHITE);
    gfx_.ellipse(ix + (iw * 3) / 4, iy + ih / 3, 26, 13, C_BLACK);

    // Region label.
    gfx_.text(ix + 14, iy + 12, "SOUTHERN OCEAN", C_BLACK, 2);

    // ---- Luna's REAL journey, projected onto the chart ----
    // Bounding box over all waypoints (+ colony), with padding, mapped
    // into the open-water area of the map (above the ice shelf).
    double minLat = JOURNEY[0].lat, maxLat = JOURNEY[0].lat;
    double minLng = JOURNEY[0].lng, maxLng = JOURNEY[0].lng;
    for (int i = 0; i < JOURNEY_POINTS; i++) {
        minLat = min(minLat, JOURNEY[i].lat); maxLat = max(maxLat, JOURNEY[i].lat);
        minLng = min(minLng, JOURNEY[i].lng); maxLng = max(maxLng, JOURNEY[i].lng);
    }
    double padLat = (maxLat - minLat) * 0.12 + 0.01;
    double padLng = (maxLng - minLng) * 0.12 + 0.01;
    minLat -= padLat; maxLat += padLat;
    minLng -= padLng; maxLng += padLng;

    // Plot area: inside the frame, above the ice shelf.
    const int plotX = ix + 20, plotY = iy + 30;
    const int plotW = iw - 40, plotH = (iceTop - 12) - plotY;

    auto projX = [&](double lng) -> int {
        return plotX + (int)((lng - minLng) / (maxLng - minLng) * plotW);
    };
    // Higher latitude (less negative, further north) -> higher on screen.
    auto projY = [&](double lat) -> int {
        return plotY + (int)((maxLat - lat) / (maxLat - minLat) * plotH);
    };

    double dayNow = (double)animal.journeyDay;

    // The CURRENT trip's waypoint range (from colony departure to the
    // next colony arrival). We draw just this trip so the chart stays
    // readable instead of showing the whole year's spaghetti.
    int tripFirst, tripLast;
    Journey::currentTripRange(dayNow, tripFirst, tripLast);

    // Faint dotted line for the WHOLE current trip (where she'll go).
    for (int i = tripFirst + 1; i <= tripLast; i++) {
        int x0 = projX(JOURNEY[i - 1].lng), y0 = projY(JOURNEY[i - 1].lat);
        int x1 = projX(JOURNEY[i].lng),     y1 = projY(JOURNEY[i].lat);
        for (int s = 0; s <= 6; s++) {
            int dx = x0 + (x1 - x0) * s / 6;
            int dy = y0 + (y1 - y0) * s / 6;
            if (s % 2 == 0) gfx_.pixel(dx, dy, C_BLACK);
        }
    }

    // Her exact current position.
    int curX = projX(animal.location.longitude);
    int curY = projY(animal.location.latitude);

    // Solid trail from the trip's start up to her current position.
    int lastReached = Journey::waypointsReached(dayNow);
    if (lastReached < tripFirst) lastReached = tripFirst;
    int px = projX(JOURNEY[tripFirst].lng), py = projY(JOURNEY[tripFirst].lat);
    for (int i = tripFirst + 1; i <= lastReached && i <= tripLast; i++) {
        int nx = projX(JOURNEY[i].lng), ny = projY(JOURNEY[i].lat);
        gfx_.line(px, py, nx, ny, C_BLACK, 2);
        gfx_.fillCircle(px, py, 2, C_BLACK);
        px = nx; py = ny;
    }
    gfx_.line(px, py, curX, curY, C_BLACK, 2);

    // Colony marker (home dot).
    int homeX = projX(JOURNEY_COLONY_LNG), homeY = projY(JOURNEY_COLONY_LAT);
    gfx_.fillCircle(homeX, homeY, 3, C_WHITE);
    gfx_.circle(homeX, homeY, 4, C_BLACK);
    gfx_.text(homeX + 6, homeY - 3, "colony", C_BLACK, 1);

    // Luna herself. If she's ashore at the colony, she stands on the
    // home dot; otherwise she's out at her current sea position.
    if (animal.atColony) {
        drawPenguin(homeX, homeY - 2);
        drawMarker(homeX + 22, homeY - 20);
    } else {
        drawPenguin(curX, curY + 26);
        drawMarker(curX + 22, curY + 8);
    }

    // Compass + scale bar.
    drawCompass(mx + mw - 40, my + 42);
    int sbx = mx + mw - 132, sby = my + mh - 20;
    gfx_.fillRect(sbx - 3, sby - 10, 108, 20, C_WHITE);
    for (int i = 0; i < 4; i++) if (i % 2 == 0) gfx_.fillRect(sbx + i * 25, sby, 25, 3, C_BLACK);
    gfx_.rect(sbx, sby, 100, 3, C_BLACK, 1);
    gfx_.text(sbx - 2, sby - 12, "0", C_BLACK, 1);
    gfx_.textRight(sbx + 100, sby - 12, "500km", C_BLACK, 1);
}

// ============================================================
// The cute cartoon King Penguin (anchor at feet)
// ============================================================

void DisplayManager::drawPenguin(int px, int py) {
    const int bodyCy = py - 26, bodyRx = 24, bodyRy = 30;
    const int headCy = py - 62, headR = 19;

    // Feet (yellow).
    gfx_.fillEllipse(px - 9, py + 2, 7, 3, C_YELLOW);
    gfx_.ellipse(px - 9, py + 2, 7, 3, C_BLACK);
    gfx_.fillEllipse(px + 9, py + 2, 7, 3, C_YELLOW);
    gfx_.ellipse(px + 9, py + 2, 7, 3, C_BLACK);

    // Flippers (black) behind body.
    gfx_.fillTriangle(px - bodyRx + 3, bodyCy - 4, px - bodyRx - 8, bodyCy + 16, px - bodyRx + 4, bodyCy + 22, C_BLACK);
    gfx_.fillTriangle(px + bodyRx - 3, bodyCy - 4, px + bodyRx + 8, bodyCy + 16, px + bodyRx - 4, bodyCy + 22, C_BLACK);

    // Body (black egg) + white belly.
    gfx_.fillEllipse(px, bodyCy, bodyRx, bodyRy, C_BLACK);
    gfx_.fillEllipse(px, bodyCy + 2, bodyRx - 8, bodyRy - 7, C_WHITE);

    // Head (black) + white face mask.
    gfx_.fillCircle(px, headCy, headR, C_BLACK);
    gfx_.fillEllipse(px, headCy + 2, headR - 5, headR - 3, C_WHITE);

    // Ear-patches (yellow) hugging the head sides.
    gfx_.fillEllipse(px - headR + 4, headCy + 4, 4, 6, C_YELLOW);
    gfx_.fillEllipse(px + headR - 4, headCy + 4, 4, 6, C_YELLOW);

    // Eyes with highlights.
    gfx_.fillCircle(px - 6, headCy + 1, 3, C_BLACK);
    gfx_.fillCircle(px + 6, headCy + 1, 3, C_BLACK);
    gfx_.fillCircle(px - 5, headCy, 1, C_WHITE);
    gfx_.fillCircle(px + 7, headCy, 1, C_WHITE);

    // Beak (yellow triangle + red lower).
    gfx_.fillTriangle(px - 4, headCy + 7, px + 4, headCy + 7, px, headCy + 13, C_YELLOW);
    gfx_.fillTriangle(px - 2, headCy + 10, px + 2, headCy + 10, px, headCy + 13, C_RED);

    // Cheeks.
    gfx_.fillCircle(px - 11, headCy + 5, 2, C_YELLOW);
    gfx_.fillCircle(px + 11, headCy + 5, 2, C_YELLOW);
}

// ============================================================
// Small decorative pieces
// ============================================================

void DisplayManager::drawHeart(int cx, int cy, int s) {
    // Two lobes + a point, filled red.
    gfx_.fillCircle(cx - s / 2, cy, s / 2 + 1, C_RED);
    gfx_.fillCircle(cx + s / 2, cy, s / 2 + 1, C_RED);
    gfx_.fillTriangle(cx - s, cy + 1, cx + s, cy + 1, cx, cy + s + 2, C_RED);
}

void DisplayManager::drawMarker(int x, int y) {
    gfx_.fillEllipse(x, y + 2, 4, 2, C_BLACK);       // ground shadow dot
    gfx_.fillCircle(x, y - 11, 8, C_RED);
    gfx_.fillTriangle(x - 7, y - 8, x + 7, y - 8, x, y + 1, C_RED);
    gfx_.circle(x, y - 11, 8, C_BLACK);
    gfx_.fillCircle(x, y - 11, 3, C_WHITE);
}

void DisplayManager::drawSparkle(int cx, int cy, int s) {
    // White 4-point star with black outline.
    gfx_.fillTriangle(cx, cy - s, cx - s / 3, cy, cx + s / 3, cy, C_WHITE);
    gfx_.fillTriangle(cx, cy + s, cx - s / 3, cy, cx + s / 3, cy, C_WHITE);
    gfx_.fillTriangle(cx - s, cy, cx, cy - s / 3, cx, cy + s / 3, C_WHITE);
    gfx_.fillTriangle(cx + s, cy, cx, cy - s / 3, cx, cy + s / 3, C_WHITE);
    gfx_.line(cx, cy - s, cx + s, cy, C_BLACK);
    gfx_.line(cx + s, cy, cx, cy + s, C_BLACK);
    gfx_.line(cx, cy + s, cx - s, cy, C_BLACK);
    gfx_.line(cx - s, cy, cx, cy - s, C_BLACK);
}

void DisplayManager::drawCompass(int cx, int cy) {
    const int R = 15;
    gfx_.fillCircle(cx, cy, R + 3, C_WHITE);
    gfx_.circle(cx, cy, R + 3, C_BLACK);
    // N/S/E/W points; north red.
    gfx_.fillTriangle(cx, cy - R, cx - 3, cy, cx + 3, cy, C_RED);      // N
    gfx_.fillTriangle(cx, cy + R, cx - 3, cy, cx + 3, cy, C_BLACK);    // S
    gfx_.fillTriangle(cx - R, cy, cx, cy - 3, cx, cy + 3, C_BLACK);    // W
    gfx_.fillTriangle(cx + R, cy, cx, cy - 3, cx, cy + 3, C_BLACK);    // E
    gfx_.fillCircle(cx, cy, 2, C_BLACK);
    gfx_.text(cx - 3, cy - R - 12, "N", C_BLACK, 1);
}

// ============================================================
// Footer
// ============================================================

void DisplayManager::drawFooter(const Animal& animal) {
    const int x = INSET;
    const int y = EpdCanvas::H - INSET - FOOTER_H + 6;
    const int right = EpdCanvas::W - INSET;

    char buf[64];

    // Name (big) + species (right).
    animal.name.toCharArray(buf, sizeof(buf));
    // Title-case-ish: keep as-is but show large.
    gfx_.text(x, y, buf, C_BLACK, 3);

    char sp[40];
    animal.species.toCharArray(sp, sizeof(sp));
    gfx_.textRight(right, y + 4, sp, C_BLACK, 2);

    // Current phase of her year (left) + coordinates (right).
    char phase[40];
    animal.phaseLabel.toCharArray(phase, sizeof(phase));
    gfx_.text(x, y + 26, phase, C_BLACK, 2);

    formatCoords(animal.location, buf, sizeof(buf));
    gfx_.textRight(right, y + 26, buf, C_BLACK, 1);

    // Explanation line (small) — reads well in every phase and, crucially,
    // explains WHY she isn't moving during the ashore phases.
    animal.phaseExplain.toCharArray(buf, sizeof(buf));
    gfx_.text(x, y + 46, buf, C_BLACK, 1);

    // Divider.
    gfx_.hline(x, y + 62, EpdCanvas::W - 2 * INSET, C_BLACK);

    // Bottom row (left): a phase-aware counter.
    //  - Ashore/stationary: "Day N of moult" style, so it ticks daily
    //    even though she isn't moving.
    //  - At sea: her day within the year.
    if (animal.atColony) {
        // Ashore: a phase-day counter (ticks daily even while still),
        // and on the right the total distance she's travelled this year.
        snprintf(buf, sizeof(buf), "Day %d of %d ashore",
                 animal.phaseDay, animal.phaseTotalDays);
        gfx_.text(x, y + 72, buf, C_BLACK, 2);

        snprintf(buf, sizeof(buf), "%ld km this year",
                 (long)(animal.distanceTravelledKm + 0.5));
        gfx_.textRight(right, y + 72, buf, C_BLACK, 2);
    } else {
        // At sea: her day within the year + distance travelled.
        snprintf(buf, sizeof(buf), "Day %d of %d",
                 animal.journeyDay, animal.journeyTotalDays);
        gfx_.text(x, y + 72, buf, C_BLACK, 2);

        snprintf(buf, sizeof(buf), "%ld km travelled",
                 (long)(animal.distanceTravelledKm + 0.5));
        gfx_.textRight(right, y + 72, buf, C_BLACK, 2);
    }

    // "Made for Mandy" signature: its own centered line at the very
    // bottom, small, so it has room and never overlaps the row above.
    const char* sig = "Made for Mandy";
    int sigW = gfx_.textWidth(sig, 1);
    int sigX = EpdCanvas::W / 2 - (sigW + 12) / 2;
    gfx_.text(sigX, y + 88, sig, C_BLACK, 1);
    drawHeart(sigX + sigW + 8, y + 91, 4);
}

// ============================================================
// Formatting helpers (font uses '^' as the degree sign)
// ============================================================

void DisplayManager::formatCoords(const AnimalLocation& loc, char* out, size_t n) {
    double lat = loc.latitude, lng = loc.longitude;
    char ns = lat >= 0 ? 'N' : 'S';
    char ew = lng >= 0 ? 'E' : 'W';
    snprintf(out, n, "%.4f^ %c  %.4f^ %c", fabs(lat), ns, fabs(lng), ew);
}

void DisplayManager::formatAge(unsigned long minutes, char* out, size_t n) {
    if (minutes < 60) {
        snprintf(out, n, "%lu min ago", minutes);
    } else {
        unsigned long h = minutes / 60, m = minutes % 60;
        if (m) snprintf(out, n, "%lu h %lu min ago", h, m);
        else   snprintf(out, n, "%lu h ago", h);
    }
}
