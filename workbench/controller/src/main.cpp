#include <Arduino.h>
#include <Servo.h>
#include "MSlave.h"
#include "motor_controller.hpp"
#include "generator_controller.hpp"

// GPIO for faulty bearing.
#define BEARING_LED 13
#define BEARING_POT A5

// GPIO for tested motor.
#define MOTOR_IN1 7
#define MOTOR_IN2 8
#define MOTOR_PWM 5
#define MOTOR_ENABLE A0

// GPIO for artificial load.
#define LOAD_IN1 4
#define LOAD_IN2 9
#define LOAD_PWM 6
#define LOAD_ENABLE A1

// GPIO for heater.
#define NOISE_SERVO A4

// Coil addresses.
#define IMPACT_MOTOR_ENABLE_FIXED_ADDR 0
#define IMPACT_MOTOR_ENABLE_RANDOM_ADDR 1
#define IMPACT_LOAD_ENABLE_FIXED_ADDR 2
#define IMPACT_LOAD_ENABLE_RANDOM_ADDR 3
#define BEARING_ENABLE_ADDR 4
#define NOISE_ENABLE_ADDR 5

// Holding register addresses.
#define MOTOR_SPEED_ADDR 0
#define LOAD_SPEED_ADDR 1

#define MOTOR_SHOCK_PERIOD_ADDR 2
#define MOTOR_SHOCK_FILLING_ADDR 3
#define LOAD_SHOCK_PERIOD_ADDR 4
#define LOAD_SHOCK_FILLING_ADDR 5

#define BEARING_PULSE_DURATION_ADDR 6
#define BEARING_PULSE_THRESHOLD_ADDR 7
#define BEARING_PULSES_PER_REV_ADDR 8

MSlave<6, 0, 9, 0> modbus;
GeneratorController motor(MOTOR_IN1, MOTOR_IN2, MOTOR_PWM, MOTOR_ENABLE, BEARING_LED, BEARING_POT);
MotorController load(LOAD_IN1, LOAD_IN2, LOAD_PWM, LOAD_ENABLE);
Servo noise;

void setup()
{
  motor.begin();
  load.begin();
  noise.attach(NOISE_SERVO);

  randomSeed(analogRead(0));

  Serial.begin(115200);
  Serial.setTimeout(10);
  modbus.begin(1, Serial);
}

void loop()
{
  if (modbus.available())
  {
    int result = modbus.read();

    // Some communication happened.
    if (result)
    {
      // Motor and impact control.
      motor.setPWM((int16_t)modbus.readHoldingRegister(MOTOR_SPEED_ADDR));
      motor.enableFixedShock(modbus.readCoil(IMPACT_MOTOR_ENABLE_FIXED_ADDR));
      motor.enableRandomShock(modbus.readCoil(IMPACT_MOTOR_ENABLE_RANDOM_ADDR));
      motor.setFixedShockPeriod(modbus.readHoldingRegister(MOTOR_SHOCK_PERIOD_ADDR));
      motor.setFixedShockFilling(modbus.readHoldingRegister(MOTOR_SHOCK_FILLING_ADDR));

      load.setPWM((int16_t)modbus.readHoldingRegister(LOAD_SPEED_ADDR));
      load.enableFixedShock(modbus.readCoil(IMPACT_LOAD_ENABLE_FIXED_ADDR));
      load.enableRandomShock(modbus.readCoil(IMPACT_LOAD_ENABLE_RANDOM_ADDR));
      load.setFixedShockPeriod(modbus.readHoldingRegister(LOAD_SHOCK_PERIOD_ADDR));
      load.setFixedShockFilling(modbus.readHoldingRegister(LOAD_SHOCK_FILLING_ADDR));

      // Bearing control.
      motor.bearingEnable(modbus.readCoil(BEARING_ENABLE_ADDR));
      motor.bearingSetPulseDuration(modbus.readHoldingRegister(BEARING_PULSE_DURATION_ADDR));
      motor.bearingSetPulseThreshold(modbus.readHoldingRegister(BEARING_PULSE_THRESHOLD_ADDR));
      motor.bearingSetPulsesPerRev(modbus.readHoldingRegister(BEARING_PULSES_PER_REV_ADDR));
    }
  }

  // Noise control.
  if (modbus.readCoil(NOISE_ENABLE_ADDR))
    noise.write(random(0, 181));
    
  motor.update();
  load.update();
  delay(10);
}