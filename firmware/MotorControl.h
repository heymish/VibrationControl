#pragma once
#include <Arduino.h>

class MotorControl {
public:
  void begin();
  void setSpeed(uint8_t percent);
  void stop();

  uint8_t speed() const;
  uint16_t duty() const;

private:
  uint8_t speedPercent_ = 0;
  uint16_t speedToDuty(uint8_t percent) const;
};
