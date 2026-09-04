#include "AppConfig.h"
#include "MotorControl.h"
#include "MotorNetworkManager.h"
#include "WebApp.h"

MotorControl motor;
MotorNetworkManager network;
WebApp webApp(motor, network);

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("ESP32-C3 motor controller starting");

  motor.begin();       // Always starts at 0%.
  network.begin();     // Saved Wi-Fi first, setup AP on failure.
  webApp.begin();      // Web UI and REST API.

  Serial.println("Controller ready");
}

void loop() {
  motor.update();
  webApp.loop();
  network.loop();

  delay(2);
}
