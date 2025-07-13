#include <WiFi.h>
#include "definitions.h"

extern int num_networks;
extern int eliminated_stations;

String getHtmlTemplate() {
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

  return html;
}