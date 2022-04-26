#pragma once

#include <Arduino.h>
#include "motor_controller.hpp"
#include "bearing_fail_sim.hpp"

class GeneratorController : public MotorController
{
private:
    int motorPWM = 0;
    FaultyBearingMock bearing;

public:
    GeneratorController(int _in1Pin, int _in2Pin, int _pwmPin, int _enaPin, int _ledPin, int _potPin)
        : MotorController(_in1Pin, _in2Pin, _pwmPin, _enaPin), bearing(_ledPin, _potPin) {}

    void begin()
    {
        MotorController::begin();
        bearing.begin();
    }

    void setPWM(int val)
    {
        motorPWM = val;
    }

    void update()
    {
        MotorController::setPWM(bearing.update(motorPWM));
        MotorController::update();
    }

    void bearingEnable(bool state)
    {
        bearing.enable(state);
    }

    void bearingSetPulseDuration(uint16_t dur)
    {
        bearing.setPulseDuration(dur);
    }

    void bearingSetPulseThreshold(uint16_t threshold)
    {
        bearing.setPulseThreshold(threshold);
    }

    void bearingSetPulsesPerRev(uint8_t pulses)
    {
        bearing.setPulsesPerRev(pulses);
    }
};