/*****************************************************************************
 * Mandy Wildlife — BUSY / wiring diagnostic
 * ---------------------------------------------------------------------------
 * The main test hangs inside the panel's Init() waiting for BUSY to go
 * idle. This tiny sketch does NOT talk to the panel at all — it just:
 *   1) powers the panel (PWR high),
 *   2) pulses RST like a real reset,
 *   3) prints the raw BUSY pin level continuously.
 *
 * How to read it:
 *   - A healthy panel: after the reset pulse, BUSY should settle and you'll
 *     see it sit at a steady level (and briefly toggle during the reset).
 *   - If BUSY reads a FIXED value that never changes no matter what — and
 *     especially if it stays LOW (0) — the BUSY wire (GPIO4) is likely
 *     disconnected, on the wrong pin, or the panel isn't powered.
 *
 * Wiring under test (from DEV_Config.h):
 *   DIN=11 CLK=12 CS=10 DC=13 RST=9 BUSY=4 PWR=5
 *
 * Board: ESP32S3 Dev Module, Serial 115200.
 *****************************************************************************/

#define PIN_DIN  11
#define PIN_CLK  12
#define PIN_CS   10
#define PIN_DC   13
#define PIN_RST  9
#define PIN_BUSY 6
#define PIN_PWR  5

void setup()
{
    Serial.begin(115200);
    delay(400);
    Serial.println();
    Serial.println("=== BUSY / wiring diagnostic ===");

    pinMode(PIN_PWR, OUTPUT);
    pinMode(PIN_RST, OUTPUT);
    pinMode(PIN_DC, OUTPUT);
    pinMode(PIN_CS, OUTPUT);
    pinMode(PIN_CLK, OUTPUT);
    pinMode(PIN_DIN, OUTPUT);
    // Use INPUT_PULLUP as a probe: if the BUSY wire is DISCONNECTED,
    // the internal pull-up makes GPIO4 read 1. If it still reads 0, the
    // pin is being actively driven/held low (real connection, or wrong pin
    // shorted low). This distinguishes "dead wire" from "stuck panel".
    pinMode(PIN_BUSY, INPUT_PULLUP);

    // Power the panel on.
    digitalWrite(PIN_PWR, HIGH);
    digitalWrite(PIN_CS, HIGH);
    Serial.println("PWR high, waiting 300ms...");
    delay(300);

    Serial.printf("BUSY right after power: %d\n", digitalRead(PIN_BUSY));

    // Do the same reset pulse the driver uses, logging BUSY around it.
    digitalWrite(PIN_RST, HIGH); delay(20);
    Serial.printf("BUSY (RST high):  %d\n", digitalRead(PIN_BUSY));
    digitalWrite(PIN_RST, LOW);  delay(2);
    Serial.printf("BUSY (RST low):   %d\n", digitalRead(PIN_BUSY));
    digitalWrite(PIN_RST, HIGH); delay(20);
    Serial.printf("BUSY (RST high2): %d\n", digitalRead(PIN_BUSY));

    Serial.println("Now streaming BUSY level. Try gently wiggling the BUSY");
    Serial.println("and PWR/VCC/GND jumpers — the number should be stable if");
    Serial.println("wired well, and may jump around if a wire is loose.");
}

void loop()
{
    static uint32_t n = 0;
    Serial.printf("[%lu] BUSY = %d\n", (unsigned long)n++, digitalRead(PIN_BUSY));
    delay(500);
}
