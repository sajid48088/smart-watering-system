# ThingsBoard Configuration

The software communicates with ThingsBoard using MQTT.

## Device Credentials

The ESP32 uses a ThingsBoard device access token:

```text
TOKEN
```

The token is stored locally in `config.h`.

## MQTT Server

The documented implementation uses:

```text
Port: 1883
```

The public example configuration uses:

```text
thingsboard.cloud
```

## RPC Methods

### `setpumpStatus`

ThingsBoard can send a Boolean/integer-style command to the device.

```text
1 → Pump ON
0 → Pump OFF
```

The ESP32 receives the RPC and updates:

```cpp
pumpState
```

The relay state is then controlled by:

```cpp
digitalWrite(RELAY1_PIN, pumpState ? LOW : HIGH);
```

### `getCurrentTime`

The ESP32 requests the current time from ThingsBoard and processes the returned `time` field.

## Telemetry

### Soil Moisture

Key:

```text
soilMoisture
```

Value:

```text
0–100
```

representing the mapped moisture percentage.

### Pump ON Time

Key:

```text
pumpOnTime
```

Value:

```text
seconds
```

The project report describes a ThingsBoard dashboard containing soil moisture and pump ON-time information, along with a manual pump-control switch.

## Suggested Dashboard Widgets

A ThingsBoard dashboard can include:

1. **Switch** → `setpumpStatus`
2. **Gauge** → `soilMoisture`
3. **Value card / chart** → `pumpOnTime`
4. **Time-series chart** → `soilMoisture`

## Security

Do not publish the ThingsBoard token. Keep it in local `config.h`.
