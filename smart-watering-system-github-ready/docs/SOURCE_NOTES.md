# Source Extraction Notes

This repository was prepared from the project report:

**Project Title:** Smart Watering System for Home Gardens

The report states that the software was written using Arduino IDE for ESP32 and was designed to read moisture levels and control the water pump. It also describes integration with ThingsBoard through MQTT for remote monitoring and control.

## Extracted implementation elements

The code shown in the report contains:

- `WiFi.h`
- `Arduino_MQTT_Client.h`
- `ThingsBoard.h`
- `Wire.h`
- `RTClib.h`
- ESP32 Wi-Fi connection
- ThingsBoard MQTT connection
- DS3231 RTC
- Relay 1 on GPIO 26
- Relay 2 on GPIO 27
- Soil moisture analog input on GPIO 34
- Pump state variable
- Pump ON-time tracking
- ThingsBoard RPC callback for `setpumpStatus`
- ThingsBoard RPC request for `getCurrentTime`
- `soilMoisture` telemetry
- `pumpOnTime` telemetry

## Security cleanup

The code screenshot in the report includes credential values for Wi-Fi and ThingsBoard. They have intentionally been replaced with placeholders in this repository.

Use:

```text
SmartWateringSystem/config.h.example
```

to create a local:

```text
SmartWateringSystem/config.h
```

The real configuration file is excluded through `.gitignore`.

## Source fidelity

The repository keeps the documented architecture and named interfaces. It does not invent an unshown soil-moisture threshold algorithm.

The report describes threshold-based watering in its methodology and results sections, while the visible code excerpt demonstrates the sensor-reading, telemetry, relay, RTC, scheduling, and RPC portions.

Any future production enhancements should be documented separately from the extracted implementation.
