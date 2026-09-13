#pragma once
#include <Arduino.h>

namespace AppConfig {
  // ========== DAC Output Configuration ==========
  // Using MCP4725 I2C DAC for analog output (0-5V)
  // Controlled via Adafruit_MCP4725 library
  
  // MCP4725 I2C address (depends on A0 pin: GND=0x60, VCC=0x61)
  constexpr uint8_t MCP4725_ADDRESS = 0x60;
  
  // MCP4725 is 12-bit DAC with max value of 4095
  constexpr uint16_t DAC_MAX_VALUE = 4095;
  constexpr uint8_t MAX_SPEED_PERCENT = 100;

  // Set true only when the external analogue output stage is inverted.
  constexpr bool OUTPUT_INVERTED = false;

  // ========== Provisioning access point ==========
  constexpr char AP_SSID[] = "MotorController-Setup";
  constexpr char AP_PASSWORD[] = "motor1234"; // At least 8 characters.

  // Available as motor.local on networks/devices that support mDNS.
  constexpr char MDNS_HOSTNAME[] = "HVT_Vibration";

  constexpr uint32_t WIFI_CONNECT_TIMEOUT_MS = 15000;
  constexpr uint32_t WIFI_RECONNECT_INTERVAL_MS = 30000;
  constexpr uint32_t SPEED_UPDATE_DELAY_MS = 120;

  constexpr char FIRMWARE_VERSION[] = "0.1.0";
  constexpr char BUILD_DATE[] = __DATE__;
  constexpr char BUILD_TIME[] = __TIME__;
}
