# Automatic Irrigation System

An ESP32-S3-based automatic irrigation system that monitors soil moisture,
estimates the water level in a reservoir, and controls a water pump when the
plant's soil becomes too dry.

The project explores practical applications of embedded systems, sensors,
automation, web interfaces, and intelligent decision-making in plant care and
small-scale agriculture.

## Overview

The system uses a soil-moisture sensor to determine whether the soil is dry
and an HC-SR04 ultrasonic sensor to estimate the remaining water in the
reservoir.

When the soil is below the configured moisture threshold and sufficient water
is available, the ESP32-S3 activates a water pump through a relay. The ESP32-S3
also hosts a basic web dashboard for monitoring sensor readings and controlling
the pump manually.

Future versions may integrate local weather data and AI-assisted irrigation
decisions through an external application.

## Objectives

- Monitor soil moisture automatically.
- Estimate the remaining water in the reservoir.
- Reduce unnecessary manual watering.
- Control a water pump using sensor data.
- Provide a basic browser-based monitoring interface.
- Explore embedded-system design and sensor-based automation.
- Investigate how weather data and AI can improve irrigation decisions.

## Main Features

- Soil-moisture monitoring.
- Reservoir-level estimation using an HC-SR04 ultrasonic sensor.
- Automatic pump control.
- Manual pump control through a web interface.
- Adjustable soil-moisture threshold.
- Low-reservoir protection.
- ESP32-S3 Wi-Fi connectivity.
- Serial-monitor output for debugging.
- Expandable architecture for weather API and AI integration.

## System Architecture

```text
                         ┌─────────────────────┐
                         │  Soil-Moisture Sensor│
                         └──────────┬──────────┘
                                    │
                                    v
┌─────────────────────┐    ┌─────────────────────┐    ┌──────────────┐
│ HC-SR04 Reservoir   │───>│     ESP32-S3        │───>│ Relay/Driver │
│ Level Sensor        │    │                     │    └──────┬───────┘
└─────────────────────┘    └──────────┬──────────┘           │
                                      │                      v
                                      │                ┌─────────────┐
                                      │                │ Water Pump  │
                                      │                └─────────────┘
                                      │
                                      v
                               Wi-Fi Web Dashboard

                    Future external inputs:
                    Local weather API and AI application
```

## How It Works

1. The soil-moisture sensor measures the condition of the soil.
2. The ESP32-S3 reads and processes the sensor value.
3. The HC-SR04 measures the distance between the sensor and the water surface.
4. The measured distance is converted into an estimated reservoir percentage.
5. If the soil is too dry and enough water is available, the ESP32-S3 activates
   the pump through the relay.
6. The pump stops when the soil reaches the configured condition, the reservoir
   level is too low, or the maximum pump runtime is reached.
7. The ESP32-S3 provides sensor readings and pump status through a web dashboard.
8. Future versions may use local weather data and AI-assisted recommendations
   before activating the pump.

The HC-SR04 measures distance to the water surface rather than water volume
directly. The reservoir percentage therefore requires calibration based on the
shape and dimensions of the actual tank.

## Hardware

- **Microcontroller:** ESP32-S3 development board.
- **Soil-moisture sensor:** Analog soil-moisture sensor.
- **Reservoir-level sensor:** HC-SR04 ultrasonic distance sensor.
- **Water pump:** DC water pump with a suitable external power supply.
- **Pump switch:** Relay module or protected MOSFET driver.
- **Protection:** Flyback diode across the DC pump terminals.
- **Voltage divider:** Required for the HC-SR04 ECHO signal when using a standard
  5 V HC-SR04 module.
- **Reservoir:** Water container with a suitable sensor mounting position.
- **Tubing:** Used to deliver water to the plant.
- **Additional components:** Resistors, wiring, connectors, fuse, and enclosure
  materials as required.

## Electrical Connections

### Soil-moisture sensor

| Sensor pin | ESP32-S3 connection |
|---|---|
| VCC | Suitable sensor supply voltage |
| GND | GND |
| AO | GPIO 1 |

Confirm that the sensor's analog output voltage is safe for the ESP32-S3 input.
If necessary, use a voltage divider or level-shifting circuit.

### HC-SR04

| HC-SR04 pin | Connection |
|---|---|
| VCC | 5 V supply for a standard module |
| GND | Common GND |
| TRIG | ESP32-S3 GPIO 5 |
| ECHO | ESP32-S3 GPIO 18 through a voltage divider |

A typical voltage-divider arrangement is:

```text
HC-SR04 ECHO ── 1 kΩ ──┬── ESP32-S3 GPIO 18
                       |
                      2 kΩ
                       |
                      GND
```

