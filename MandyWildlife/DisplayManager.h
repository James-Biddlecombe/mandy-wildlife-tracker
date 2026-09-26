#pragma once

#include <Arduino.h>
#include "models.h"
#include "src/epd/EpdCanvas.h"

// ============================================================
// DisplayManager
// ------------------------------------------------------------
// Renders the Mandy's Penguin UI onto the Waveshare 5.83" (G)
// panel. Mirrors the browser simulator: a framed field-guide
// plate with a polar chart, Luna the King Penguin, her journey,
// and a footer showing the current phase of her year. Fully offline.
//
// It draws into an EpdCanvas (4-colour framebuffer) then pushes
// the buffer to the vendored Waveshare driver for a full refresh.
// ============================================================

class DisplayManager {
public:
    bool begin();

    void clear();

    void showBootScreen();
    void showAnimal(const Animal& animal);
    void showError(const String& message);

    void sleep();

private:
    EpdCanvas gfx_;
    bool ready_ = false;

    // Push the framebuffer to the panel (full refresh).
    void present();

    // Scene building blocks (mirror the simulator regions).
    void drawFrame();
    void drawHeader(const Animal& animal);
    void drawScene(const Animal& animal, int mapX, int mapY, int mapW, int mapH);
    void drawFooter(const Animal& animal);

    // Decorative pieces.
    void drawPenguin(int px, int py);
    void drawHeart(int cx, int cy, int s);
    void drawCompass(int cx, int cy);
    void drawMarker(int x, int y);
    void drawSparkle(int cx, int cy, int s);

    // Helpers.
    void formatCoords(const AnimalLocation& loc, char* out, size_t n);
    void formatAge(unsigned long minutes, char* out, size_t n);
};
