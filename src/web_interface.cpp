#include <WebServer.h>
#include <ArduinoJson.h>
#include "web_interface.h"
#include "routes/web_routes.h"
#include "routes/api_routes.h"
#include "definitions.h"
#include "deauth.h"

WebServer server(80);
int num_networks;
unsigned long start_time;

void start_web_interface() {
  start_time = millis();
  
  // Web routes
  server.on("/", handle_root);
  server.on("/deauth", HTTP_POST, handle_deauth);
  server.on("/deauth_all", HTTP_POST, handle_deauth_all);
  server.on("/rescan", HTTP_POST, handle_rescan);
  server.on("/stop", HTTP_POST, handle_stop);
  
  // API endpoints
  server.on("/api/networks", handle_api_networks);
  server.on("/api/stats", handle_api_stats);
  server.on("/api/temperature", handle_api_temperature);

  server.begin();
}

void web_interface_handle_client() {
  server.handleClient();
}