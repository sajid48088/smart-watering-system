# Wiring / Hardware Notes

This document summarizes the hardware connections explicitly represented by the software implementation in the project report.

## ESP32

The ESP32 WROOM-32 is the central controller.

## Relay 1 / Water Pump

```text
ESP32 GPIO 26
      │
      ▼
   Relay 1
      │
      ▼
  Water Pump
```

The software uses:

```cpp
digitalWrite(RELAY1_PIN, LOW);
```

to activate the pump in the documented relay configuration.

## Relay 2

Relay 2 is assigned to:

```text
GPIO 27
```

The source initializes it but does not otherwise use it for the main pump-control logic.

## Soil Moisture Sensor

The YL-69 analog signal is connected to:

```text
ESP32 GPIO 34
```

The software reads:

```cpp
analogRead(SOIL_MOISTURE_PIN);
```

## RTC

The report identifies an RTC module and the software uses:

```cpp
RTC_DS3231 rtc;
```

The RTC communicates through the ESP32's I2C interface.

## Power

The report identifies a 5V power source.

Use a suitable regulated supply and ensure that the pump's current requirements are not being supplied directly from an ESP32 GPIO.