The voltage divider reduces the ECHO signal to a safer level for the ESP32-S3.
The exact resistor values may be adjusted, but the resulting GPIO voltage must
remain within the ESP32-S3 input limits.

### Relay and pump

| Component | Connection |
|---|---|
| Relay control input | ESP32-S3 GPIO 7 |
| Relay VCC | Suitable relay-module supply |
| Relay GND | Common GND, if required |
| Pump supply | Separate external pump power supply |
| Pump switching path | Relay contacts or suitable MOSFET driver |

The ESP32-S3 must not power the water pump directly. The pump requires a
separate supply capable of providing its startup and operating current.

For a DC pump, connect a flyback diode across the pump terminals:

```text
Pump positive ────────|<|──────── Pump negative
                     Diode
```

For the common low-side switching arrangement, the diode cathode is connected
toward the positive motor supply and the anode toward the switched negative
side. Confirm the switching arrangement before wiring.

## Circuit Documentation

The `electronics/` directory contains the electrical design files.

Recommended contents:

```text
electronics/
├── irrigation-system.kicad_pro
├── irrigation-system.kicad_sch
├── schematic.png
├── wiring-table.md
└── bill-of-materials.csv
```

The KiCad schematic documents the electrical relationships between the ESP32-S3,
sensors, relay, pump power system, protection components, and voltage divider.

The wiring table provides a simpler pin-by-pin connection reference, while the
bill of materials lists the components used in the prototype.

## Software

- **Firmware language:** Arduino C++.
- **Development environment:** Arduino IDE or PlatformIO.
- **Microcontroller framework:** Arduino framework for ESP32.
- **Wi-Fi library:** `WiFi.h`.
- **Web-server library:** `WebServer.h`.
- **Optional future software:** Python, web application backend, weather API,
  database, and AI/ML services.

## Firmware Structure

```text
firmware/
└── automatic_irrigation.ino
```

The firmware is responsible for:

- Connecting the ESP32-S3 to Wi-Fi.
- Reading the soil-moisture sensor.
- Measuring reservoir distance.
- Estimating reservoir level.
- Controlling the pump relay.
- Enforcing a maximum pump runtime.
- Serving the web dashboard.
- Providing sensor data through a JSON status endpoint.

## Web Dashboard

The ESP32-S3 hosts a basic web dashboard on the local network.

The dashboard displays:

- Soil sensor reading.
- Estimated reservoir percentage.
- Reservoir distance.
- Pump state.
- Automatic/manual control status.

Example endpoints:

```text
GET /
GET /api/status
GET /api/pump?state=on
GET /api/pump?state=off
```

To access the dashboard:

1. Upload the firmware to the ESP32-S3.
2. Open the Serial Monitor at 115200 baud.
3. Wait for the ESP32-S3 to connect to Wi-Fi.
4. Copy the IP address printed in the Serial Monitor.
5. Open that IP address in a browser connected to the same network.

## Setup

1. Install the ESP32 board package in Arduino IDE.
2. Select the appropriate ESP32-S3 board.
3. Open `firmware/automatic_irrigation.ino`.
4. Enter the Wi-Fi network name and password.
5. Confirm the GPIO assignments.
6. Check the relay's active-LOW or active-HIGH behavior.
7. Upload the firmware.
8. Open the Serial Monitor at 115200 baud.
9. Open the ESP32-S3 IP address in a browser.

Do not publish Wi-Fi passwords, API keys, or other credentials in this
repository.

## Control Logic

The current prototype uses the following basic control logic:

```text
If soil is dry
and reservoir water is sufficient:
    turn the pump on

Otherwise:
    turn the pump off
```

The system also stops the pump when the maximum runtime is reached. This helps
reduce the risk of continuous pumping caused by an incorrect sensor reading or
unexpected software behavior.

## Weather API and AI Integration

Weather and AI functions are planned extensions rather than core hardware
functions.

Potential future inputs include:

- Rain probability.
- Local weather forecast.
- Temperature.
- Humidity.
- Historical soil-moisture data.
- Plant type and watering requirements.
- Previous watering duration.
- AI-assisted watering recommendations.

A future control system could use logic such as:

```text
If the soil is dry
and the reservoir has enough water
and significant rain is not expected:
    recommend or activate watering
```

The AI system should initially provide recommendations or explanations rather
than directly controlling the pump without safety limits.

## Current Status

The project is an ongoing embedded-system prototype.

### Completed or planned components

- ESP32-S3-based control architecture.
- Soil-moisture sensing.
- HC-SR04 reservoir-level sensing.
- Relay-controlled pump concept.
- Basic web-dashboard architecture.
- KiCad electrical documentation.
- Future weather and AI integration plan.

Update this section as the project develops:

