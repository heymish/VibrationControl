#pragma once
#include <Arduino.h>
#include <WebServer.h>
#include "MotorControl.h"
#include "MotorNetworkManager.h"

class WebApp {
public:
  WebApp(MotorControl& motor, MotorNetworkManager& network);
  void begin();
  void loop();

private:
  WebServer server_{80};
  MotorControl& motor_;
  MotorNetworkManager& network_;

  static String htmlEscape(const String& input);
  static String jsonEscape(const String& input);
  static bool isUnsignedInteger(const String& input);

  String statusJson() const;
  void configureRoutes();
  void handleHome();
  void handleSetup();
  void handleSaveWiFi();
  void handleClearWiFi();
  void handleStatus();
  void handleSpeed();
  void handleStop();
  void handleNetworks();
  void handleNotFound();
};
