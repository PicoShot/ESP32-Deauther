#pragma once
#include <WebServer.h>

extern WebServer server;
extern int num_networks;
extern unsigned long start_time;

void start_web_interface();
void web_interface_handle_client();