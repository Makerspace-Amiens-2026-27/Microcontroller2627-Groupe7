#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "config.h"

Adafruit_ST7789 tft(&SPI, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RESET);

constexpr uint32_t FRAME_TIME_MS = 16;

float dotX = 120, dotY = 160;   // position calculée
int drawnX = 120, drawnY = 160; // position affichée

constexpr int JOYSTICK_DEADZONE = 300;
int joystickCenterX = 2048;

constexpr int PRESSED_LEVEL = LOW;           // niveau lu quand on appuie (étape 1)
constexpr unsigned long DEBOUNCE_MS = 30;

struct Button {
    int pin;
    int stableLevel;              // dernier niveau confirmé
    int lastRead;                 // dernière lecture brute
    unsigned long lastChangeMs;   // date du dernier mouvement du signal
};

Button button = {PIN_BUTTON, !PRESSED_LEVEL, !PRESSED_LEVEL, 0};

// Renvoie true une seule fois par appui, au moment où il est confirmé
bool justPressed(Button& b) {
    int level = digitalRead(b.pin);
    if (level != b.lastRead) {         // le signal bouge : rebond ou vrai changement
        b.lastRead = level;
        b.lastChangeMs = millis();
    }
    if (millis() - b.lastChangeMs > DEBOUNCE_MS && level != b.stableLevel) {
        b.stableLevel = level;         // stable depuis 30 ms : changement confirmé
        return level == PRESSED_LEVEL; // vrai sur l'appui, faux sur le relâchement
    }
    return false;
}

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

    Serial.begin(115200);
    pinMode(PIN_BUTTON, INPUT_PULLUP);

    pinMode(PIN_JOYSTICK_SELECT, INPUT_PULLUP);
    analogReadResolution(12);  // lectures entre 0 et 4095

    pinMode(PIN_BUZZER, OUTPUT);

    SPI.begin(PIN_TFT_SCK, -1, PIN_TFT_MOSI, PIN_TFT_CS);  // pas de MISO
    tft.init(240, 320, SPI_MODE3);
    tft.setRotation(2);                                     // portrait
    tft.fillScreen(ST77XX_BLACK);

    tft.setTextSize(4);
    tft.setTextColor(ST77XX_WHITE);
    tft.setCursor(72, 40);
    tft.print("PONG");

    tft.drawRect(20, 120, 200, 60, ST77XX_WHITE);   // rectangle
    tft.fillCircle(120, 250, 20, ST77XX_YELLOW);    // disque
}

void loop() {
    static uint32_t lastFrameMs = 0;
    uint32_t nowMs = millis();
    if (nowMs - lastFrameMs < FRAME_TIME_MS) {
        return;
    }
    lastFrameMs = nowMs;

    // 1. Lire
    float x = normalizeJoystick(analogRead(PIN_JOYSTICK_X), joystickCenterX);
    bool beep = justPressed(button);

    // 2. Calculer
    dotX = constrain(dotX + x * 4.0f, 0.0f, 235.0f);
    if (beep) {
        tone(PIN_BUZZER, 880, 50);
    }

    // 3. Afficher : effacer l'ancien point, dessiner le nouveau
    if ((int)dotX != drawnX) {
        tft.fillRect(drawnX, drawnY, 5, 5, ST77XX_BLACK);
        drawnX = (int)dotX;
        tft.fillRect(drawnX, drawnY, 5, 5, ST77XX_WHITE);
    }
}