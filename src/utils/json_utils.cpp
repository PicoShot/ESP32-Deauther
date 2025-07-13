#include "utils/json_utils.h"

extern WebServer server;

void sendJsonResponse(bool success, String message) {
  JsonDocument doc;
  doc["success"] = success;
  doc["message"] = message;
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}