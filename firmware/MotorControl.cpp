#include "MotorControl.h"
#include "AppConfig.h"

void MotorControl::begin() {
  const bool attached = ledcAttach(
    AppConfig::MOTOR_PWM_PIN,
    AppConfig::PWM_FREQUENCY_HZ,
    AppConfig::PWM_RESOLUTION_BITS
  );

  if (!attached) {
    Serial.println("ERROR: PWM output could not be configured");
  }

  stop();
}

uint16_t MotorControl::speedToDuty(uint8_t percent) const {
  percent = constrain(percent, 0, AppConfig::MAX_SPEED_PERCENT);

  uint16_t value = map(
    percent,
    0,
    AppConfig::MAX_SPEED_PERCENT,
    0,
    AppConfig::PWM_MAX_DUTY
  );

  if (AppConfig::OUTPUT_INVERTED) {
    value = AppConfig::PWM_MAX_DUTY - value;
  }

  return value;
}

void MotorControl::setSpeed(uint8_t percent) {
  speedPercent_ = constrain(percent, 0, AppConfig::MAX_SPEED_PERCENT);
  ledcWrite(AppConfig::MOTOR_PWM_PIN, speedToDuty(speedPercent_));

  Serial.printf("Motor speed: %u%%, PWM duty: %u\n", speedPercent_, duty());
}

void MotorControl::stop() {
  setSpeed(0);
}

uint8_t MotorControl::speed() const {
  return speedPercent_;
}

uint16_t MotorControl::duty() const {
  return speedToDuty(speedPercent_);
}