```text
- [ ] Complete circuit assembly
- [ ] Calibrate soil-moisture sensor
- [ ] Calibrate reservoir-level measurement
- [ ] Test relay and pump separately
- [ ] Test automatic watering logic
- [ ] Test web dashboard
- [ ] Add weather API
- [ ] Add AI-assisted decision support
- [ ] Design enclosure
- [ ] Complete long-duration testing
```

## Testing

| Test | Expected result | Actual result | Status |
|---|---|---|---|
| Sensor reading in dry soil | Reading indicates dry condition | [Result] | [Pass/Fail] |
| Sensor reading in wet soil | Reading indicates wet condition | [Result] | [Pass/Fail] |
| Reservoir distance at full level | Distance matches calibration | [Result] | [Pass/Fail] |
| Reservoir distance at low level | Distance indicates low water | [Result] | [Pass/Fail] |
| Pump activation below threshold | Pump turns on | [Result] | [Pass/Fail] |
| Pump deactivation above threshold | Pump turns off | [Result] | [Pass/Fail] |
| Pump stops at low reservoir level | Pump remains off | [Result] | [Pass/Fail] |
| Maximum pump runtime | Pump stops automatically | [Result] | [Pass/Fail] |
| Web dashboard connection | Dashboard displays correctly | [Result] | [Pass/Fail] |
| Wi-Fi disconnection behavior | System enters a safe state | [Result] | [Pass/Fail] |

## Calibration

### Soil-moisture calibration

Record the sensor's raw readings in:

- Dry soil.
- Moist soil.
- Very wet soil.

Use these measurements to determine a suitable threshold rather than relying
on a generic value.

### Reservoir calibration

Measure the HC-SR04 distance at known reservoir levels:

- Empty.
- 25% full.
- 50% full.
- 75% full.
- Full.

Use the measurements to improve the estimated reservoir percentage.

## Limitations

- The soil-moisture sensor requires calibration for the selected soil.
- Some resistive soil sensors may corrode over time.
- The HC-SR04 estimates level from distance and does not directly measure
  volume.
- Water movement, foam, condensation, and tank geometry can affect ultrasonic
  readings.
- The standard HC-SR04 may require voltage protection on the ECHO signal.
- The basic firmware does not yet use weather data or AI.
- Wi-Fi failure may prevent dashboard access.
- The prototype has not been validated for long-term outdoor operation.
- This system is not a certified agricultural or safety-critical controller.

## Safety

- Keep water away from exposed electrical connections.
- Use a suitable enclosure for outdoor or humid environments.
- Use an appropriate external power supply for the pump.
- Do not power the pump directly from an ESP32-S3 GPIO pin.
- Use suitable fusing and wiring for the pump current.
- Verify relay ratings before switching the pump.
- Test the pump circuit separately before connecting it to the automatic logic.
- Do not expose Wi-Fi passwords or API credentials in the source code.
- Supervise the prototype during testing.

This project is intended for education and experimentation. It is not a
medical, agricultural, or safety-certified control system.

## Future Improvements

- Improve soil-moisture calibration.
- Replace or compare resistive and capacitive soil sensors.
- Add pump-current monitoring.
- Add a physical water-level sensor as a second measurement method.
- Add temperature and humidity monitoring.
- Add data logging.
- Add historical charts.
- Add local weather API integration.
- Add AI-assisted irrigation recommendations.
- Add notifications through a web or mobile application.
- Add sensor-fault detection.
- Add hysteresis and moving-average filtering.
- Improve power protection.
- Design a custom PCB in KiCad.
- Design a weather-resistant enclosure in FreeCAD.
- Perform long-duration and outdoor testing.

## Project Structure

```text
automatic-irrigation-system/
├── README.md
├── LICENSE
├── firmware/
│   └── automatic_irrigation.ino
├── web/
│   └── dashboard.html
├── electronics/
│   ├── irrigation-system.kicad_pro
│   ├── irrigation-system.kicad_sch
│   ├── schematic.png
│   ├── wiring-table.md
│   └── bill-of-materials.csv
├── docs/
│   ├── system-design.md
│   └── testing-log.md
├── images/
│   ├── prototype.jpg
│   ├── circuit.jpg
│   └── dashboard.png
└── data/
    └── sample-readings.csv
```

## My Contribution

- Designed the embedded-system architecture.
- Selected and integrated the ESP32-S3 controller.
- Integrated the soil-moisture and reservoir-level sensors.
- Designed the pump-control interface.
- Documented the electrical connections in KiCad.
- Developed the ESP32 firmware.
- Created the local web dashboard.
- Performed sensor calibration and hardware testing.
- Investigated future weather and AI integration.
- Documented design decisions, limitations, and future improvements.

Update this section so that it reflects only the work actually completed.

## License

This project is licensed under the MIT License. See the `LICENSE` file for
details.
