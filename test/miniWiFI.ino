#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP);

  bool ok = WiFi.softAP(
      "TEST-ESP32",
      "password123",
      1,
      false
  );

  Serial.printf("AP=%s\n", ok ? "OK" : "FAIL");
  Serial.println(WiFi.softAPIP());
}

void loop() {}
