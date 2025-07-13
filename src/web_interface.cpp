#include <WebServer.h>
#include <ArduinoJson.h>
#include "web_interface.h"
#include "definitions.h"
#include "deauth.h"

WebServer server(80);
int num_networks;
unsigned long start_time;

String getEncryptionType(wifi_auth_mode_t encryptionType);

void sendJsonResponse(bool success, String message) {
  DynamicJsonDocument doc(1024);
  doc["success"] = success;
  doc["message"] = message;
  
  String response;
  serializeJson(doc, response);
  
  server.send(200, "application/json", response);
}

void handle_root() {
  String html = R"(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32-Deauther | PicoShot</title>
    <style>
        :root {
            --bg-primary: #0a0a0a;
            --bg-secondary: #141414;
            --bg-card: #1a1a1a;
            --text-primary: #f0f0f0;
            --text-secondary: #aaaaaa;
            --accent-red: #ff2222;
            --accent-red-glow: rgba(255, 34, 34, 0.15);
            --accent-dark-red: #cc0000;
            --accent-green: #00cc66;
            --border-color: #333333;
            --highlight: rgba(255, 34, 34, 0.08);
            --console-bg: #0a0a0a;
            --grid-line: rgba(255, 34, 34, 0.2);
        }
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
            font-family: 'Courier New', monospace;
            transition: all 0.2s cubic-bezier(0.25, 0.1, 0.25, 1);
        }
        
        body {
            background-color: var(--bg-primary);
            color: var(--text-primary);
            min-height: 100vh;
            line-height: 1.6;
            display: flex;
            flex-direction: column;
            background-image: 
                radial-gradient(var(--accent-red-glow) 1px, transparent 1px),
                radial-gradient(var(--accent-red-glow) 1px, transparent 1px);
            background-size: 50px 50px;
            background-position: 0 0, 25px 25px;
            overflow-x: hidden;
        }
        
        .grid-overlay {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-image: linear-gradient(var(--grid-line) 1px, transparent 1px),
                            linear-gradient(90deg, var(--grid-line) 1px, transparent 1px);
            background-size: 50px 50px;
            z-index: -1;
            opacity: 0.2;
            pointer-events: none;
        }
        
        .noise-overlay {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background-image: url("data:image/svg+xml,%3Csvg viewBox='0 0 400 400' xmlns='http://www.w3.org/2000/svg'%3E%3Cfilter id='noiseFilter'%3E%3CfeTurbulence type='fractalNoise' baseFrequency='0.6' numOctaves='3' stitchTiles='stitch'/%3E%3C/filter%3E%3Crect width='100%25' height='100%25' filter='url(%23noiseFilter)' opacity='0.05'/%3E%3C/svg%3E");
            z-index: -1;
            opacity: 0.3;
            pointer-events: none;
        }
        
        .app-container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            display: grid;
            grid-template-columns: 1fr;
            gap: 25px;
            width: 100%;
        }
        
        .header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            padding: 15px 0;
            border-bottom: 1px solid var(--border-color);
        }
        
        .logo {
            color: var(--accent-red);
            display: flex;
            align-items: center;
            gap: 15px;
        }
        
        .logo-icon {
            font-size: 2rem;
            animation: pulse 2s infinite;
        }
        
        .logo-text {
            font-size: 1.8rem;
            font-weight: bold;
            letter-spacing: 2px;
            text-transform: uppercase;
            text-shadow: 0 0 10px var(--accent-red-glow);
            position: relative;
            overflow: hidden;
        }
        
        .status-bar {
            display: flex;
            align-items: center;
            gap: 20px;
            font-size: 0.9rem;
        }
        
        .status-item {
            display: flex;
            align-items: center;
            gap: 5px;
        }
        
        .status-indicator {
            display: inline-block;
            width: 10px;
            height: 10px;
            border-radius: 50%;
            background-color: var(--accent-green);
        }
        
        .status-indicator.busy {
            background-color: var(--accent-red);
            animation: blink 1s infinite;
        }
        
        .main-content {
            display: grid;
            grid-template-columns: 1fr 300px;
            gap: 25px;
        }
        
        .dashboard {
            display: grid;
            grid-template-rows: auto auto 1fr;
            gap: 25px;
        }
        
        .sidebar {
            display: flex;
            flex-direction: column;
            gap: 25px;
        }
        
        .card {
            background-color: var(--bg-card);
            border-radius: 8px;
            border: 1px solid var(--border-color);
            box-shadow: 0 8px 30px rgba(0, 0, 0, 0.5);
            padding: 20px;
            position: relative;
            overflow: hidden;
        }
        
        .card::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            height: 2px;
            background: linear-gradient(90deg, transparent, var(--accent-red), transparent);
            opacity: 0.8;
        }
        
        .card-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin-bottom: 15px;
            padding-bottom: 15px;
            border-bottom: 1px solid var(--border-color);
        }
        
        .card-title {
            color: var(--accent-red);
            font-size: 1.25rem;
            font-weight: bold;
            display: flex;
            align-items: center;
            gap: 10px;
        }
        
        .card-title .hacker-icon {
            display: inline-block;
            animation: blink 1.5s infinite;
        }
        
        .card-actions {
            display: flex;
            gap: 10px;
        }
        
        .networks-wrapper {
            max-height: 400px;
            overflow-y: auto;
            margin-bottom: 15px;
            scrollbar-width: thin;
            scrollbar-color: var(--accent-red) var(--bg-card);
        }
        
        .networks-wrapper::-webkit-scrollbar {
            width: 6px;
        }
        
        .networks-wrapper::-webkit-scrollbar-track {
            background: var(--bg-card);
        }
        
        .networks-wrapper::-webkit-scrollbar-thumb {
            background-color: var(--accent-red);
            border-radius: 3px;
        }
        
        .networks-table {
            width: 100%;
            border-collapse: collapse;
        }
        
        .networks-table th {
            padding: 12px;
            text-align: left;
            color: var(--text-secondary);
            border-bottom: 1px solid var(--border-color);
            font-size: 0.85rem;
            font-weight: normal;
            text-transform: uppercase;
        }
        
        .networks-table td {
            padding: 15px 12px;
            border-bottom: 1px solid var(--border-color);
        }
        
        .networks-table tr:hover {
            background-color: var(--highlight);
            cursor: pointer;
        }
        
        .networks-table tr.selected {
            background-color: var(--highlight);
            border-left: 3px solid var(--accent-red);
        }
        
        .network-ssid {
            font-weight: bold;
        }
        
        .network-rssi {
            display: flex;
            align-items: center;
            gap: 5px;
        }
        
        .rssi-bars {
            display: inline-block;
            width: 20px;
        }
        
        .encryption-tag {
            display: inline-block;
            font-size: 0.8rem;
            padding: 2px 8px;
            border-radius: 4px;
            background-color: var(--border-color);
        }
        
        .attack-controls {
            display: grid;
            grid-template-columns: 1fr;
            gap: 15px;
        }
        
        .form-group {
            margin-bottom: 15px;
        }
        
        .form-group label {
            display: block;
            margin-bottom: 8px;
            color: var(--text-secondary);
            font-size: 0.9rem;
        }
        
        .select-wrapper {
            position: relative;
        }
        
        .select-wrapper::after {
            content: '⌄';
            position: absolute;
            right: 15px;
            top: 50%;
            transform: translateY(-50%);
            color: var(--accent-red);
            pointer-events: none;
        }
        
        select, input {
            width: 100%;
            padding: 10px 15px;
            border-radius: 4px;
            background-color: var(--bg-secondary);
            border: 1px solid var(--border-color);
            color: var(--text-primary);
            appearance: none;
        }
        
        select:focus, input:focus {
            outline: none;
            border-color: var(--accent-red);
            box-shadow: 0 0 0 2px var(--accent-red-glow);
        }
        
        .btn {
            display: inline-flex;
            align-items: center;
            justify-content: center;
            gap: 10px;
            padding: 12px 20px;
            border-radius: 4px;
            background-color: var(--bg-secondary);
            border: 1px solid var(--border-color);
            color: var(--text-primary);
            font-weight: bold;
            cursor: pointer;
            text-transform: uppercase;
            transition: all 0.2s;
            font-size: 0.9rem;
            letter-spacing: 1px;
        }
        
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 5px 15px rgba(0, 0, 0, 0.2);
        }
        
        .btn:active {
            transform: translateY(1px);
        }
        
        .btn-primary {
            background-color: var(--accent-red);
            border-color: var(--accent-red);
        }
        
        .btn-primary:hover {
            background-color: var(--accent-dark-red);
        }
        
        .btn-group {
            display: flex;
            gap: 10px;
        }
        
        .btn-group .btn {
            flex: 1;
        }
        
        .console {
            background-color: var(--console-bg);
            border-radius: 8px;
            border: 1px solid var(--border-color);
            padding: 15px;
            font-family: monospace;
            font-size: 0.85rem;
            color: var(--text-primary);
            height: 200px;
            overflow-y: auto;
            margin-top: 15px;
        }
        
        .console-line {
            margin-bottom: 5px;
            line-height: 1.4;
            display: flex;
        }
        
        .console-prompt {
            color: var(--accent-red);
            margin-right: 8px;
        }
        
        .console-message {
            word-break: break-word;
        }
        
        .console-message.success {
            color: var(--accent-green);
        }
        
        .console-message.error {
            color: var(--accent-red);
        }
        
        .stats-card {
            display: grid;
            grid-template-columns: repeat(auto-fit, minmax(120px, 1fr));
            gap: 15px;
        }
        
        .stat-item {
            text-align: center;
            padding: 15px;
            background-color: var(--bg-secondary);
            border-radius: 6px;
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
        }
        
        .stat-value {
            font-size: 2rem;
            font-weight: bold;
            color: var(--accent-red);
            margin-bottom: 5px;
        }
        
        .stat-label {
            font-size: 0.8rem;
            color: var(--text-secondary);
            text-transform: uppercase;
        }
        
        .reason-codes {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(140px, 1fr));
            gap: 10px;
            margin-top: 10px;
        }
        
        .reason-code-item {
            background-color: var(--bg-secondary);
            border-radius: 4px;
            padding: 10px;
            cursor: pointer;
            text-align: center;
            border: 1px solid var(--border-color);
        }
        
        .reason-code-item:hover {
            background-color: var(--highlight);
            transform: translateY(-2px);
        }
        
        .reason-code-value {
            color: var(--accent-red);
            font-weight: bold;
            font-size: 1.2rem;
            margin-bottom: 5px;
        }
        
        .reason-code-text {
            font-size: 0.75rem;
            color: var(--text-secondary);
        }
        
        .credit {
            text-align: center;
            padding: 15px;
            font-size: 0.8rem;
            color: var(--text-secondary);
            border-top: 1px solid var(--border-color);
            margin-top: 20px;
        }
        
        .credit a {
            color: var(--accent-red);
            text-decoration: none;
        }
        
        .notification {
            position: fixed;
            top: 20px;
            right: 20px;
            padding: 15px 20px;
            background-color: var(--bg-card);
            border-left: 4px solid var(--accent-red);
            border-radius: 4px;
            box-shadow: 0 5px 15px rgba(0, 0, 0, 0.3);
            display: flex;
            align-items: center;
            gap: 10px;
            transform: translateX(120%);
            opacity: 0;
            transition: all 0.3s cubic-bezier(0.68, -0.55, 0.27, 1.55);
            z-index: 100;
            max-width: 300px;
        }
        
        .notification.show {
            transform: translateX(0);
            opacity: 1;
        }
        
        .notification-icon {
            color: var(--accent-red);
            font-size: 1.2rem;
        }
        
        .notification-content {
            flex: 1;
        }
        
        .notification-title {
            font-weight: bold;
            font-size: 0.9rem;
            margin-bottom: 2px;
        }
        
        .notification-message {
            font-size: 0.8rem;
            color: var(--text-secondary);
        }
        
        .loader {
            display: none;
            position: relative;
            width: 20px;
            height: 20px;
        }
        
        .loader:after {
            content: '';
            display: block;
            width: 100%;
            height: 100%;
            border-radius: 50%;
            border: 2px solid var(--accent-red);
            border-color: var(--accent-red) transparent var(--accent-red) transparent;
            animation: loader-spin 1.2s linear infinite;
        }
        
        @keyframes loader-spin {
            0% { transform: rotate(0deg); }
            100% { transform: rotate(360deg); }
        }
        
        .loader.show {
            display: inline-block;
        }
        
        @keyframes blink {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.3; }
        }
        
        @keyframes pulse {
            0% { transform: scale(1); }
            50% { transform: scale(1.1); }
            100% { transform: scale(1); }
        }
        
        @keyframes scan {
            0% { background-position: 0% 0%; }
            100% { background-position: 100% 0%; }
        }
        
        @keyframes glitch {
            0% { transform: translate(0); }
            20% { transform: translate(-3px, 3px); }
            40% { transform: translate(-3px, -3px); }
            60% { transform: translate(3px, 3px); }
            80% { transform: translate(3px, -3px); }
            100% { transform: translate(0); }
        }
        
        .glitch-effect {
            animation: glitch 1s linear infinite;
            animation-play-state: paused;
        }
        
        .glitch-effect.active {
            animation-play-state: running;
        }
        
        /* Mobile Responsiveness */
        @media (max-width: 992px) {
            .main-content {
                grid-template-columns: 1fr;
            }
            
            .header {
                flex-direction: column;
                align-items: flex-start;
                gap: 15px;
            }
            
            .status-bar {
                width: 100%;
                justify-content: space-between;
            }
            
            .btn-group {
                flex-direction: column;
            }
        }
        
        @media (max-width: 768px) {
            .networks-wrapper {
                overflow-x: auto;
            }
            
            .networks-table {
                min-width: 650px;
            }
            
            .btn-group {
                flex-direction: column;
            }
            
            .sidebar {
                margin-top: 20px;
            }
            
            .stats-card {
                grid-template-columns: repeat(2, 1fr);
            }
        }
    </style>
