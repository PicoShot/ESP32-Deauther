# ESP32-Deauther

A WiFi deauthentication tool built for ESP32 microcontrollers with a modern web interface. This project demonstrates WiFi security concepts and network analysis capabilities.

## DISCLAIMER

**This project is made for learning and educational purposes only. I do not accept any responsibility for any trouble, damage, or illegal activities that may result from the use of this software. Users are solely responsible for ensuring their use complies with local laws and regulations.**

## Features

 ### Pros

  - 2.4GHz WiFi Support
  - Modern Web Interface
  - Real-time Network Scanning
  - Multiple Attack Modes
  - Live Statistics
  - Low Cost Hardware
  - Portable
  - Customizable Reason Codes

 ### Cons

  - No 5GHz Support
  - Limited Range
  - No WPA3 Deauth
  - Single Threaded


<p align="center">
  <img src="images/main-page.jpg" alt="Main Page" width="600"/>
</p>

  
## Requirements

- **ESP32 Development Board** (ESP32-WROOM-32, ESP32 DevKit v1, etc.)
- **VSCode** (With PlatformIO extension)
- **CP2102 Driver** (Linux already has this driver in its kernel)
## Installation

### Using PlatformIO (Recommended)

1. **Clone the repository:**
   ```bash
   git clone https://github.com/PicoShot/ESP32-Deauther.git
   cd ESP32-Deauther
   ```

2. **Open in VSCode (make sure you have been installed PlatformIO extension)**

3. **Upload to ESP32**

##  Usage

1. **Power on the ESP32** - It will create a WiFi Access Point
2. **Connect to the AP:**
   - **SSID:** `ESP32-Deauther` (default)
   - **Password:** `1234+abc` (default)
3. **Open web browser** and navigate to: `http://192.168.4.1`
4. **Scan for networks** using the "Scan Networks" button
5. **Select target network** from the list
6. **Choose deauth reason code**
7. **Launch attack** or use "Deauth All" for broader impact

## Configuration

Edit `include/definitions.h` to customize:

```cpp
#define AP_SSID "ESP32-Deauther"    // Access Point name
#define AP_PASS "1234+abc"          // Access Point password
#define CHANNEL_MAX 13              // Maximum WiFi channel
// #define LED 2                    // Uncomment to enable LED (if you have one)
// #define SERIAL_DEBUG             // Uncomment for serial output
```

---

**Remember: With great power comes great responsibility. Use this tool ethically and legally.**
