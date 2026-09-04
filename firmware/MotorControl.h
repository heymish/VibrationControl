#pragma once

#include <Arduino.h>

class MotorControl {
public:
  void begin();

  // Sets the desired speed. The actual output ramps toward it.
  void setSpeed(uint8_t percent);

  // Ramps the motor down to zero.
  void stop();

  // Immediately sets the output to zero without ramping.
  void emergencyStop();

  // Must be called regularly from loop().
  void update();

  // Current applied speed.
  uint8_t speed() const;

  // Requested target speed.
  uint8_t targetSpeed() const;

  // Current applied PWM duty.
  uint16_t duty() const;

  // True while the applied speed is moving toward the target.
  bool isRamping() const;

private:
  static constexpr uint32_t RAMP_INTERVAL_MS = 20;
  static constexpr uint8_t RAMP_STEP_PERCENT = 2;

  uint8_t speedPercent_ = 0;
  uint8_t targetSpeedPercent_ = 0;

  uint32_t lastRampUpdateMs_ = 0;
  bool pwmAttached_ = false;

  uint16_t speedToDuty(uint8_t percent) const;
  void applySpeed(uint8_t percent);
};
