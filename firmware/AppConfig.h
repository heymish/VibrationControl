#pragma once
#include <Arduino.h>

namespace AppConfig {
  // ESP32-C3 SuperMini output pin.
  constexpr uint8_t MOTOR_PWM_PIN = 1;
  constexpr uint32_t PWM_FREQUENCY_HZ = 5000;
  constexpr uint8_t PWM_RESOLUTION_BITS = 8;
  constexpr uint16_t PWM_MAX_DUTY = 255;
  constexpr uint8_t MAX_SPEED_PERCENT = 100;

  // Set true only when the external analogue output stage is inverted.
  constexpr bool OUTPUT_INVERTED = false;

  // Provisioning access point.
  constexpr char AP_SSID[] = "MotorController-Setup";
  constexpr char AP_PASSWORD[] = "motor1234"; // At least 8 characters.

  // Available as motor.local on networks/devices that support mDNS.
  constexpr char MDNS_HOSTNAME[] = "HVT_Vibration";

  constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
  constexpr uint32_t WIFI_RECONNECT_INTERVAL_MS = 30000;
  constexpr uint32_t SPEED_UPDATE_DELAY_MS = 120;
}
