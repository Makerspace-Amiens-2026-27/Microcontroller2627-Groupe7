#include <Arduino.h>
#include "config.h"

void setup() {
    Serial.begin(115200);
    pinMode(PIN_BUTTON, INPUT);
}

void loop() {
    Serial.println(digitalRead(PIN_BUTTON) == HIGH ? "HIGH" : "LOW");
    delay(200);  // programme de test : on ralentit l'affichage
}