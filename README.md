# Automatic Irrigation System

An embedded automatic irrigation system that monitors soil moisture and
controls plant watering when the soil becomes too dry.

## Overview

This project was developed to explore practical applications of embedded
systems, sensors, and automation in agriculture and plant care.

The system measures soil moisture and activates a water pump when the measured
moisture level falls below a defined threshold.

## Objectives

- Monitor soil moisture automatically
- Reduce unnecessary manual watering
- Control a water pump using sensor data
- Explore embedded-system design and sensor-based automation

## Main Features

- Soil-moisture monitoring
- Automatic pump control
- Adjustable moisture threshold
- [Manual override, if available]
- [Temperature or humidity monitoring, if available]
- [Display or notification system, if available]

## Hardware

- Microcontroller: [Arduino Uno / ESP32 / other]
- Soil-moisture sensor: [model]
- Water pump: [model or voltage]
- Relay or MOSFET driver: [model]
- Power supply: [specification]
- Additional components: [list]

## Software

- Programming language: [Arduino C++ / C++ / MicroPython / other]
- Development environment: [Arduino IDE / PlatformIO / other]
- Libraries: [list libraries]

## System Diagram

Add your block diagram here.

```text
Soil-Moisture Sensor
          |
          v
    Microcontroller
          |
          v
    Relay or MOSFET
          |
          v
       Water Pump
```

## How It Works

1. The sensor measures the moisture level of the soil.
2. The microcontroller reads and processes the sensor value.
3. If the moisture level is below the selected threshold, the controller
   activates the pump.
4. The pump stops when the soil reaches the desired moisture level.

## Current Status

[Choose one and edit it.]

- Completed and tested as a working prototype.
- Hardware assembled; software testing is in progress.
- Simulation completed; physical prototype planned.
- Ongoing project; sensor calibration and pump testing remain.

## Testing

| Test | Result |
|---|---|
| Sensor reading in dry soil | [Result] |
| Sensor reading in wet soil | [Result] |
| Pump activation below threshold | [Pass/Fail] |
| Pump deactivation above threshold | [Pass/Fail] |

## Future Improvements

- Improve sensor calibration
- Add a water-level sensor
- Add temperature and humidity monitoring
- Add data logging
- Add a mobile or web dashboard
- Improve power protection and enclosure design

## My Contribution

- [Circuit design]
- [Programming]
- [Hardware assembly]
- [Sensor testing]
- [Troubleshooting]
- [Documentation]

## License

This project is licensed under the MIT License. See the `LICENSE` file for
details.
