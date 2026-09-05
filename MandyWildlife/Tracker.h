#pragma once

#include <Arduino.h>
#include "models.h"

class Tracker {
public:

    bool begin();

    bool update(Animal& animal);

private:

    bool getMockAnimal(Animal& animal);

    bool getMovebankAnimal(Animal& animal);
};
