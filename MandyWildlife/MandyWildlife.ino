#include "App.h"

App app;

void setup()
{
    Serial.begin(115200);

    delay(1000);

    app.begin();
}

void loop()
{
    app.loop();
}
