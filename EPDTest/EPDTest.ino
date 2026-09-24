/*****************************************************************************
 * Mandy Wildlife Tracker — e-Paper four-colour TEST sketch
 * ---------------------------------------------------------------------------
 * Purpose: prove the physical Waveshare 5.83" e-Paper (G) panel works,
 * wired to the ESP32-S3 Mini with THIS project's pins. This is the
 * README "Step 5" milestone: show WHITE / BLACK / RED / YELLOW.
 *
 * This is a STANDALONE sketch. It carries its own copy of the vendored
 * Waveshare driver (EPD_5in83g + DEV_Config), so you can flash it on its
 * own without touching the main MandyWildlife app.
 *
 * Board:  ESP32S3 Dev Module        Serial: 115200
 *
 * Wiring (fixed — see DEV_Config.h):
 *   DIN=GPIO11  CLK=GPIO12  CS=GPIO10  DC=GPIO13
 *   RST=GPIO9   BUSY=GPIO4  PWR=GPIO5
 *
 * What you should see:
 *   1) Panel clears to white.
 *   2) A short pause.
 *   3) Four vertical colour bands, left -> right:
 *          WHITE | YELLOW | RED | BLACK
 *      plus a black border and two diagonal lines, so you can confirm
 *      full 648x480 coverage and all four colours.
 *   4) Panel goes to sleep (image is retained).
 *
 * If the panel stays blank: check power (PWR/5V), the BUSY line, and
 * that the board is "ESP32S3 Dev Module". The Serial log narrates each
 * step at 115200 baud.
 *****************************************************************************/

#include "EPD_5in83g.h"

// Packed 4-colour framebuffer: 2 bits per pixel, 4 pixels per byte.
// 648 / 4 * 480 = 77,760 bytes.
static const uint32_t kBytesPerRow = (EPD_5IN83G_WIDTH % 4 == 0)
                                         ? (EPD_5IN83G_WIDTH / 4)
                                         : (EPD_5IN83G_WIDTH / 4 + 1);
static const uint32_t kBufferSize = kBytesPerRow * EPD_5IN83G_HEIGHT;

static uint8_t *frame = nullptr;

// Set one pixel (x,y) to a 2-bit colour code in the packed buffer.
static void setPixel(int x, int y, uint8_t color)
{
    if (x < 0 || x >= EPD_5IN83G_WIDTH || y < 0 || y >= EPD_5IN83G_HEIGHT) return;
    const uint32_t byteIndex = (uint32_t)y * kBytesPerRow + (x >> 2);
    const int shift = (3 - (x & 0x03)) * 2;      // MSB-first packing
    uint8_t b = frame[byteIndex];
    b &= ~(0x03 << shift);
    b |= (color & 0x03) << shift;
    frame[byteIndex] = b;
}

// Fill the whole buffer with one colour code.
static void fillAll(uint8_t color)
{
    const uint8_t packed = (color << 6) | (color << 4) | (color << 2) | color;
    memset(frame, packed, kBufferSize);
}