</head>
<body>
    <div class="grid-overlay"></div>
    <div class="noise-overlay"></div>
    
    <div class="app-container">
        <header class="header">
            <div class="logo">
                <div class="logo-icon">⚡</div>
                <div class="logo-text">ESP32-Deauther</div>
            </div>
            <div class="status-bar">
                <div class="status-item">
                    <span class="status-indicator" id="attack-status"></span>
                    <span id="status-text">Idle</span>
                </div>
                <div class="status-item">
                    <span id="time">00:00:00</span>
                </div>
            </div>
        </header>
        
        <main class="main-content">
            <div class="dashboard">
                <div class="card">
                    <div class="card-header">
                        <h2 class="card-title"><span class="hacker-icon">▶</span> WiFi Networks</h2>
                        <div class="card-actions">
                            <button id="scan-btn" class="btn">
                                <span id="scan-loader" class="loader"></span>
                                Scan Networks
                            </button>
                        </div>
                    </div>
                    <div class="networks-wrapper">
                        <table class="networks-table">
                            <thead>
                                <tr>
                                    <th>SSID</th>
                                    <th>BSSID</th>
                                    <th>Channel</th>
                                    <th>Signal</th>
                                    <th>Security</th>
                                </tr>
                            </thead>
                            <tbody id="networks-list">
)";

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

  html += R"(
                            </tbody>
                        </table>
                    </div>
                </div>
                
                <div class="card">
                    <div class="card-header">
                        <h2 class="card-title"><span class="hacker-icon">⚔</span> Attack Controls</h2>
                    </div>
                    <form id="attack-form" class="attack-controls">
                        <div class="form-group">
                            <label for="target-select">Target Network</label>
                            <div class="select-wrapper">
                                <select id="target-select" required>
                                    <option value="" selected disabled>Select a target network...</option>
)";

  for (int i = 0; i < num_networks; i++) {
    html += "<option value='" + String(i) + "'>" + WiFi.SSID(i) + " (" + WiFi.BSSIDstr(i) + ")</option>";
  }

  html += R"(
                                </select>
                            </div>
                        </div>
                        
                        <div class="form-group">
                            <label for="reason-select">Deauth Reason</label>
                            <div class="select-wrapper">
                                <select id="reason-select" required>
                                    <option value="" selected disabled>Select a reason code...</option>
                                    <option value="1">1: Unspecified reason</option>
                                    <option value="2">2: Auth expired</option>
                                    <option value="3">3: Station leaving</option>
                                    <option value="4">4: Inactivity</option>
                                    <option value="5">5: AP too busy</option>
                                    <option value="6">6: Class 2 frame from non-auth station</option>
                                    <option value="7">7: Class 3 frame from non-assoc station</option>
                                </select>
                            </div>
                        </div>
                        
                        <div class="btn-group">
                            <button type="submit" id="attack-btn" class="btn btn-primary">
                                <span id="attack-loader" class="loader"></span>
                                Launch Attack
                            </button>
                            <button type="button" id="stop-btn" class="btn">Stop Attack</button>
                        </div>
                    </form>
                    
                    <div class="console" id="console">
                        <div class="console-line">
                            <span class="console-prompt">></span>
                            <span class="console-message">System initialized. Ready for commands.</span>
                        </div>
                    </div>
                </div>
                
                <div class="card">
                    <div class="card-header">
                        <h2 class="card-title"><span class="hacker-icon">🔄</span> Deauth All Networks</h2>
                    </div>
                    <form id="deauth-all-form" class="attack-controls">
                        <div class="form-group">
                            <label for="reason-all-select">Deauth Reason</label>
                            <div class="select-wrapper">
                                <select id="reason-all-select" required>
                                    <option value="" selected disabled>Select a reason code...</option>
                                    <option value="1">1: Unspecified reason</option>
                                    <option value="2">2: Auth expired</option>
                                    <option value="3">3: Station leaving</option>
                                    <option value="4">4: Inactivity</option>
                                    <option value="5">5: AP too busy</option>
                                    <option value="6">6: Class 2 frame from non-auth station</option>
                                    <option value="7">7: Class 3 frame from non-assoc station</option>
                                </select>
                            </div>
                        </div>
                        
                        <button type="submit" id="deauth-all-btn" class="btn btn-primary">
                            <span id="deauth-all-loader" class="loader"></span>
                            Deauth All Networks
                        </button>
                        <p style="margin-top: 10px; font-size: 0.8rem; color: var(--text-secondary);">⚠️ Warning: This will deauth ALL networks and may require restarting the ESP32.</p>
                    </form>
                </div>
            </div>
            
            <div class="sidebar">
                <div class="card">
                    <div class="card-header">
                        <h2 class="card-title"><span class="hacker-icon">📊</span> Statistics</h2>
                    </div>
                    <div class="stats-card">
                        <div class="stat-item">
                            <div class="stat-value" id="network-count">)" + String(num_networks) + R"(</div>
                            <div class="stat-label">Networks</div>
                        </div>
                        <div class="stat-item">
                            <div class="stat-value" id="eliminated-count">)" + String(eliminated_stations) + R"(</div>
                            <div class="stat-label">Eliminated</div>
                        </div>
                        <div class="stat-item">
                            <div class="stat-value" id="uptime-count">0</div>
                            <div class="stat-label">Uptime (m)</div>
                        </div>
                        <div class="stat-item">
                            <div class="stat-value" id="temp-count">--</div>
                            <div class="stat-label">Temp (°C)</div>
                        </div>
                    </div>
                </div>
                
                <div class="card">
                    <div class="card-header">
                        <h2 class="card-title"><span class="hacker-icon">🔢</span> Reason Codes</h2>
                    </div>
                    <div class="reason-codes">
                        <div class="reason-code-item" data-code="1">
                            <div class="reason-code-value">1</div>
                            <div class="reason-code-text">Unspecified</div>
                        </div>
                        <div class="reason-code-item" data-code="2">
                            <div class="reason-code-value">2</div>
                            <div class="reason-code-text">Auth Expired</div>
                        </div>
                        <div class="reason-code-item" data-code="3">
                            <div class="reason-code-value">3</div>
                            <div class="reason-code-text">Leaving</div>
                        </div>
                        <div class="reason-code-item" data-code="4">
                            <div class="reason-code-value">4</div>
                            <div class="reason-code-text">Inactivity</div>
                        </div>
                        <div class="reason-code-item" data-code="5">
                            <div class="reason-code-value">5</div>
                            <div class="reason-code-text">AP Too Busy</div>
                        </div>
                        <div class="reason-code-item" data-code="6">
                            <div class="reason-code-value">6</div>
                            <div class="reason-code-text">Class 2 Frame</div>
                        </div>
                        <div class="reason-code-item" data-code="7">
                            <div class="reason-code-value">7</div>
                            <div class="reason-code-text">Class 3 Frame</div>
                        </div>
                        <div class="reason-code-item" data-code="8">
                            <div class="reason-code-value">8</div>
                            <div class="reason-code-text">Leaving BSS</div>
                        </div>
                    </div>
                </div>
            </div>
        </main>
        
        <footer class="credit">
            Powered by <a href="#" target="_blank">PicoShot</a> &copy; 2025
        </footer>
        
        <div class="notification" id="notification">
            <div class="notification-icon">!</div>
            <div class="notification-content">
                <div class="notification-title" id="notification-title">Success</div>
                <div class="notification-message" id="notification-message">Operation completed successfully.</div>
            </div>
        </div>
    </div>
    
    <script>
        const appState = {
            attacking: false,
            selectedNetwork: null,
            stats: {
                networks: )" + String(num_networks) + R"(,
                eliminated: )" + String(eliminated_stations) + R"(,
                uptime: 0
            },
            updateInterval: null
        };
        
        const networksList = document.getElementById('networks-list');
        const targetSelect = document.getElementById('target-select');
        const reasonSelect = document.getElementById('reason-select');
        const reasonAllSelect = document.getElementById('reason-all-select');
        const consoleEl = document.getElementById('console');
        const attackStatus = document.getElementById('attack-status');
        const statusText = document.getElementById('status-text');
        const networkCountEl = document.getElementById('network-count');
        const eliminatedCountEl = document.getElementById('eliminated-count');
        const uptimeCountEl = document.getElementById('uptime-count');
        const tempCountEl = document.getElementById('temp-count');
        const timeEl = document.getElementById('time');
        
        document.addEventListener('DOMContentLoaded', () => {
            initEventListeners();
            startClock();
            updateStats();
            setupUptime();
            setupTemperatureUpdates();
            
            const logoText = document.querySelector('.logo-text');
            typeWriter(logoText.textContent, logoText);
        });
        
        function initEventListeners() {
            document.querySelectorAll('#networks-list tr').forEach(row => {
                row.addEventListener('click', () => selectNetwork(row));
            });
            
            document.querySelectorAll('.reason-code-item').forEach(item => {
                item.addEventListener('click', () => selectReasonCode(item));
            });
            document.getElementById('scan-btn').addEventListener('click', scanNetworks);
            document.getElementById('attack-form').addEventListener('submit', launchAttack);
            document.getElementById('stop-btn').addEventListener('click', stopAttack);
            document.getElementById('deauth-all-form').addEventListener('submit', deauthAll);
        }
        
        function selectNetwork(row) {
            document.querySelectorAll('#networks-list tr').forEach(r => r.classList.remove('selected'));
            row.classList.add('selected');
            const networkId = row.getAttribute('data-id');
            targetSelect.value = networkId;
            appState.selectedNetwork = networkId;
            logToConsole(`Selected network: ${row.querySelector('.network-ssid').textContent} (ID: ${networkId})`);
        }
        
        function selectReasonCode(item) {
            const code = item.getAttribute('data-code');
            reasonSelect.value = code;
            reasonAllSelect.value = code;
            
            item.classList.add('glitch-effect', 'active');
            setTimeout(() => {
                item.classList.remove('glitch-effect', 'active');
            }, 500);
            
            logToConsole(`Selected reason code: ${code}`);
        }
        
        function scanNetworks(e) {
            e.preventDefault();
            const loader = document.getElementById('scan-loader');
            loader.classList.add('show');
            logToConsole('Scanning for WiFi networks...');
            fetch('/rescan', {
                method: 'POST'
            })
            .then(response => {
                if(!response.ok) throw new Error('Network scan failed');
                return response.json();
            })
            .then(data => {
                updateNetworksList();
                showNotification('Success', 'Network scan completed');
                logToConsole('Network scan completed successfully', 'success');
            })
            .catch(error => {
                showNotification('Error', 'Failed to scan networks');
                logToConsole(`Error: ${error.message}`, 'error');
            })
            .finally(() => {
                loader.classList.remove('show');
            });
        }
        
        function updateNetworksList() {
            fetch('/api/networks')
            .then(response => {
                if(!response.ok) throw new Error('Failed to get networks');
                return response.json();
            })
            .then(data => {
                networksList.innerHTML = '';
                
                appState.stats.networks = data.networks.length;
                networkCountEl.textContent = appState.stats.networks;
                
                targetSelect.innerHTML = '<option value="" selected disabled>Select a target network...</option>';
                
                data.networks.forEach(network => {
                    const row = document.createElement('tr');
                    row.setAttribute('data-id', network.id);
                    row.innerHTML = `
                        <td class="network-ssid">${network.ssid}</td>
                        <td>${network.bssid}</td>
                        <td>${network.channel}</td>
                        <td class="network-rssi">${network.rssi} dBm</td>
                        <td><span class="encryption-tag">${network.encryption}</span></td>
                    `;
                    
                    row.addEventListener('click', () => selectNetwork(row));
                    networksList.appendChild(row);
                    const option = document.createElement('option');
                    option.value = network.id;
                    option.textContent = `${network.ssid} (${network.bssid})`;
                    targetSelect.appendChild(option);
                });
            })
            .catch(error => {
                logToConsole(`Error updating networks list: ${error.message}`, 'error');
            });
        }
        
        function launchAttack(e) {
            e.preventDefault();
            
            const networkId = targetSelect.value;
            const reasonCode = reasonSelect.value;
            
            if (!networkId || !reasonCode) {
                showNotification('Error', 'Please select both network and reason code');
                return;
            }
            const loader = document.getElementById('attack-loader');
            loader.classList.add('show');
            logToConsole(`Launching deauth attack on network ID ${networkId} with reason code ${reasonCode}...`);
            
            const formData = new FormData();
            formData.append('net_num', networkId);
            formData.append('reason', reasonCode);
            
            fetch('/deauth', {
                method: 'POST',
                body: formData
            })
            .then(response => {
                if(!response.ok) throw new Error('Attack failed to start');
                return response.json();
            })
            .then(data => {
                appState.attacking = true;
                updateAttackStatus(true);
                showNotification('Success', `Attack launched on network ID ${networkId}`);
                logToConsole(`Attack launched successfully. Target: Network ID ${networkId}, Reason: ${reasonCode}`, 'success');
                
                startStatsUpdate();
            })
            .catch(error => {
                showNotification('Error', 'Failed to launch attack');
                logToConsole(`Error: ${error.message}`, 'error');
            })
            .finally(() => {
                loader.classList.remove('show');
            });
        }
        
        function stopAttack(e) {
            e.preventDefault();
            logToConsole('Stopping attack...');
            fetch('/stop', {
                method: 'POST'
            })
            .then(response => {
                if(!response.ok) throw new Error('Failed to stop attack');
                return response.json();
            })
            .then(data => {
                appState.attacking = false;
                updateAttackStatus(false);
                showNotification('Success', 'Attack stopped successfully');
                logToConsole('Attack stopped successfully', 'success');
                stopStatsUpdate();
            })
            .catch(error => {
                showNotification('Error', 'Failed to stop attack');
                logToConsole(`Error: ${error.message}`, 'error');
            });
        }
        function deauthAll(e) {
            e.preventDefault();
            
            const reasonCode = reasonAllSelect.value;
            
            if (!reasonCode) {
                showNotification('Error', 'Please select a reason code');
                return;
            }
            
            if (!confirm('⚠️ WARNING: This will deauth ALL networks and may require restarting the ESP32. Continue?')) {
                return;
            }

            const loader = document.getElementById('deauth-all-loader');
            loader.classList.add('show');
            logToConsole(`Launching deauth attack on ALL networks with reason code ${reasonCode}...`);
            
            const formData = new FormData();
            formData.append('reason', reasonCode);
            
            fetch('/deauth_all', {
                method: 'POST',
                body: formData
            })
            .then(response => {
                if(!response.ok) throw new Error('Deauth all failed to start');
                return response.json();
            })
            .then(data => {
                appState.attacking = true;
                updateAttackStatus(true);
                showNotification('Success', 'Deauth All attack launched');
                logToConsole(`Deauth All attack launched successfully. Reason: ${reasonCode}`, 'success');
                logToConsole('⚠️ ESP32 may need to be restarted to regain control', 'error');
            })
            .catch(error => {
                updateAttackStatus(true);
                showNotification('Notice', 'Deauth All attack likely launched');
                logToConsole(`ESP32 is likely in Deauth All mode. Server connection lost as expected.`, 'success');
                logToConsole('⚠️ ESP32 may need to be restarted to regain control', 'error');
            })
            .finally(() => {
                loader.classList.remove('show');
            });
        }
        
        function logToConsole(message, type = '') {
            const line = document.createElement('div');
            line.className = 'console-line';
            
            const prompt = document.createElement('span');
            prompt.className = 'console-prompt';
            prompt.textContent = '>';
            
            const messageEl = document.createElement('span');
            messageEl.className = `console-message ${type}`;
            messageEl.textContent = message;
            
            line.appendChild(prompt);
            line.appendChild(messageEl);
            
            consoleEl.appendChild(line);
            consoleEl.scrollTop = consoleEl.scrollHeight;
        }
        
        function showNotification(title, message) {
            const notification = document.getElementById('notification');
            const titleEl = document.getElementById('notification-title');
            const messageEl = document.getElementById('notification-message');
            
            titleEl.textContent = title;
            messageEl.textContent = message;
            
            notification.classList.add('show');
            
            setTimeout(() => {
                notification.classList.remove('show');
            }, 3000);
        }
        
        function updateAttackStatus(attacking) {
            const statusIndicator = document.getElementById('attack-status');
            const statusText = document.getElementById('status-text');
            
            if (attacking) {
                statusIndicator.classList.add('busy');
                statusText.textContent = 'Attacking';
            } else {
                statusIndicator.classList.remove('busy');
                statusText.textContent = 'Idle';
            }
        }
        
        function startStatsUpdate() {
            if (appState.updateInterval) return;
            
            appState.updateInterval = setInterval(updateStats, 2000);
        }
        
        function stopStatsUpdate() {
            if (!appState.updateInterval) return;
            
            clearInterval(appState.updateInterval);
            appState.updateInterval = null;
        }
        
        function updateStats() {
            fetch('/api/stats')
            .then(response => {
                if(!response.ok) throw new Error('Failed to get stats');
                return response.json();
            })
            .then(data => {
                appState.stats.eliminated = data.eliminated;
                eliminatedCountEl.textContent = appState.stats.eliminated;
                
                if (appState.stats.eliminated > parseInt(eliminatedCountEl.textContent)) {
                    logToConsole(`Successfully eliminated a station. Total: ${appState.stats.eliminated}`, 'success');
                }
            })
            .catch(error => {
                console.error('Error updating stats:', error);
            });
        }
        
        function setupUptime() {
            let seconds = 0;
            
            setInterval(() => {
                seconds++;
                const minutes = Math.floor(seconds / 60);
                uptimeCountEl.textContent = minutes;
            }, 1000);
        }
        
        function setupTemperatureUpdates() {
            updateTemperature();
            setInterval(updateTemperature, 5000);
        }
        
        function updateTemperature() {
            fetch('/api/temperature')
            .then(response => {
                if(!response.ok) throw new Error('Failed to get temperature');
                return response.json();
            })
            .then(data => {
                if (data.temperature !== undefined) {
                    tempCountEl.textContent = Math.round(data.temperature);
                }
            })
            .catch(error => {
                console.error('Error updating temperature:', error);
                tempCountEl.textContent = '--';
            });
        }
        
        function startClock() {
            setInterval(() => {
                const now = new Date();
                const hours = String(now.getHours()).padStart(2, '0');
                const minutes = String(now.getMinutes()).padStart(2, '0');
                const seconds = String(now.getSeconds()).padStart(2, '0');
                timeEl.textContent = `${hours}:${minutes}:${seconds}`;
            }, 1000);
        }
        
        function typeWriter(text, element) {
            element.textContent = '';
            let i = 0;
            
            const type = () => {
                if (i < text.length) {
                    element.textContent += text.charAt(i);
                    i++;
                    setTimeout(type, 100);
                }
            };
            
            type();
        }
    </script>
</body>
</html>
)";

  server.send(200, "text/html", html);
}

