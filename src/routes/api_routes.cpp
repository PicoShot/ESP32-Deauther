#include <WebServer.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "routes/api_routes.h"
#include "utils/wifi_utils.h"
#include "definitions.h"

extern WebServer server;
extern int num_networks;
extern int eliminated_stations;

void handle_api_networks() {
  JsonDocument doc;
  JsonArray networksArray = doc["networks"].to<JsonArray>();
  
  for (int i = 0; i < num_networks; i++) {
    JsonObject network = networksArray.add<JsonObject>();
    String encryption = getEncryptionType(WiFi.encryptionType(i));
    int rssi = WiFi.RSSI(i);
    int rssiPercentage = map(rssi, -100, -50, 0, 100);
    rssiPercentage = constrain(rssiPercentage, 0, 100);
    
    int signalBars = map(rssiPercentage, 0, 100, 0, 4);

    network["id"] = i;
    network["ssid"] = WiFi.SSID(i);
    network["bssid"] = WiFi.BSSIDstr(i);
    network["channel"] = WiFi.channel(i);
    network["rssi"] = rssi;
    network["encryption"] = encryption;
  }
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handle_api_stats() {
  JsonDocument doc;
  doc["eliminated"] = eliminated_stations;
  doc["networks"] = num_networks;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handle_api_temperature() {
  JsonDocument doc;
  doc["temperature"] = temperatureRead();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}