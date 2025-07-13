#pragma once
#include <ArduinoJson.h>
#include <WebServer.h>

void sendJsonResponse(bool success, String message);