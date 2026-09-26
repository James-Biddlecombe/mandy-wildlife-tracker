#pragma once

// ============================================================
// EpdCanvas — a tiny 4-colour framebuffer + drawing layer for
// the Waveshare 5.83" (G) panel.
//
// This is the firmware equivalent of the browser simulator's
// drawing helpers: a packed 2-bits-per-pixel buffer plus simple
// primitives (pixel/line/rect/circle/ellipse) and a compact
// built-in font. DisplayManager draws the Luna scene with it,
// then hands the buffer to the vendored Waveshare driver.
//
// Colours match the panel codes: black=0, white=1, yellow=2, red=3.
// ============================================================

#include <Arduino.h>
#include "EPD_5in83g.h"

// Convenience colour aliases (same values as the driver's macros).
enum : uint8_t {
    C_BLACK  = EPD_5IN83G_BLACK,
    C_WHITE  = EPD_5IN83G_WHITE,
    C_YELLOW = EPD_5IN83G_YELLOW,
    C_RED    = EPD_5IN83G_RED,
};

class EpdCanvas {
public:
    static constexpr int W = EPD_5IN83G_WIDTH;   // 648
    static constexpr int H = EPD_5IN83G_HEIGHT;  // 480
    static constexpr int BYTES_PER_ROW = W / 4;  // 162
    static constexpr int BUFFER_SIZE = BYTES_PER_ROW * H; // 77,760

    EpdCanvas();

    // Allocate the framebuffer (PSRAM preferred). false if it fails.
    bool begin();
    bool ready() const { return buf_ != nullptr; }
    uint8_t* buffer() { return buf_; }

    // --- Whole-canvas ---
    void clear(uint8_t color = C_WHITE);

    // --- Primitives (all clipped to the canvas) ---
    void pixel(int x, int y, uint8_t color);
    void hline(int x, int y, int w, uint8_t color);
    void vline(int x, int y, int h, uint8_t color);
    void fillRect(int x, int y, int w, int h, uint8_t color);
    void rect(int x, int y, int w, int h, uint8_t color, int thickness = 1);
    void line(int x0, int y0, int x1, int y1, uint8_t color, int thickness = 1);
    void circle(int cx, int cy, int r, uint8_t color);        // outline
    void fillCircle(int cx, int cy, int r, uint8_t color);
    void fillEllipse(int cx, int cy, int rx, int ry, uint8_t color);
    void ellipse(int cx, int cy, int rx, int ry, uint8_t color);
    void fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color);

    // --- Text (compact 5x7 font, integer scaling) ---
    // Returns x advance end. Left-aligned at (x, y=top).
    int  text(int x, int y, const char* s, uint8_t color, int scale = 1);
    int  textWidth(const char* s, int scale = 1) const;
    void textCentered(int cx, int y, const char* s, uint8_t color, int scale = 1);
    void textRight(int xRight, int y, const char* s, uint8_t color, int scale = 1);

private:
    uint8_t* buf_ = nullptr;
    void drawGlyph(int x, int y, char c, uint8_t color, int scale);
};
