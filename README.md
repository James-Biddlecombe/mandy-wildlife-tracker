Mandy Wildlife Tracker — Full Project Summary
1. Project goal

Build a polished desk-mounted Wi-Fi wildlife tracker as a gift for Mandy.

The device will use:

ESP32-S3 Mini
Waveshare 5.83" 4-colour e-paper display
Wi-Fi
A real wildlife tracking data source such as Movebank
An attractive illustrated map rather than a basic Google-style map
Periodic updates showing the animal's latest known location

The final device should feel like a small dedicated wildlife appliance rather than an electronics prototype.

The display should show things such as:

Animal name
Species
Current/latest known location
Map
Animal's route/history
Current position marker
Coordinates
Last signal/update time
Distance travelled
Compass
Map labels
Attractive wildlife/naturalist styling
"Made for Mandy"

Mandy's favourite animals include:

Penguins
Owls
Sloths
Pangolins
Tigers
2. Hardware
ESP32

Exact board:

Waveshare ESP32-S3 Mini

ESP32-S3
8 MB Flash
8 MB PSRAM
Micro USB/USB-C programming connection depending on board revision
South African supplier: Micro Robotics
SKU: W33549

Arduino IDE board selection:

ESP32S3 Dev Module

Do not select Arduino Nano ESP32.

3. E-paper display

Exact display:

Waveshare 5.83" E-Paper HAT (G)

Specifications:

5.83"
648 × 480 pixels
4-colour
Black
White
Red
Yellow
SPI interface
Driver HAT

Official Waveshare product:

https://www.waveshare.com/5.83inch-e-paper-g.htm

The exact display physically received is the 5.83" G version with Driver HAT.

The project should use the actual Waveshare G driver/example, rather than blindly assuming a generic GxEPD2 configuration.

4. ⭐ AUTHORITATIVE PHYSICAL PIN MAPPING

This is the most important hardware information.

The following is the actual wiring currently used on the physical device and must be treated as authoritative.

5.83" G Driver HAT	ESP32-S3 Mini
VCC	3V3
GND	GND
DIN	GPIO11
CLK / SCLK	GPIO12
CS	GPIO10
DC	GPIO13
RST	GPIO9
BUSY	GPIO4
PWR	GPIO5

Therefore the software configuration must use:

constexpr int EPD_DIN  = 11;
constexpr int EPD_CLK  = 12;
constexpr int EPD_CS   = 10;
constexpr int EPD_DC   = 13;
constexpr int EPD_RST  = 9;
constexpr int EPD_BUSY = 4;
constexpr int EPD_PWR  = 5;
Critical rules

Do not change this wiring.

In particular:

RST is GPIO9
DIN is GPIO11
CLK is GPIO12
CS is GPIO10
DC is GPIO13
BUSY is GPIO4
PWR is GPIO5
Do not use GPIO8
Do not use GPIO14

If an example/library uses different GPIOs, modify the software pin definitions to match the physical wiring.

Do not tell the user to rewire the hardware simply because a library example has different defaults.

5. Current wiring state

The display has already been physically wired using Dupont jumper wires.

No soldering is currently required for the prototype.

The user has:

ESP32-S3 Mini
5.83" G e-paper display + Driver HAT
Male-female jumper wires
5V 4A USB-C power supply

The ESP32 has already successfully connected to a PC and uploaded firmware.

The ESP32's blue LED was observed blinking.

6. Arduino environment

Development environment:

Arduino IDE
ESP32 Arduino core from Espressif Systems

Board package URL:

https://espressif.github.io/arduino-esp32/package_esp32_index.json

Board:

ESP32S3 Dev Module

Serial:

115200

The ESP32 has already successfully uploaded code.

7. Current project repository

GitHub repository:

https://github.com/James-Biddlecombe/mandy-wildlife-tracker

The repository should be treated as the source of truth for the current implementation.

The desired project structure is approximately:

mandy-wildlife-tracker/
├── MandyWildlife.ino
├── config.h
├── models.h
├── App.h
├── App.cpp
├── WiFiManager.h
├── WiFiManager.cpp
├── Tracker.h
├── Tracker.cpp
├── DisplayManager.h
├── DisplayManager.cpp
├── README.md
├── .gitignore
└── docs/
    ├── architecture.md
    ├── wiring.md
    └── display-design.md
8. Current software architecture

The project is split into several logical components.

App

Main application controller.

Responsible for:

Startup
Wi-Fi
Tracker
Display
Periodic updates
WiFiManager

Responsible for:

Connecting to Wi-Fi
Monitoring connection
Eventually Wi-Fi provisioning
Tracker

