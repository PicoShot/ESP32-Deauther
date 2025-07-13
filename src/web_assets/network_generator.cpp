#include <WiFi.h>
#include "utils/wifi_utils.h"

extern int num_networks;

String generateNetworkRows() {
  String html = "";
  
  for (int i = 0; i < num_networks; i++) {
    String encryption = getEncryptionType(WiFi.encryptionType(i));
    int rssi = WiFi.RSSI(i);
    int rssiPercentage = map(rssi, -100, -50, 0, 100);
    rssiPercentage = constrain(rssiPercentage, 0, 100);
    
    int signalBars = map(rssiPercentage, 0, 100, 0, 4);
    
    html += "<tr data-id='" + String(i) + "'><td class='network-ssid'>" + WiFi.SSID(i) + 
            "</td><td>" + WiFi.BSSIDstr(i) + "</td><td>" + String(WiFi.channel(i)) + 
            "</td><td class='network-rssi'>" + String(rssi) + " dBm</td><td><span class='encryption-tag'>" + encryption + "</span></td></tr>";
  }
  
  return html;
}