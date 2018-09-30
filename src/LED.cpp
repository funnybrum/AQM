#include "AQMonitor.h"

void LED::begin() {
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    analogWrite(RED, 1023);
    analogWrite(GREEN, 1023);
    analogWrite(BLUE, 1023);
}

void LED::loop() {
}

void LED::set(uint8_t red, uint8_t green, uint8_t blue) {
    analogWrite(RED, map(red, 0, 255, 1023, 0));
    analogWrite(GREEN, map(green, 0, 255, 1023, 0));
    analogWrite(BLUE, map(blue, 0, 255, 1023, 0));
}

LED led = LED();