static void drawTestPattern()
{
    // Four vertical bands across the full width.
    const int bandW = EPD_5IN83G_WIDTH / 4;
    const uint8_t bands[4] = {
        EPD_5IN83G_WHITE, EPD_5IN83G_YELLOW, EPD_5IN83G_RED, EPD_5IN83G_BLACK
    };
    for (int y = 0; y < EPD_5IN83G_HEIGHT; y++) {
        for (int x = 0; x < EPD_5IN83G_WIDTH; x++) {
            int band = x / bandW;
            if (band > 3) band = 3;
            setPixel(x, y, bands[band]);
        }
    }

    // Black border to confirm the panel reaches all four edges.
    for (int x = 0; x < EPD_5IN83G_WIDTH; x++) {
        setPixel(x, 0, EPD_5IN83G_BLACK);
        setPixel(x, 1, EPD_5IN83G_BLACK);
        setPixel(x, EPD_5IN83G_HEIGHT - 1, EPD_5IN83G_BLACK);
        setPixel(x, EPD_5IN83G_HEIGHT - 2, EPD_5IN83G_BLACK);
    }
    for (int y = 0; y < EPD_5IN83G_HEIGHT; y++) {
        setPixel(0, y, EPD_5IN83G_BLACK);
        setPixel(1, y, EPD_5IN83G_BLACK);
        setPixel(EPD_5IN83G_WIDTH - 1, y, EPD_5IN83G_BLACK);
        setPixel(EPD_5IN83G_WIDTH - 2, y, EPD_5IN83G_BLACK);
    }

    // Two diagonals in red so we can see the full area is addressed.
    for (int x = 0; x < EPD_5IN83G_WIDTH; x++) {
        int y = (int)((long)x * (EPD_5IN83G_HEIGHT - 1) / (EPD_5IN83G_WIDTH - 1));
        setPixel(x, y, EPD_5IN83G_RED);
        setPixel(x, EPD_5IN83G_HEIGHT - 1 - y, EPD_5IN83G_RED);
    }
}

void setup()
{
    DEV_Module_Init();   // starts Serial at 115200 and configures pins
    delay(200);
    Serial.println();
    Serial.println("=== Mandy Wildlife — 5.83\" G four-colour test ===");
    Serial.printf("Framebuffer: %u bytes (%d x %d, 2bpp)\n",
                  (unsigned)kBufferSize, EPD_5IN83G_WIDTH, EPD_5IN83G_HEIGHT);

    // Allocate the framebuffer. Prefer PSRAM (the S3 Mini has 8MB).
    frame = (uint8_t *)ps_malloc(kBufferSize);
    if (!frame) frame = (uint8_t *)malloc(kBufferSize);
    if (!frame) {
        Serial.println("FATAL: could not allocate framebuffer");
        while (true) delay(1000);
    }

    Serial.println("Init...");
    Serial.flush();
    EPD_5IN83G_Init();

    // ---- Stage 1: prove the PANEL + DRIVER with Waveshare's own Clear ----
    // Clear() fills the whole screen with one colour and refreshes. If these
    // show, the hardware and vendored driver are 100% good, and any later
    // problem is in our framebuffer code.
    Serial.println("Stage 1a: Clear WHITE (whole screen should go white)");
    Serial.flush();
    EPD_5IN83G_Clear(EPD_5IN83G_WHITE);
    delay(3000);

    Serial.println("Stage 1b: Clear RED (whole screen should go red)");
    Serial.flush();
    EPD_5IN83G_Clear(EPD_5IN83G_RED);
    delay(3000);

    Serial.println("Stage 1c: Clear YELLOW");
    Serial.flush();
    EPD_5IN83G_Clear(EPD_5IN83G_YELLOW);
    delay(3000);

    Serial.println("Stage 1d: Clear BLACK");
    Serial.flush();
    EPD_5IN83G_Clear(EPD_5IN83G_BLACK);
    delay(3000);

    // ---- Stage 2: our own framebuffer test pattern ----
    Serial.println("Stage 2: re-init for framebuffer pattern");
    Serial.flush();
    EPD_5IN83G_Init();

    Serial.println("Stage 2: fillAll white...");
    Serial.flush();
    fillAll(EPD_5IN83G_WHITE);
    Serial.println("Stage 2: fillAll done, drawing pattern...");
    Serial.flush();
    drawTestPattern();
    Serial.println("Stage 2: pattern built, refreshing...");
    Serial.flush();
    EPD_5IN83G_Display(frame);

    Serial.println("Done. Sleeping panel (image is retained).");
    Serial.flush();
    EPD_5IN83G_Sleep();

    free(frame);
    frame = nullptr;
}

void loop()
{
    // Nothing to do; the image stays on the panel after sleep.
    delay(5000);
}
