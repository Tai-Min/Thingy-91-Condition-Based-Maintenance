#pragma once

#include <Arduino.h>
#include <Servo.h>
#include "Generators/Analog/LinearGenerator/LinearGenerator.h"

using namespace SigUtil::Generators::Analog;

/**
 * @brief Logic to simulate faulty bearing.
 */
class FaultyBearingMock
{
private:
    int ledPin;   //!< LED GPIO.
    int potPin;   //!< Photoresistor's GPIO.
    int servoPin; //!< Servo's GPIO.

    int pulseDuration = 100;         //!<
    int triggerTreshold = 300;       //!< Difference between analog pin readings that will be treated as encoder's pulse.
    const int encoderResolution = 8; //!< Number of encoder impulses per one revolution.
    int pulsesPerRevolution = 1;     //!< Set number of bearing pulses per one revolution.

    bool enabled = false;

    Servo servo;                    //!< Servo that will vibrate to simulate faulty bearing.
    LinearGenerator servoGenerator; //!< Servo's time based angle generator.
    bool servoDir = false;          //!< Whether to move 0 - 180 or 180 - 0.
    int servoMinAngle = 0;
    int servoMaxAngle = 180;

    bool fstScan = false; //!< First update() call indicator.

    int pulseCntr = 0;          //!< Bearing pulse tracker.
    int previousPotReading = 0; //!< Previous reading of encoder's photoresistor.

public:
    FaultyBearingMock(int _ledPin, int _potPin, int _servoPin)
        : ledPin(_ledPin), potPin(_potPin), servoPin(_servoPin) {}

    void begin()
    {
        pinMode(potPin, INPUT);
        pinMode(ledPin, OUTPUT);
        servo.attach(servoPin);
    }

    void update()
    {
        int potReading = analogRead(potPin);
        int potDiff = potReading - previousPotReading;

        // Check for encoder's pulse.
        if (fstScan)
            fstScan = false;
        else if (!servoGenerator.running() && abs(potDiff) >= triggerTreshold && potDiff > 0)
        {
            if (pulseCntr < pulsesPerRevolution)
            {
                if (servoDir && enabled)
                    servoGenerator.start(servoMinAngle, servoMaxAngle, pulseDuration);
                else if (enabled)
                    servoGenerator.start(servoMaxAngle, servoMinAngle, pulseDuration);
            }

            servoDir = !servoDir;
            pulseCntr++;
            if (pulseCntr == encoderResolution)
                pulseCntr = 0;
        }

        servo.write(servoGenerator.read());

        previousPotReading = potReading;
    }

    void enable(bool state)
    {
        enabled = state;
        digitalWrite(ledPin, state);
    }

    void setPulseDuration(int dur)
    {
        pulseDuration = dur;
    }

    void setPulseThreshold(int threshold)
    {
        triggerTreshold = threshold;
    }

    void setPulsesPerRev(int pulses)
    {
        pulsesPerRevolution = pulses;
    }

    void setMinAngle(int angle)
    {
        servoMinAngle = angle;
    }

    void setMaxAngle(int angle)
    {
        servoMaxAngle = angle;
    }
};