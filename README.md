# ESP32-C5 Mini Kit Motor Controller

## Arduino IDE Setup

1. Create or open a sketch named `MotorController`.
2. Put every file in this folder into the same Arduino sketch directory.
3. Select **ESP32-C5-DEV** (or **ESP32-C5 Dev Module**).
4. Set **USB CDC On Boot** to **Enabled**.
5. Compile and upload.

## First-time setup

- Join Wi-Fi network `MotorController-Setup`.
- Password: `motor1234`.
- Open `http://192.168.4.1/`.
- Open **Wi-Fi setup**, choose a network, and save.

## REST API

- `GET /api/status`
- `GET /api/speed?value=50`
- `POST /api/stop`
- `GET /api/networks`

## File responsibilities

- `MotorController.ino`: application entry point.
- `AppConfig.h`: settings, pins, AP credentials and timing.
- `MotorControl.*`: MCP4725 DAC control and speed ramping.
- `MotorNetworkManager.*`: Preferences, station mode, AP fallback and mDNS.
- `WebApp.*`: routes, request validation and JSON.
- `WebPages.*`: embedded HTML, CSS and JavaScript.

## Hardware

### MCP4725 DAC Connection (I2C)

The motor speed is controlled via an I2C MCP4725 12-bit DAC that outputs 0-5V:

- **SDA (Data)**: GPIO 8
- **SCL (Clock)**: GPIO 9
- **I2C Address**: 0x60 (default, A0 pin connected to GND)
- **Vref**: 5V (external power supply)

### Electrical Note

The MCP4725 outputs 0-5V directly to your motor controller. The ESP32-C5 communicates with the DAC via I2C at 3.3V logic levels.

## Libraries Required

Install via Arduino IDE → Sketch → Include Library → Manage Libraries:
- **Adafruit MCP4725** - I2C DAC control
