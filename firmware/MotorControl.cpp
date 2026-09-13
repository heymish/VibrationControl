#include "MotorControl.h"
#include "AppConfig.h"

void MotorControl::begin() {
  // Initialize MCP4725 DAC using Adafruit library
  if (!dac_.begin(AppConfig::MCP4725_ADDRESS)) {
    Serial.println("ERROR: MCP4725 DAC could not be initialized");
    return;
  }

  // Force a known safe output immediately during startup.
  speedPercent_ = 0;
  targetSpeedPercent_ = 0;
  lastRampUpdateMs_ = millis();

  applySpeed(0);

  dacInitialized_ = true;

  Serial.printf(
    "Motor DAC configured: Adafruit MCP4725 at address 0x%02X\n",
    AppConfig::MCP4725_ADDRESS
  );
}

uint16_t MotorControl::speedToDAC(uint8_t percent) const {
  percent = constrain(
    percent,
    0,
    AppConfig::MAX_SPEED_PERCENT
  );

  uint16_t value = map(
    percent,
    0,
    AppConfig::MAX_SPEED_PERCENT,
    0,
    AppConfig::DAC_MAX_VALUE
  );

  if (AppConfig::OUTPUT_INVERTED) {
    value = AppConfig::DAC_MAX_VALUE - value;
  }

  return value;
}

void MotorControl::applySpeed(uint8_t percent) {
  percent = constrain(
    percent,
    0,
    AppConfig::MAX_SPEED_PERCENT
  );

  speedPercent_ = percent;

  if (!dacInitialized_) {
    return;
  }

  uint16_t dacValue = speedToDAC(speedPercent_);
  dac_.setVoltage(dacValue, false);  // false = no EEPROM write
}

void MotorControl::setSpeed(uint8_t percent) {
  const uint8_t constrainedPercent = constrain(
    percent,
    0,
    AppConfig::MAX_SPEED_PERCENT
  );

  if (constrainedPercent == targetSpeedPercent_) {
    return;
  }

  targetSpeedPercent_ = constrainedPercent;

  Serial.printf(
    "Motor target speed: %u%%, current speed: %u%%\n",
    targetSpeedPercent_,
    speedPercent_
  );
}

void MotorControl::stop() {
  // Normal stop uses the configured ramp.
  setSpeed(0);
}

void MotorControl::emergencyStop() {
  targetSpeedPercent_ = 0;
  applySpeed(0);
  lastRampUpdateMs_ = millis();

  Serial.printf(
    "Motor emergency stop, DAC value: %u\n",
    dacValue()
  );
}

void MotorControl::update() {
  if (!dacInitialized_) {
    return;
  }

  if (speedPercent_ == targetSpeedPercent_) {
    return;
  }

  const uint32_t now = millis();
  const uint32_t elapsedMs = now - lastRampUpdateMs_;

  if (elapsedMs < RAMP_INTERVAL_MS) {
    return;
  }

  /*
   * Determine how many ramp steps are due.
   *
   * This means the ramp timing remains approximately correct even if another
   * part of the application occasionally delays loop() for longer than one
   * ramp interval.
   */
  const uint32_t elapsedSteps = elapsedMs / RAMP_INTERVAL_MS;
  lastRampUpdateMs_ += elapsedSteps * RAMP_INTERVAL_MS;

  const uint32_t change =
    elapsedSteps * static_cast<uint32_t>(RAMP_STEP_PERCENT);

  uint8_t nextSpeed = speedPercent_;

  if (speedPercent_ < targetSpeedPercent_) {
    const uint32_t increasedSpeed =
      static_cast<uint32_t>(speedPercent_) + change;

    nextSpeed = static_cast<uint8_t>(
      min(
        increasedSpeed,
        static_cast<uint32_t>(targetSpeedPercent_)
      )
    );
  } else {
    const int32_t decreasedSpeed =
      static_cast<int32_t>(speedPercent_) -
      static_cast<int32_t>(change);

    nextSpeed = static_cast<uint8_t>(
      max(
        decreasedSpeed,
        static_cast<int32_t>(targetSpeedPercent_)
      )
    );
  }

  applySpeed(nextSpeed);

  if (speedPercent_ == targetSpeedPercent_) {
    Serial.printf(
      "Motor reached target: %u%%, DAC value: %u\n",
      speedPercent_,
      dacValue()
    );
  }
}

uint8_t MotorControl::speed() const {
  return speedPercent_;
}

uint8_t MotorControl::targetSpeed() const {
  return targetSpeedPercent_;
}

uint16_t MotorControl::dacValue() const {
  return speedToDAC(speedPercent_);
}

bool MotorControl::isRamping() const {
  return speedPercent_ != targetSpeedPercent_;
}
