#include <Arduino.h>
#include "config.h"
constexpr int JOYSTICK_DEADZONE = 300;
int joystickCenterX = 2048;

void calibrateJoystick() {
    long sum = 0;
    for (int i = 0; i < 16; i++) {
        sum += analogRead(PIN_JOYSTICK_X);
        delay(2);
    }
    joystickCenterX = sum / 16;
}

// Convertit une lecture brute (0..4095) en valeur entre -1.0 et +1.0
float normalizeJoystick(int raw, int center) {
    int offset = raw - center;
    if (abs(offset) < JOYSTICK_DEADZONE) {
        return 0.0f;
    }
    int range = (offset > 0) ? (4095 - center) : center;
    return constrain((float)offset / range, -1.0f, 1.0f);
}
void setup() {
    pinMode(PIN_BUZZER, OUTPUT);

    // Test 1 : buzzer passif, une gamme do-ré-mi-fa-sol
    const int notes[] = {262, 294, 330, 349, 392};  // fréquences en Hz
    for (int note : notes) {
        tone(PIN_BUZZER, note, 200);
        delay(250);  // test de démarrage : attendre la fin de la note
    }
    delay(1000);

    // Test 2 : buzzer actif, un bip d'une demi-seconde
    digitalWrite(PIN_BUZZER, HIGH);
    delay(500);
    digitalWrite(PIN_BUZZER, LOW);
}

void loop() {
    int x = analogRead(PIN_JOYSTICK_X);
    int y = analogRead(PIN_JOYSTICK_Y);
    bool pressed = digitalRead(PIN_JOYSTICK_SELECT) == LOW;

    Serial.printf("&float x: %.3f, y: %.3f, pressed: %d\n", normalizeJoystick(x, joystickCenterX), normalizeJoystick(y, 2048), pressed);
    delay(100);
}

