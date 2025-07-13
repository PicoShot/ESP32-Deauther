#include <WiFi.h>
#include "definitions.h"

extern int num_networks;
extern int eliminated_stations;

String getHtmlTemplateEnd() {
  String html = R"(
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

  return html;
}