void handle_api_networks() {
  DynamicJsonDocument doc(4096);
  JsonArray networksArray = doc.createNestedArray("networks");
  
  for (int i = 0; i < num_networks; i++) {
    JsonObject network = networksArray.createNestedObject();
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
  DynamicJsonDocument doc(256);
  doc["eliminated"] = eliminated_stations;
  doc["networks"] = num_networks;
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handle_api_temperature() {
  DynamicJsonDocument doc(128);
  doc["temperature"] = temperatureRead();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
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

void start_web_interface() {
  start_time = millis();
  
  // web routes
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

String getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch (encryptionType) {
    case WIFI_AUTH_OPEN:
      return "Open";
    case WIFI_AUTH_WEP:
      return "WEP";
    case WIFI_AUTH_WPA_PSK:
      return "WPA_PSK";
    case WIFI_AUTH_WPA2_PSK:
      return "WPA2_PSK";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA_WPA2_PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2_ENTERPRISE";
    case WIFI_AUTH_WPA3_PSK:
      return "WPA3_PSK";
    case WIFI_AUTH_WPA2_WPA3_PSK:
      return "WPA2_WPA3_PSK";
    case WIFI_AUTH_WAPI_PSK:
      return "WAPI_PSK";
    default:
      return "UNKNOWN";
  }
}