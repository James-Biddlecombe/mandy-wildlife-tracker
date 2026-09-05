#include "DisplayManager.h"

bool DisplayManager::begin()
{
    Serial.println("[DISPLAY] Initialising display...");

    // Real e-paper initialisation will go here.
    // We will add the Waveshare driver once the hardware
    // arrives and we confirm the exact driver revision.

    return true;
}

void DisplayManager::clear()
{
    Serial.println("[DISPLAY] CLEAR");
}

void DisplayManager::showBootScreen()
{
    Serial.println();
    Serial.println("================================");
    Serial.println("       MANDY'S WILDLIFE");
    Serial.println("================================");
    Serial.println();
}

void DisplayManager::showWiFiConnecting()
{
    Serial.println("[DISPLAY] Connecting to Wi-Fi...");
}

void DisplayManager::showWiFiConnected(const String& ip)
{
    Serial.print("[DISPLAY] Wi-Fi connected: ");
    Serial.println(ip);
}

void DisplayManager::showAnimal(const Animal& animal)
{
    Serial.println();
    Serial.println("================================");
    Serial.println("       MANDY'S WILDLIFE");
    Serial.println("================================");

    Serial.print("Animal: ");
    Serial.println(animal.name);

    Serial.print("Species: ");
    Serial.println(animal.species);

    Serial.print("Location: ");
    Serial.println(animal.location.locationName);

    Serial.print("Latitude: ");
    Serial.println(animal.location.latitude, 6);

    Serial.print("Longitude: ");
    Serial.println(animal.location.longitude, 6);

    Serial.print("Journey: ");
    Serial.print(animal.distanceTravelledKm);
    Serial.println(" km");

    Serial.println("================================");
    Serial.println();
}

void DisplayManager::showError(const String& message)
{
    Serial.print("[DISPLAY ERROR] ");
    Serial.println(message);
}

void DisplayManager::sleep()
{
    Serial.println("[DISPLAY] Entering sleep mode");
}
