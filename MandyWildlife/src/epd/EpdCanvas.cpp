#include "EpdCanvas.h"

// ============================================================
// Packed 2-bit framebuffer helpers
// ============================================================

EpdCanvas::EpdCanvas() {}

bool EpdCanvas::begin() {
    buf_ = (uint8_t*) ps_malloc(BUFFER_SIZE);
    if (!buf_) buf_ = (uint8_t*) malloc(BUFFER_SIZE);
    if (!buf_) return false;
    clear(C_WHITE);
    return true;
}

void EpdCanvas::clear(uint8_t color) {
    if (!buf_) return;
    const uint8_t packed = (color << 6) | (color << 4) | (color << 2) | color;
    memset(buf_, packed, BUFFER_SIZE);
}

void EpdCanvas::pixel(int x, int y, uint8_t color) {
    if (!buf_) return;
    if (x < 0 || x >= W || y < 0 || y >= H) return;
    const int byteIndex = y * BYTES_PER_ROW + (x >> 2);
    const int shift = (3 - (x & 0x03)) * 2;      // MSB-first packing
    uint8_t b = buf_[byteIndex];
    b &= ~(0x03 << shift);
    b |= (color & 0x03) << shift;
    buf_[byteIndex] = b;
}

void EpdCanvas::hline(int x, int y, int w, uint8_t color) {
    for (int i = 0; i < w; i++) pixel(x + i, y, color);
}

void EpdCanvas::vline(int x, int y, int h, uint8_t color) {
    for (int i = 0; i < h; i++) pixel(x, y + i, color);
}

void EpdCanvas::fillRect(int x, int y, int w, int h, uint8_t color) {
    for (int j = 0; j < h; j++) hline(x, y + j, w, color);
}

void EpdCanvas::rect(int x, int y, int w, int h, uint8_t color, int thickness) {
    for (int t = 0; t < thickness; t++) {
        hline(x, y + t, w, color);
        hline(x, y + h - 1 - t, w, color);
        vline(x + t, y, h, color);
        vline(x + w - 1 - t, y, h, color);
    }
}

