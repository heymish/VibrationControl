#pragma once
#include <Arduino.h>

namespace WebPages {
  String controlPage(uint8_t currentSpeed, uint32_t updateDelayMs);
  String setupPage();
  String savedPage(const String& escapedSSID, const String& apSSID);
  String erasedPage();
}