Responsible for:

Getting animal data
Currently mock data
Eventually Movebank/API data
DisplayManager

Responsible for:

E-paper initialization
Boot screen
Wi-Fi status
Animal information
Map rendering
Error screens
Display sleep
models.h

Contains the data structures used between components.

9. Current data model

Current AnimalLocation:

struct AnimalLocation {
    double latitude;
    double longitude;
    String locationName;
    unsigned long timestamp;
    bool valid;
};

Current Animal:

struct Animal {
    String name;
    String species;
    String description;
    AnimalLocation location;
    double distanceTravelledKm;
};

Current tracking status:

struct TrackingStatus {
    bool connected;
    bool dataAvailable;
    String lastError;
    unsigned long lastUpdate;
};
10. Current configuration

Current update interval:

constexpr unsigned long TRACKING_UPDATE_INTERVAL_MS =
    30UL * 60UL * 1000UL;

So currently:

30 minutes

Mock tracking is currently enabled:

constexpr bool MOCK_TRACKING = true;

Display resolution:

constexpr int DISPLAY_WIDTH  = 648;
constexpr int DISPLAY_HEIGHT = 480;

Serial:

constexpr unsigned long SERIAL_BAUD = 115200;

The pin definitions should be added to config.h:

constexpr int EPD_DIN  = 11;
constexpr int EPD_CLK  = 12;
constexpr int EPD_CS   = 10;
constexpr int EPD_DC   = 13;
constexpr int EPD_RST  = 9;
constexpr int EPD_BUSY = 4;
constexpr int EPD_PWR  = 5;
11. Current mock animal

The current mock data is:

Name: Luna
Species: King Penguin
Location: Southern Ocean

Latitude:  -54.8123
Longitude: 158.4231

Distance travelled: 1284.7 km

This is only test data.

Eventually it will be replaced with real tracking data.

12. Real tracking data

The preferred data source is Movebank.

Movebank provides wildlife tracking datasets and supports projects such as Animal Tracker.

Important official resources:

Movebank Access Data
Movebank Manual
Movebank Animal Tracker
Movebank Data Repository

The exact animal/study still needs to be selected based on:

Public availability
Data access permissions
Update frequency
Interesting geography
Species suitability
Whether location data can legally/technically be accessed by the project

The device should not claim an animal is "live" if the underlying tracking device only reports periodically.

Better wording:

Latest known location

and:

Last signal: 42 min ago

13. Recommended final architecture

Rather than making the ESP32 perform complicated GIS/map rendering, the preferred architecture is:

             Wildlife tracking data
                      │
                      ▼
                Movebank/API
                      │
                      ▼
              Lightweight backend
                      │
          ┌───────────┴───────────┐
          │                       │
          ▼                       ▼
     Animal data              Map rendering
          │                       │
          └───────────┬───────────┘
                      ▼
                648 × 480 bitmap
                      │
                      ▼
                  Wi-Fi
                      │
                      ▼
                  ESP32-S3
                      │
                     SPI
                      │
                      ▼
             5.83" E-paper G

The backend can generate the complicated map.

The ESP32 simply downloads the resulting image/data and displays it.

This keeps the ESP32 code relatively lightweight.

14. Display design

The display should not look like Google Maps.

Desired aesthetic:

Naturalist / vintage wildlife field-guide map

Approximately:

┌────────────────────────────────────────────────────┐
│ MANDY'S WILDLIFE              KING PENGUIN          │
│                                                    │
│ ┌────────────────────────────────────────────────┐ │
│ │                                                │ │
│ │                 MAP                            │ │
│ │                                                │ │
│ │       ~~~~~~~ route ~~~~~~~                    │ │
│ │                         ●                     │ │
│ │                                                │ │
│ │             SOUTHERN OCEAN                    │ │
│ │                                                │ │
│ └────────────────────────────────────────────────┘ │
│                                                    │
│ LUNA                    KING PENGUIN               │
│ Latest known location                              │
│ Southern Ocean                                     │
│                                                    │
│ Last signal: 34 min ago     1,284 km travelled     │
└────────────────────────────────────────────────────┘

The map should ideally include:

Coastlines
Islands
Ocean
Ice where appropriate
Route history
Current marker
Animal illustration/icon
Location labels
Compass
Scale
Coordinates
Last update time
15. Four-colour display limitations

The Waveshare G display supports:

White
Black
Yellow
Red

The design should intentionally work within those four colours.

Suggested visual language:

Black
Text
Coastlines
Borders
Route
Icons
Yellow
Ocean/ice accents
Highlight areas
Decorative elements
Red
Current animal location
Important marker
Small accents
White
Background
Negative space

The map renderer will need to use dithering carefully where photographs or complex imagery are involved.

16. Current immediate development priority

Do not jump straight into Movebank.

The current milestone is:

Get the actual physical e-paper working.

Order:

Step 1

Confirm ESP32 boots.

Step 2

Confirm the GPIO definitions:

DIN  = 11
CLK  = 12
CS   = 10
DC   = 13
RST  = 9
BUSY = 4
PWR  = 5
Step 3

Initialize the Waveshare 5.83" G driver.

Step 4

Confirm:

SPI communication
PWR
RST
BUSY
DC
CS
Step 5

Run a simple test pattern.

Ideally test:

WHITE
BLACK
RED
YELLOW
Step 6

Confirm the full:

648 × 480

resolution.

Step 7

Only after the physical display works, integrate it into:

DisplayManager
17. Important implementation rule

Do not blindly install a random e-paper library and hope its defaults match the hardware.

The display is specifically:

Waveshare 5.83" E-Paper HAT (G)

and it should be implemented using the appropriate Waveshare G driver/example.

The software must be adapted to the user's actual GPIO wiring.

The physical wiring takes precedence over example GPIO values.

18. Existing DisplayManager

The current DisplayManager is still effectively a serial/mock implementation.

It currently prints things such as:

================================
       MANDY'S WILDLIFE
================================

Animal: Luna
Species: King Penguin
Location: Southern Ocean
Latitude: -54.812300
Longitude: 158.423100
Journey: 1284.7 km
================================

The next major task is replacing this with actual e-paper rendering.

19. Wi-Fi

Currently Wi-Fi uses placeholder credentials.

Current prototype:

const char* WIFI_SSID = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";

These must never be committed with real credentials.

.gitignore already contains:

secrets.h
credentials.h

Eventually the project should have proper Wi-Fi provisioning.

Desired final experience:

Mandy powers on device.
If no Wi-Fi is configured, the ESP32 creates a setup network.
Mandy connects with her phone.
A setup page appears.
She selects her Wi-Fi.
Enters password.
Device saves credentials.
Device connects automatically in future.
She can move the tracker to another Wi-Fi network without needing a PC.
20. Future features

After the physical display works:

Phase 1 — Display
Real e-paper initialization
Four-colour test
Static Mandy UI
Map rendering
Phase 2 — Wildlife
Real Movebank data
Animal selection
Location parsing
Timestamp handling
Route history
Phase 3 — Map
Coastlines
Geography
Animal marker
Route
Labels
Compass
Scale
Dithering
Phase 4 — Wi-Fi
Captive portal
Wi-Fi configuration
Saved credentials
Reconnection
Phase 5 — Device management
OTA firmware updates
Configuration page
Device status
Error handling
Phase 6 — Final hardware
Enclosure
Display mounting
Cable management
Power
Desk presentation
21. PC simulator

A PC simulator is also desired.

The simulator should reproduce the actual:

648 × 480

display.

It should emulate the four available colours:

Black
White
Red
Yellow

Potential structure:

simulator/
├── index.html
├── app.js
└── style.css

The simulator can eventually show the same UI that the physical display receives.

This will make designing the map/UI much faster than repeatedly refreshing the e-paper.

Wokwi can optionally be used for ESP32/electronics simulation, but the browser-based 648×480 visual simulator is more important for UI development.

22. Git workflow

The project should remain properly version controlled.

Typical workflow:

git add .
git commit -m "Add e-paper display driver"
git push

Potential future branches:

feature/esp32-setup
feature/wifi
feature/display
feature/tracking-api
feature/map-renderer
feature/mandy-ui
23. Current status
Completed
ESP32-S3 selected
Arduino IDE configured
ESP32S3 Dev Module selected
ESP32 successfully programmed
Display physically received
Display identified as Waveshare 5.83" G
Hardware wired
Wi-Fi prototype works
Mock tracking works
Project structure established
GitHub repository established
Current hardware wiring
5.83" G Driver HAT       ESP32-S3 Mini
────────────────────────────────────────
VCC                       3V3
GND                       GND
DIN                       GPIO11
CLK / SCLK                GPIO12
CS                        GPIO10
DC                        GPIO13
RST                       GPIO9
BUSY                      GPIO4
PWR                       GPIO5
Current immediate task

Get the physical 5.83" G e-paper displaying a four-colour test pattern using exactly those pins.

After that:

Build the real Mandy wildlife display UI.
