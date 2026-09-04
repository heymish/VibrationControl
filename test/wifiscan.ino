#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  delay(1000);

  Serial.println("Scanning...");

  int n = WiFi.scanNetworks();

  Serial.printf("Found %d networks\n", n);

  for (int i = 0; i < n; i++) {
    Serial.printf("%s RSSI=%d CH=%d\n",
                  WiFi.SSID(i).c_str(),
                  WiFi.RSSI(i),
                  WiFi.channel(i));
  }
}

void loop() {
}
