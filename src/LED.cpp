#include "AQMonitor.h"
#include <math.h>

void LED::begin() {
    pinMode(RED, OUTPUT);
    pinMode(GREEN, OUTPUT);
    pinMode(BLUE, OUTPUT);
    analogWrite(RED, 1023);
    analogWrite(GREEN, 1023);
    analogWrite(BLUE, 1023);
}

void LED::loop() {
    if (settings.get()->blinkInterval > 0 &&
        millis() - _lastBlink > 1000 * settings.get()->blinkInterval) {
        blink(aqSensors.getIAQ());
    }

    set(_targetRed, _targetGreen, _targetBlue);

    if (millis() - _lastUpdate > 1000 * 0.2) {
        // Just for 0.2 seconds. After that - turn off the LEDs.
        _targetRed = 0;
        _targetGreen = 0;
        _targetBlue = 0;
    }
}

void LED::set(uint8_t red, uint8_t green, uint8_t blue) {
    analogWrite(RED, map(red, 0, 255, 1023, 0));
    analogWrite(GREEN, map(green, 0, 255, 1023, 0));
    analogWrite(BLUE, map(blue, 0, 255, 1023, 0));
}

void LED::blink(float iaq) {
        uint8_t red[] =   {0,   0,   255, 255, 255, 204, 153,  0};
        uint8_t green[] = {228, 228, 255, 126,   6,   3,   0,  0};
        uint8_t blue[] =  {0,   0,     0,   0,   6,  41,  76,  0};

        iaq *= 0.02f;

        int range_start_index = floor(iaq);
        int range_end_index = ceil(iaq);

        range_start_index = min(range_start_index, (int) sizeof(red)-1);
        range_end_index = min(range_end_index, (int) sizeof(red)-1);

        float fraction = iaq - floor(iaq);

        _targetRed = red[range_start_index] + fraction * (red[range_end_index] - red[range_start_index]);
        _targetGreen = green[range_start_index] + fraction * (green[range_end_index] - green[range_start_index]);
        _targetBlue = blue[range_start_index] + fraction * (blue[range_end_index] - blue[range_start_index]);
        _lastUpdate = millis();
        _lastBlink = millis();

        _targetGreen *= 0.1f;

        logger.log("Blinking to %d, %d, %d", _targetRed, _targetGreen, _targetBlue);
}

LED led = LED();