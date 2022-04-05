#pragma once

#include <Arduino.h>

class MotorController
{
private:
    int in1Pin;
    int in2Pin;
    int pwmPin;
    int enaPin;

    int motorPWM = 0; //!< User desired motor's PWM (signed to specify direction).

    bool fixedShockEnabled = false; //!< Whether shocking with specified parameters is enabled.
    bool randomShockEnabled = false; //!< Whether shocking with randomized parameters is enabled.

    int randomShockPeriod = 500; //!< Period of randomized shock in ms, randomized after each shock.
    int randomShockFilling = 50; //!< Filling of randomized shock in %, randomized after each shock.

    int fixedShockPeriod = 500; //!< Period of fixed shock in ms, specified by user.
    int fixedShockFilling = 50; //!< Filling of fixed shock in %, specified by user.

    unsigned long startShockTime = 0; //!< Used to track shock time.

    void writePWM();

public:
    MotorController(int _in1Pin, int _in2Pin, int _pwmPin, int _enaPin)
        : in1Pin(_in1Pin), in2Pin(_in2Pin), pwmPin(_pwmPin), enaPin(_enaPin) {}

    void begin()
    {
        pinMode(in1Pin, OUTPUT);
        pinMode(in2Pin, OUTPUT);
        pinMode(pwmPin, OUTPUT);
        pinMode(enaPin, OUTPUT);

        digitalWrite(enaPin, 1);
    }

    void writePWM(int val)
    {
        if (val > 0)
        {
            digitalWrite(in1Pin, HIGH);
            digitalWrite(in2Pin, LOW);
        }
        else
        {
            digitalWrite(in1Pin, LOW);
            digitalWrite(in2Pin, HIGH);
        }

        analogWrite(pwmPin, abs(val));
    }

    void setPWM(int val)
    {
        motorPWM = val;
    }

    void update()
    {
        int onTime;
        unsigned long currTime = millis();
        if (fixedShockEnabled)
        {
            onTime = fixedShockPeriod * (float)fixedShockFilling / 100.0;
            if (currTime - startShockTime > fixedShockPeriod)
            {
                startShockTime = currTime;
                writePWM(motorPWM);
            }
            else if (currTime - startShockTime > onTime)
                writePWM(0);
        }
        else if (randomShockEnabled)
        {
            onTime = randomShockPeriod * (float)randomShockFilling / 100.0;
            if (currTime - startShockTime > randomShockPeriod)
            {
                startShockTime = currTime;
                randomShockFilling = random(10, 101);
                randomShockPeriod = random(100, 1001);
                writePWM(motorPWM);
            }
            else if (currTime - startShockTime > onTime)
            {
                writePWM(0);
            }
        }
        else
            writePWM(motorPWM);
    }

    void enableFixedShock(bool state)
    {
        fixedShockEnabled = state;
    }

    void enableRandomShock(bool state)
    {
        randomShockEnabled = state;
    }

    void setFixedShockPeriod(int period)
    {
        if (period > 1000)
            period = 1000;
        else if (period < 100)
            period = 100;

        fixedShockPeriod = period;
    }

    void setFixedShockFilling(int filling)
    {
        if (filling < 10)
            filling = 10;
        else if (filling > 100)
            filling = 100;

        fixedShockFilling = filling;
    }
};