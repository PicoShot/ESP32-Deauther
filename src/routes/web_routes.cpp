#include <WebServer.h>
#include <WiFi.h>
#include "routes/web_routes.h"
#include "web_assets/html_template.h"
#include "utils/json_utils.h"
#include "deauth.h"
#include "definitions.h"

extern WebServer server;
extern int num_networks;

void handle_root() {
  String html = getHtmlTemplate();
  html += generateNetworkRows();
  html += getHtmlTemplateEnd();
  
  server.send(200, "text/html", html);
}

void handle_deauth() {
  int wifi_number = server.arg("net_num").toInt();
  uint16_t reason = server.arg("reason").toInt();

  if (wifi_number < num_networks) {
    start_deauth(wifi_number, DEAUTH_TYPE_SINGLE, reason);
    sendJsonResponse(true, "Attack launched successfully");
  } else {
    sendJsonResponse(false, "Invalid network number");
  }
}

void handle_deauth_all() {
  uint16_t reason = server.arg("reason").toInt();
  
  sendJsonResponse(true, "Deauth All attack launched");
  
  delay(100);
  
  server.stop();
  start_deauth(0, DEAUTH_TYPE_ALL, reason);
}

void handle_stop() {
  stop_deauth();
  sendJsonResponse(true, "Attack stopped successfully");
}

void handle_rescan() {
  num_networks = WiFi.scanNetworks();
  sendJsonResponse(true, "Scan completed successfully");
}