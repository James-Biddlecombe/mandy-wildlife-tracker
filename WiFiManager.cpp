#include "WiFiManager.h"
#include <WiFi.h>

namespace
{
    const char* WIFI_SSID = "YOUR_WIFI";
    const char* WIFI_PASSWORD = "YOUR_PASSWORD";
}

bool MandyWiFiManager::begin()
{
    Serial.println("[WIFI] Starting Wi-Fi...");

    WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    unsigned long start = millis();

    while (WiFi.status() != WL_CONNECTED &&
           millis() - start < 15000)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("[WIFI] Connected");
        Serial.print("[WIFI] IP: ");
        Serial.println(WiFi.localIP());

        return true;
    }

    Serial.println("[WIFI] Connection failed");

    return false;
}

bool MandyWiFiManager::connected()
{
    return WiFi.status() == WL_CONNECTED;
}

String MandyWiFiManager::ipAddress()
{
    if (!connected())
    {
        return "";
    }

    return WiFi.localIP().toString();
}

void MandyWiFiManager::maintain()
{
    if (!connected())
    {
        Serial.println("[WIFI] Connection lost");
    }
}
