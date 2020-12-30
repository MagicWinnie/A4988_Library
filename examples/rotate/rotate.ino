#include "A4988.h"

Stepper stepper(200, 3, 2, 4, 5, 6);

void setup()
{
    stepper.setWorkMode(SPEED);
    stepper.setSpeed(1000);
}

void loop()
{
    stepper.tick();
}