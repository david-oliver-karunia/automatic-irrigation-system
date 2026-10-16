# Wiring Table

## ESP32-S3 and sensors

| Component | Pin | ESP32-S3 connection | Notes |
|---|---|---|---|
| Soil-moisture sensor | AO | GPIO 1 | Confirm output voltage is ESP32-safe |
| Soil-moisture sensor | GND | GND | Common ground |
| Soil-moisture sensor | VCC | Suitable supply | Follow sensor specification |
| HC-SR04 | TRIG | GPIO 5 | Digital output from ESP32 |
| HC-SR04 | ECHO | GPIO 18 through divider | Do not connect 5 V ECHO directly |
| HC-SR04 | VCC | 5 V for standard module | Check exact sensor version |
| HC-SR04 | GND | GND | Common ground |

## Relay and pump

| Component | Connection | Notes |
|---|---|---|
| Relay input | GPIO 7 | Confirm active-LOW or active-HIGH behavior |
| Relay VCC | Suitable relay supply | Follow relay-module specification |
| Relay GND | Common GND | Required for signal reference |
| Pump positive | External pump supply positive | Do not use ESP32 GPIO power |
| Pump negative | Relay or driver output | Depends on switching circuit |
| Flyback diode | Across pump terminals | Cathode toward positive motor supply |
