#pragma once

#include <Arduino.h>

/**
 * @brief Logic to simulate faulty bearing.
 */
class FaultyBearingMock
{
private:
    int ledPin; //!< LED GPIO.
    int potPin; //!< Photoresistor's GPIO.

    uint16_t pulseDuration = 100;        //!<
    uint16_t triggerTreshold = 300;      //!< Difference between analog pin readings that will be treated as encoder's pulse.
    const uint8_t encoderResolution = 8; //!< Number of encoder impulses per one revolution.
    uint8_t pulsesPerRevolution = 1;     //!< Set number of bearing pulses per one revolution.

    bool enabled = false;

    bool running = false;
    unsigned long startTime = 0;

    uint8_t pulseCntr = 0;          //!< Bearing pulse tracker.
    int16_t previousPotReading = 0; //!< Previous reading of encoder's photoresistor.

public:
    FaultyBearingMock(int _ledPin, int _potPin)
        : ledPin(_ledPin), potPin(_potPin) {}

    void begin()
    {
        pinMode(potPin, INPUT);
        pinMode(ledPin, OUTPUT);
    }

    int update(int pwm)
    {
        int resPWM = pwm;

        int16_t potReading = analogRead(potPin);
        int16_t potDiff = potReading - previousPotReading;

        // Check for encoder's pulse.
        if (abs(potDiff) >= triggerTreshold && potDiff > 0)
        {
            if (!running && pulseCntr < pulsesPerRevolution)
            {
                running = true;
                startTime = millis();
            }

            pulseCntr++;
            if (pulseCntr >= encoderResolution)
                pulseCntr = 0;
        }

        if (running && (millis() - startTime) >= pulseDuration)
        {
            running = false;
        }
        else if (running)
        {
            resPWM = (int)(pwm * 0.2) % 255;
        }

        previousPotReading = potReading;

        return resPWM;
    }

    void enable(bool state)
    {
        enabled = state;
        digitalWrite(ledPin, state);
    }

    void setPulseDuration(uint16_t dur)
    {
        pulseDuration = dur;
    }

    void setPulseThreshold(uint16_t threshold)
    {
        triggerTreshold = threshold;
    }

    void setPulsesPerRev(uint8_t pulses)
    {
        pulsesPerRevolution = pulses;
    }
};