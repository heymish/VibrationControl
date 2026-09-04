#include "WebApp.h"
#include "AppConfig.h"
#include "WebPages.h"
#include <WiFi.h>

WebApp::WebApp(MotorControl& motor, MotorNetworkManager& network)
  : motor_(motor), network_(network) {}

String WebApp::htmlEscape(const String& input) {
  String output;
  for (size_t i = 0; i < input.length(); ++i) {
    switch (input[i]) {
      case '&': output += "&amp;"; break;
      case '<': output += "&lt;"; break;
      case '>': output += "&gt;"; break;
      case '"': output += "&quot;"; break;
      case '\'': output += "&#39;"; break;
      default: output += input[i];
    }
  }
  return output;
}

String WebApp::jsonEscape(const String& input) {
  String output;
  for (size_t i = 0; i < input.length(); ++i) {
    const char c = input[i];
    if (c == '"' || c == '\\') output += '\\';
    if (c == '\n') output += "\\n";
    else if (c == '\r') output += "\\r";
    else if (c == '\t') output += "\\t";
    else output += c;
  }
  return output;
}

bool WebApp::isUnsignedInteger(const String& input) {
  if (input.isEmpty()) return false;
  for (size_t i = 0; i < input.length(); ++i) {
    if (!isDigit(input[i])) return false;
  }
  return true;
}

String WebApp::statusJson() const {
  String json = "{\"firmwareVersion\":\"";
  json += AppConfig::FIRMWARE_VERSION;
  json += "\"";
  json += ",\"buildDate\":\"" + AppConfig::BUILD_DATE + "\"";
  json += ",\"buildTime\":\"" + AppConfig::BUILD_TIME + "\"";
  json += ",\"speed\":" + String(motor_.speed());
  json += ",\"pwmDuty\":" + String(motor_.duty());
  json += ",\"mode\":\"" + jsonEscape(network_.modeName()) + "\"";
  json += ",\"ip\":\"" + jsonEscape(network_.ipAddress()) + "\"";
  json += ",\"ssid\":\"" + jsonEscape(network_.ssid()) + "\"";
  json += ",\"rssi\":";
  json += network_.hasRssi() ? String(network_.rssi()) : "null";
  json += "}";
  return json;
}

void WebApp::handleHome() {
  server_.sendHeader("Cache-Control", "no-store");
  server_.send(200, "text/html", WebPages::controlPage(
    motor_.speed(), AppConfig::SPEED_UPDATE_DELAY_MS));
}

void WebApp::handleSetup() {
  server_.sendHeader("Cache-Control", "no-store");
  server_.send(200, "text/html", WebPages::setupPage());
}

void WebApp::handleSaveWiFi() {
  if (!server_.hasArg("ssid")) {
    server_.send(400, "application/json", "{\"error\":\"Missing SSID\"}");
    return;
  }

  String name = server_.arg("ssid");
  const String password = server_.arg("password");
  name.trim();

  if (name.isEmpty() || name.length() > 32) {
    server_.send(400, "application/json", "{\"error\":\"SSID must contain 1 to 32 characters\"}");
    return;
  }

  if (password.length() > 63) {
    server_.send(400, "application/json", "{\"error\":\"Password is too long\"}");
    return;
  }

  motor_.stop();
  network_.saveCredentials(name, password);
  server_.send(200, "text/html", WebPages::savedPage(
    htmlEscape(name), AppConfig::AP_SSID));
  delay(1500);
  ESP.restart();
}

void WebApp::handleClearWiFi() {
  motor_.stop();
  network_.eraseCredentials();
  server_.send(200, "text/html", WebPages::erasedPage());
  delay(1500);
  ESP.restart();
}

void WebApp::handleStatus() {
  server_.sendHeader("Cache-Control", "no-store");
  server_.send(200, "application/json", statusJson());
}

void WebApp::handleSpeed() {
  if (!server_.hasArg("value") || !isUnsignedInteger(server_.arg("value"))) {
    server_.send(400, "application/json", "{\"error\":\"Speed must be an integer from 0 to 100\"}");
    return;
  }

  const int value = server_.arg("value").toInt();
  if (value < 0 || value > AppConfig::MAX_SPEED_PERCENT) {
    server_.send(400, "application/json", "{\"error\":\"Speed must be from 0 to 100\"}");
    return;
  }

  motor_.setSpeed(value);
  server_.sendHeader("Cache-Control", "no-store");
  server_.send(200, "application/json",
    "{\"speed\":" + String(motor_.speed()) +
    ",\"pwmDuty\":" + String(motor_.duty()) + "}");
}

void WebApp::handleStop() {
  motor_.stop();
  server_.sendHeader("Cache-Control", "no-store");
  server_.send(200, "application/json", "{\"speed\":0,\"pwmDuty\":0,\"stopped\":true}");
}

void WebApp::handleNetworks() {
  const int count = WiFi.scanNetworks();
  String json = "{\"networks\":[";

  for (int i = 0; i < count; ++i) {
    if (i) json += ',';
    json += "{\"ssid\":\"" + jsonEscape(WiFi.SSID(i)) + "\"";
    json += ",\"rssi\":" + String(WiFi.RSSI(i));
    json += ",\"secure\":";
    json += WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "false" : "true";
    json += '}';
  }

  json += "]}";
  WiFi.scanDelete();
  server_.sendHeader("Cache-Control", "no-store");
  server_.send(200, "application/json", json);
}

void WebApp::handleNotFound() {
  server_.send(404, "application/json",
    "{\"error\":\"Not found\",\"path\":\"" +
    jsonEscape(server_.uri()) + "\"}");
}

void WebApp::configureRoutes() {
  server_.on("/", HTTP_GET, [this]() { handleHome(); });
  server_.on("/setup", HTTP_GET, [this]() { handleSetup(); });
  server_.on("/save-wifi", HTTP_POST, [this]() { handleSaveWiFi(); });
  server_.on("/clear-wifi", HTTP_POST, [this]() { handleClearWiFi(); });
  server_.on("/api/status", HTTP_GET, [this]() { handleStatus(); });
  server_.on("/api/speed", HTTP_GET, [this]() { handleSpeed(); });
  server_.on("/api/stop", HTTP_POST, [this]() { handleStop(); });
  server_.on("/api/networks", HTTP_GET, [this]() { handleNetworks(); });
  server_.onNotFound([this]() { handleNotFound(); });
}

void WebApp::begin() {
  configureRoutes();
  server_.begin();
  Serial.println("Web server started");
}

void WebApp::loop() {
  server_.handleClient();
}
