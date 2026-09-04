# ESP32-C3 SuperMini Motor Controller

## Arduino IDE

1. Create or open a sketch named `MotorController`.
2. Put every file in this folder into the same Arduino sketch directory.
3. Select **ESP32C3 Dev Module**.
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
- `MotorControl.*`: PWM and speed state.
- `NetworkManager.*`: Preferences, station mode, AP fallback and mDNS.
- `WebApp.*`: routes, request validation and JSON.
- `WebPages.*`: embedded HTML, CSS and JavaScript.

## Electrical note

The ESP32-C3 GPIO output is 0 to 3.3 V. The motor controller input was measured as 0 to 5 V. Use an appropriate filtered and buffered 0 to 5 V interface if full speed requires 5 V. Never connect 5 V or 24 V to an ESP32 GPIO.
