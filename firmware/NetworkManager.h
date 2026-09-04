#pragma once
#include <Arduino.h>
#include <Preferences.h>

class NetworkManager {
public:
  void begin();
  void loop();

  String modeName() const;
  String ipAddress() const;
  String ssid() const;
  int32_t rssi() const;
  bool hasRssi() const;
  bool accessPointActive() const;

  void saveCredentials(const String& ssid, const String& password);
  void eraseCredentials();

private:
  Preferences preferences_;
  bool apActive_ = false;
  bool mdnsActive_ = false;
  uint32_t lastReconnectAttempt_ = 0;

  String savedSSID();
  String savedPassword();
  bool connectSaved();
  void startAccessPoint();
  void startMDNS();
};
