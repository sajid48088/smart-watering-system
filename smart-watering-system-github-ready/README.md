# 🌱 Smart Watering System for Home Gardens

An ESP32-based IoT smart irrigation system designed to automate garden watering using soil-moisture monitoring, an RTC module, a relay-controlled water pump, Wi-Fi, MQTT, and ThingsBoard.

This repository contains the software implementation documented in the project report **"Smart Watering System for Home Gardens"**.

## ✨ Features

- 🌱 Soil-moisture monitoring using a YL-69 sensor
- 💧 Relay-controlled mini water pump
- ⏰ RTC-based time keeping with DS3231
- 📡 ESP32 Wi-Fi connectivity
- ☁️ ThingsBoard cloud monitoring
- 📬 MQTT communication
- 🎛️ Remote pump control through ThingsBoard RPC
- 📊 Soil-moisture telemetry
- ⏱️ Pump ON-time telemetry
- 🔄 Wi-Fi reconnection handling
- 🕐 ThingsBoard time request through RPC

## 📁 Repository Structure

```text
smart-watering-system/
│
├── SmartWateringSystem/
│   ├── SmartWateringSystem.ino
│   └── config.h.example
│
├── docs/
│   ├── WIRING.md
│   ├── THINGSBOARD.md
│   └── SOURCE_NOTES.md
│
├── .gitignore
├── LICENSE
├── libraries.txt
└── README.md
```

## 🧰 Hardware

The project report identifies these main components:

- ESP32 WROOM-32
- Mini water pump
- RTC module
- YL-69 soil hygrometer / moisture sensor
- 3V single-channel relay
- 5V power source
- Jumper wires

## 🔌 Pin Configuration

| Component | ESP32 GPIO |
|---|---:|
| Relay 1 / Pump | GPIO 26 |
| Relay 2 | GPIO 27 |
| YL-69 analog output | GPIO 34 |

The report's software excerpt defines GPIO 26 and GPIO 27 for relays and GPIO 34 for soil moisture.

## 🛠️ Software Requirements

- Arduino IDE
- ESP32 board support for Arduino
- Wi-Fi-capable ESP32 board
- Required libraries listed in `libraries.txt`

## 📚 Required Libraries

```text
WiFi.h
Arduino_MQTT_Client.h
ThingsBoard.h
Wire.h
RTClib.h
```

`WiFi.h` and `Wire.h` are provided with the ESP32/Arduino environment.

The external libraries are:

- ThingsBoard Arduino SDK
- Arduino MQTT Client
- RTClib

## 🔐 Configuration

The project deliberately does **not** store Wi-Fi passwords or ThingsBoard credentials in the public repository.

Go to:

```text
SmartWateringSystem/config.h.example
```

Copy it as:

```text
SmartWateringSystem/config.h
```

Then set:

```cpp
constexpr char WIFI_SSID[] = "YOUR_WIFI_SSID";
constexpr char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";
constexpr char TOKEN[] = "YOUR_THINGSBOARD_DEVICE_TOKEN";
constexpr char THINGSBOARD_SERVER[] = "thingsboard.cloud";
```

`config.h` is excluded by `.gitignore`.

### Important

The original project report contains real-looking Wi-Fi and ThingsBoard credential values in its code screenshot. Those values have **not** been copied into this public GitHub-ready version.

If those credentials are still active, they should be changed/revoked before public distribution.

## 🚀 Arduino IDE Setup

### 1. Install ESP32 board support

Open Arduino IDE and install the ESP32 board package.

### 2. Install libraries

Install:

- ThingsBoard
- Arduino MQTT Client
- RTClib

### 3. Create local configuration

Copy:

```text
config.h.example
```

to:

```text
config.h
```

Enter your Wi-Fi and ThingsBoard credentials.

### 4. Open the sketch

Open:

```text
SmartWateringSystem/SmartWateringSystem.ino
```

### 5. Select your ESP32 board

Choose the ESP32 board corresponding to your hardware.

### 6. Upload

Connect the ESP32 and upload the sketch.

## ☁️ ThingsBoard

The documented implementation communicates with ThingsBoard using MQTT.

### RPC

The project uses the RPC method:

```text
setpumpStatus
```

A value of:

```text
1
```

turns the pump ON.

A value of:

```text
0
```

turns the pump OFF.

The project also requests current time using:

```text
getCurrentTime
```

### Telemetry

The device sends:

```text
soilMoisture
```

and:

```text
pumpOnTime
```

See `docs/THINGSBOARD.md` for the expected dashboard-side configuration.

## 📊 Soil Moisture

The code reads the ESP32 ADC:

```cpp
analogRead(SOIL_MOISTURE_PIN)
```

using the documented range:

```text
0–4095
```

and converts it to a percentage:

```text
0–100%
```

The mapping is:

```cpp
map(value, 0, 4095, 100, 0)
```

Actual soil-moisture percentage depends on sensor calibration, soil composition, sensor placement, and the electrical characteristics of the particular YL-69 module.

## 🧪 Testing

The project report describes tests for:

- Soil moisture below threshold
- Soil moisture above threshold
- Remote pump control
- Sensor calibration across soil types
- Power consumption

The report states that these tests were successful for the implemented prototype.

## ⚠️ Important Implementation Note

The report describes threshold-based automatic watering, but the visible code excerpt primarily demonstrates sensor telemetry, relay control, RTC handling, scheduled pump activation, and ThingsBoard RPC.

Therefore, this repository does **not invent an additional soil-moisture threshold algorithm** that is not present in the extracted source code.

If you want a production version with explicit configurable dry/wet thresholds, hysteresis, safety timeouts, and non-blocking scheduling, that should be added as a separate enhancement.

## 🔒 Security

Never commit:

- Wi-Fi passwords
- ThingsBoard access tokens
- API keys
- Private certificates
- Other credentials

Use `config.h` locally and keep it ignored by Git.

## 💰 Project Cost

The project report states a total project cost of approximately **800 BDT**.

## 📈 Future Scope

The report proposes:

- AI-assisted watering schedules
- Larger-scale agricultural deployment
- Offline/low-connectivity operation
- Advanced sensors for nutrients, pH, and plant health
- Real-time water-source monitoring
- Solar power integration

## 👥 Project Information

**Project:** Smart Watering System for Home Gardens

**Submission date in report:** 05/10/2024

**Trainers:**
- Riyaz Hasan
- Zubayer Al Billal Khan
- Shababa Binte Hossain

## 📜 License

MIT License. See `LICENSE`.

---

⭐ If this project is useful, consider starring the repository.
