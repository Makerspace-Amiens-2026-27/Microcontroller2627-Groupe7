#include <Arduino.h>
#include "config.h"

void setup() {
    Serial.begin(115200);
    pinMode(PIN_JOYSTICK_SELECT, INPUT_PULLUP);
    analogReadResolution(12);  // lectures entre 0 et 4095
}

void loop() {
    int x = analogRead(PIN_JOYSTICK_X);
    int y = analogRead(PIN_JOYSTICK_Y);
    bool pressed = digitalRead(PIN_JOYSTICK_SELECT) == LOW;

    Serial.printf("x=%d y=%d select=%d\n", x, y, pressed);
    delay(100);
}