void EpdCanvas::line(int x0, int y0, int x1, int y1, uint8_t color, int thickness) {
    // Bresenham, with optional thickness (draws a small block per step).
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;
    const int t = thickness / 2;
    while (true) {
        if (thickness <= 1) {
            pixel(x0, y0, color);
        } else {
            for (int a = -t; a <= t; a++)
                for (int b = -t; b <= t; b++)
                    pixel(x0 + a, y0 + b, color);
        }
        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void EpdCanvas::fillCircle(int cx, int cy, int r, uint8_t color) {
    for (int y = -r; y <= r; y++) {
        int span = (int)(sqrtf((float)(r * r - y * y)) + 0.5f);
        hline(cx - span, cy + y, 2 * span + 1, color);
    }
}

void EpdCanvas::circle(int cx, int cy, int r, uint8_t color) {
    // Midpoint circle outline.
    int x = r, y = 0, err = 1 - r;
    while (x >= y) {
        pixel(cx + x, cy + y, color); pixel(cx + y, cy + x, color);
        pixel(cx - y, cy + x, color); pixel(cx - x, cy + y, color);
        pixel(cx - x, cy - y, color); pixel(cx - y, cy - x, color);
        pixel(cx + y, cy - x, color); pixel(cx + x, cy - y, color);
        y++;
        if (err < 0) err += 2 * y + 1;
        else { x--; err += 2 * (y - x) + 1; }
    }
}

void EpdCanvas::fillEllipse(int cx, int cy, int rx, int ry, uint8_t color) {
    if (rx <= 0 || ry <= 0) return;
    for (int y = -ry; y <= ry; y++) {
        float fy = (float)y / ry;
        int span = (int)(rx * sqrtf(fmaxf(0.0f, 1.0f - fy * fy)) + 0.5f);
        hline(cx - span, cy + y, 2 * span + 1, color);
    }
}

void EpdCanvas::ellipse(int cx, int cy, int rx, int ry, uint8_t color) {
    // Simple outline by sampling the perimeter.
    int prevX = cx + rx, prevY = cy;
    for (int a = 1; a <= 72; a++) {
        float t = a * (2.0f * PI / 72.0f);
        int x = cx + (int)(rx * cosf(t) + 0.5f);
        int y = cy + (int)(ry * sinf(t) + 0.5f);
        line(prevX, prevY, x, y, color);
        prevX = x; prevY = y;
    }
}

void EpdCanvas::fillTriangle(int x0, int y0, int x1, int y1, int x2, int y2, uint8_t color) {
    // Sort by y.
    if (y0 > y1) { int tx=x0;x0=x1;x1=tx; int ty=y0;y0=y1;y1=ty; }
    if (y0 > y2) { int tx=x0;x0=x2;x2=tx; int ty=y0;y0=y2;y2=ty; }
    if (y1 > y2) { int tx=x1;x1=x2;x2=tx; int ty=y1;y1=y2;y2=ty; }
    if (y2 == y0) return;

    for (int y = y0; y <= y2; y++) {
        bool secondHalf = y > y1 || y1 == y0;
        int segH = secondHalf ? (y2 - y1) : (y1 - y0);
        if (segH == 0) segH = 1;
        float alpha = (float)(y - y0) / (y2 - y0);
        float beta  = (float)(y - (secondHalf ? y1 : y0)) / segH;
        int ax = x0 + (int)((x2 - x0) * alpha);
        int bx = secondHalf ? (x1 + (int)((x2 - x1) * beta))
                            : (x0 + (int)((x1 - x0) * beta));
        if (ax > bx) { int t = ax; ax = bx; bx = t; }
        hline(ax, y, bx - ax + 1, color);
    }
}

// ============================================================
// Compact 5x7 font
// ------------------------------------------------------------
// Each glyph is 5 columns wide, 7 rows tall. Stored as 5 bytes,
// one per column, low 7 bits = rows top->bottom. Advance = 6px.
// ============================================================

struct Glyph { char c; uint8_t col[5]; };

// Bit r (0=top) of each column byte lights a pixel.
static const Glyph FONT[] = {
    {' ', {0x00,0x00,0x00,0x00,0x00}},
    {'!', {0x00,0x00,0x5F,0x00,0x00}},
    {'\'',{0x00,0x03,0x00,0x00,0x00}},
    {',', {0x00,0x50,0x30,0x00,0x00}},
    {'-', {0x08,0x08,0x08,0x08,0x08}},
    {'.', {0x00,0x60,0x60,0x00,0x00}},
    {':', {0x00,0x36,0x36,0x00,0x00}},
    {'0', {0x3E,0x51,0x49,0x45,0x3E}},
    {'1', {0x00,0x42,0x7F,0x40,0x00}},
    {'2', {0x42,0x61,0x51,0x49,0x46}},
    {'3', {0x21,0x41,0x45,0x4B,0x31}},
    {'4', {0x18,0x14,0x12,0x7F,0x10}},
    {'5', {0x27,0x45,0x45,0x45,0x39}},
    {'6', {0x3C,0x4A,0x49,0x49,0x30}},
    {'7', {0x01,0x71,0x09,0x05,0x03}},
    {'8', {0x36,0x49,0x49,0x49,0x36}},
    {'9', {0x06,0x49,0x49,0x29,0x1E}},
    {'A', {0x7E,0x11,0x11,0x11,0x7E}},
    {'B', {0x7F,0x49,0x49,0x49,0x36}},
    {'C', {0x3E,0x41,0x41,0x41,0x22}},
    {'D', {0x7F,0x41,0x41,0x22,0x1C}},
    {'E', {0x7F,0x49,0x49,0x49,0x41}},
    {'F', {0x7F,0x09,0x09,0x09,0x01}},
    {'G', {0x3E,0x41,0x49,0x49,0x7A}},
    {'H', {0x7F,0x08,0x08,0x08,0x7F}},
    {'I', {0x00,0x41,0x7F,0x41,0x00}},
    {'J', {0x20,0x40,0x41,0x3F,0x01}},
    {'K', {0x7F,0x08,0x14,0x22,0x41}},
    {'L', {0x7F,0x40,0x40,0x40,0x40}},
    {'M', {0x7F,0x02,0x0C,0x02,0x7F}},
    {'N', {0x7F,0x04,0x08,0x10,0x7F}},
    {'O', {0x3E,0x41,0x41,0x41,0x3E}},
    {'P', {0x7F,0x09,0x09,0x09,0x06}},
    {'Q', {0x3E,0x41,0x51,0x21,0x5E}},
    {'R', {0x7F,0x09,0x19,0x29,0x46}},
    {'S', {0x46,0x49,0x49,0x49,0x31}},
    {'T', {0x01,0x01,0x7F,0x01,0x01}},
    {'U', {0x3F,0x40,0x40,0x40,0x3F}},
    {'V', {0x1F,0x20,0x40,0x20,0x1F}},
    {'W', {0x7F,0x20,0x18,0x20,0x7F}},
    {'X', {0x63,0x14,0x08,0x14,0x63}},
    {'Y', {0x03,0x04,0x78,0x04,0x03}},
    {'Z', {0x61,0x51,0x49,0x45,0x43}},
    {'a', {0x20,0x54,0x54,0x54,0x78}},
    {'b', {0x7F,0x48,0x44,0x44,0x38}},
    {'c', {0x38,0x44,0x44,0x44,0x20}},
    {'d', {0x38,0x44,0x44,0x48,0x7F}},
    {'e', {0x38,0x54,0x54,0x54,0x18}},
    {'f', {0x08,0x7E,0x09,0x01,0x02}},
    {'g', {0x0C,0x52,0x52,0x52,0x3E}},
    {'h', {0x7F,0x08,0x04,0x04,0x78}},
    {'i', {0x00,0x44,0x7D,0x40,0x00}},
    {'j', {0x20,0x40,0x44,0x3D,0x00}},
    {'k', {0x7F,0x10,0x28,0x44,0x00}},
    {'l', {0x00,0x41,0x7F,0x40,0x00}},
    {'m', {0x7C,0x04,0x18,0x04,0x78}},
    {'n', {0x7C,0x08,0x04,0x04,0x78}},
    {'o', {0x38,0x44,0x44,0x44,0x38}},
    {'p', {0x7C,0x14,0x14,0x14,0x08}},
    {'q', {0x08,0x14,0x14,0x18,0x7C}},
    {'r', {0x7C,0x08,0x04,0x04,0x08}},
    {'s', {0x48,0x54,0x54,0x54,0x20}},
    {'t', {0x04,0x3F,0x44,0x40,0x20}},
    {'u', {0x3C,0x40,0x40,0x20,0x7C}},
    {'v', {0x1C,0x20,0x40,0x20,0x1C}},
    {'w', {0x3C,0x40,0x30,0x40,0x3C}},
    {'x', {0x44,0x28,0x10,0x28,0x44}},
    {'y', {0x0C,0x50,0x50,0x50,0x3C}},
    {'z', {0x44,0x64,0x54,0x4C,0x44}},
    // Degree sign mapped to '^' so we can print coordinates like 54^ S.
    {'^', {0x00,0x07,0x05,0x07,0x00}},
};

static const Glyph* findGlyph(char c) {
    for (unsigned i = 0; i < sizeof(FONT) / sizeof(FONT[0]); i++)
        if (FONT[i].c == c) return &FONT[i];
    return nullptr;
}

void EpdCanvas::drawGlyph(int x, int y, char c, uint8_t color, int scale) {
    const Glyph* g = findGlyph(c);
    if (!g) return;
    for (int col = 0; col < 5; col++) {
        uint8_t bits = g->col[col];
        for (int row = 0; row < 7; row++) {
            if (bits & (1 << row)) {
                if (scale <= 1) {
                    pixel(x + col, y + row, color);
                } else {
                    fillRect(x + col * scale, y + row * scale, scale, scale, color);
                }
            }
        }
    }
}

int EpdCanvas::text(int x, int y, const char* s, uint8_t color, int scale) {
    int cx = x;
    const int adv = 6 * scale;
    for (const char* p = s; *p; p++) {
        drawGlyph(cx, y, *p, color, scale);
        cx += adv;
    }
    return cx;
}

int EpdCanvas::textWidth(const char* s, int scale) const {
    int n = 0;
    for (const char* p = s; *p; p++) n++;
    return n * 6 * scale;
}

void EpdCanvas::textCentered(int cx, int y, const char* s, uint8_t color, int scale) {
    text(cx - textWidth(s, scale) / 2, y, s, color, scale);
}

void EpdCanvas::textRight(int xRight, int y, const char* s, uint8_t color, int scale) {
    text(xRight - textWidth(s, scale), y, s, color, scale);
}
