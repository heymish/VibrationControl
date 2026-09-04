#include "MotorNetworkManager.h"
#include "AppConfig.h"
#include <WiFi.h>
#include <ESPmDNS.h>

String MotorNetworkManager::savedSSID() {
  preferences_.begin("wifi", true);
  String value = preferences_.getString("ssid", "");
  preferences_.end();
  return value;
}

String MotorNetworkManager::savedPassword() {
  preferences_.begin("wifi", true);
  String value = preferences_.getString("password", "");
  preferences_.end();
  return value;
}

void MotorNetworkManager::saveCredentials(const String& ssidValue,
                                     const String& password) {
  preferences_.begin("wifi", false);
  preferences_.putString("ssid", ssidValue);
  preferences_.putString("password", password);
  preferences_.end();
}

void MotorNetworkManager::eraseCredentials() {
  preferences_.begin("wifi", false);
  preferences_.clear();
  preferences_.end();
}

void MotoretworkManager::startMDNS() {
  if (mdnsActive_ || WiFi.status() != WL_CONNECTED) return;

  if (MDNS.begin(AppConfig::MDNS_HOSTNAME)) {
    MDNS.addService("http", "tcp", 80);
    mdnsActive_ = true;
    Serial.printf("mDNS: http://%s.local/\n", AppConfig::MDNS_HOSTNAME);
  }
}

bool MotorNetworkManager::connectSaved() {
  const String name = savedSSID();
  const String password = savedPassword();

  if (name.isEmpty()) {
    Serial.println("No saved Wi-Fi credentials");
    return false;
  }

  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  WiFi.begin(name.c_str(), password.c_str());

  Serial.printf("Connecting to %s", name.c_str());
  const uint32_t started = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - started < AppConfig::WIFI_CONNECT_TIMEOUT_MS) {
    delay(250);
    Serial.print('.');
  }
  Serial.println();

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Saved Wi-Fi connection failed");
    WiFi.disconnect();
    return false;
  }

  apActive_ = false;
  Serial.print("Station IP: ");
  Serial.println(WiFi.localIP());
  startMDNS();
  return true;
}

void MotorNetworkManager::startAccessPoint() {
  WiFi.mode(WIFI_AP_STA); // Keep STA enabled so the setup page can scan.

  if (!WiFi.softAP(AppConfig::AP_SSID, AppConfig::AP_PASSWORD)) {
    Serial.println("ERROR: Setup AP could not be started");
    return;
  }

  apActive_ = true;
  Serial.printf("Setup AP: %s\n", AppConfig::AP_SSID);
  Serial.print("Setup page: http://");
  Serial.println(WiFi.softAPIP());
}

void MotorNetworkManager::begin() {
  if (!connectSaved()) startAccessPoint();
}

void MotorNetworkManager::loop() {
  if (apActive_) return;

  if (WiFi.status() == WL_CONNECTED) {
    startMDNS();
    return;
  }

  if (millis() - lastReconnectAttempt_ <
      AppConfig::WIFI_RECONNECT_INTERVAL_MS) return;

  lastReconnectAttempt_ = millis();
  const String name = savedSSID();

  if (name.isEmpty()) {
    startAccessPoint();
    return;
  }

  Serial.println("Attempting background Wi-Fi reconnection");
  WiFi.disconnect();
  WiFi.begin(name.c_str(), savedPassword().c_str());
}

String MotorNetworkManager::modeName() const {
  if (apActive_ && WiFi.status() == WL_CONNECTED) return "access-point-and-station";
  if (apActive_) return "access-point";
  if (WiFi.status() == WL_CONNECTED) return "station";
  return "disconnected";
}

String MotorNetworkManager::ipAddress() const {
  if (apActive_) return WiFi.softAPIP().toString();
  if (WiFi.status() == WL_CONNECTED) return WiFi.localIP().toString();
  return "0.0.0.0";
}

String MotorNetworkManager::ssid() const {
  if (WiFi.status() == WL_CONNECTED) return WiFi.SSID();
  if (apActive_) return AppConfig::AP_SSID;
  return "";
}

int32_t MotorNetworkManager::rssi() const {
  return WiFi.RSSI();
}

bool MotorNetworkManager::hasRssi() const {
  return WiFi.status() == WL_CONNECTED;
}

bool MotorNetworkManager::accessPointActive() const {
  return apActive_;
}
