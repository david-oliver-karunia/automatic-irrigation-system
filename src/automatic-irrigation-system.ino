#include <WiFi.h>
#include <WebServer.h>

const char* WIFI_SSID = "YOUR_WIFI_NAME";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

const int SOIL_PIN = 1;
const int TRIG_PIN = 5;
const int ECHO_PIN = 18;
const int RELAY_PIN = 7;

const int SOIL_DRY_THRESHOLD = 2600;

const float RESERVOIR_EMPTY_DISTANCE_CM = 25.0;
const float RESERVOIR_FULL_DISTANCE_CM = 5.0;

const unsigned long SENSOR_INTERVAL_MS = 2000;
const unsigned long MAX_PUMP_RUNTIME_MS = 15000;

WebServer server(80);

int soilRaw = 0;
float reservoirDistanceCm = -1.0;
float reservoirPercent = -1.0;
bool pumpOn = false;
bool automaticMode = true;
unsigned long lastSensorRead = 0;
unsigned long pumpStartedAt = 0;

float clampFloat(float value, float minimum, float maximum) {
  if (value < minimum) return minimum;
  if (value > maximum) return maximum;
  return value;
}

float readDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0) {
    return -1.0;
  }

  return (duration * 0.0343) / 2.0;
}

float calculateReservoirPercent(float distanceCm) {
  if (distanceCm < 0) {
    return -1.0;
  }

  float percentage =
      ((RESERVOIR_EMPTY_DISTANCE_CM - distanceCm) /
       (RESERVOIR_EMPTY_DISTANCE_CM - RESERVOIR_FULL_DISTANCE_CM)) * 100.0;

  return clampFloat(percentage, 0.0, 100.0);
}

bool reservoirAvailable() {
  return reservoirPercent >= 10.0;
}

void setPump(bool enabled) {
  pumpOn = enabled;

  // Many relay modules are active LOW.
  // Change LOW/HIGH here if your relay behaves differently.
  digitalWrite(RELAY_PIN, enabled ? LOW : HIGH);

  if (enabled) {
    pumpStartedAt = millis();
  }
}

void updateSensors() {
  soilRaw = analogRead(SOIL_PIN);
  reservoirDistanceCm = readDistanceCm();
  reservoirPercent = calculateReservoirPercent(reservoirDistanceCm);

  if (automaticMode) {
    bool soilIsDry = soilRaw > SOIL_DRY_THRESHOLD;
    bool enoughWater = reservoirAvailable();

    if (soilIsDry && enoughWater && !pumpOn) {
      setPump(true);
    }

    if ((!soilIsDry || !enoughWater) && pumpOn) {
      setPump(false);
    }
  }

  if (pumpOn && millis() - pumpStartedAt >= MAX_PUMP_RUNTIME_MS) {
    setPump(false);
  }

  Serial.print("Soil raw: ");
  Serial.print(soilRaw);
  Serial.print(" | Reservoir distance: ");
  Serial.print(reservoirDistanceCm);
  Serial.print(" cm | Reservoir: ");
  Serial.print(reservoirPercent);
  Serial.print("% | Pump: ");
  Serial.println(pumpOn ? "ON" : "OFF");
}

String jsonStatus() {
  String json = "{";
  json += "\"soilRaw\":" + String(soilRaw);
  json += ",\"soilDryThreshold\":" + String(SOIL_DRY_THRESHOLD);
  json += ",\"reservoirDistanceCm\":" + String(reservoirDistanceCm, 1);
  json += ",\"reservoirPercent\":" + String(reservoirPercent, 1);
  json += ",\"pumpOn\":" + String(pumpOn ? "true" : "false");
  json += ",\"automaticMode\":" + String(automaticMode ? "true" : "false");
  json += "}";

  return json;
}

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Automatic Irrigation System</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      max-width: 760px;
      margin: 0 auto;
      padding: 24px;
      background: #f3f6f4;
      color: #1b2a22;
    }

    h1 {
      margin-bottom: 6px;
    }

    .subtitle {
      color: #5f6f65;
      margin-top: 0;
    }

    .grid {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(190px, 1fr));
      gap: 14px;
      margin: 22px 0;
    }

    .card {
      background: white;
      border-radius: 12px;
      padding: 18px;
      box-shadow: 0 2px 8px rgba(0,0,0,0.08);
    }

    .label {
      color: #63756a;
      font-size: 0.9rem;
    }

    .value {
      font-size: 1.7rem;
      font-weight: bold;
      margin-top: 7px;
    }

    button {
      border: 0;
      border-radius: 8px;
      padding: 12px 16px;
      margin: 5px;
      color: white;
      background: #26734d;
      cursor: pointer;
    }

    button.stop {
      background: #ae3e3e;
    }

    .status {
      margin-top: 16px;
      padding: 12px;
      border-radius: 8px;
      background: #e4eee7;
    }
  </style>
</head>
<body>
  <h1>Automatic Irrigation System</h1>
  <p class="subtitle">ESP32-S3 monitoring dashboard</p>

  <div class="grid">
    <div class="card">
      <div class="label">Soil sensor raw value</div>
      <div class="value" id="soil">--</div>
    </div>

    <div class="card">
      <div class="label">Reservoir level</div>
      <div class="value" id="reservoir">--</div>
    </div>

    <div class="card">
      <div class="label">Reservoir distance</div>
      <div class="value" id="distance">--</div>
    </div>

    <div class="card">
      <div class="label">Pump</div>
      <div class="value" id="pump">--</div>
    </div>
  </div>

  <button onclick="setPump('on')">Turn pump on</button>
  <button class="stop" onclick="setPump('off')">Turn pump off</button>

  <div class="status" id="status">
    Waiting for data...
  </div>

  <script>
    async function updateStatus() {
      try {
        const response = await fetch('/api/status');
        const data = await response.json();

        document.getElementById('soil').textContent = data.soilRaw;
        document.getElementById('reservoir').textContent =
          data.reservoirPercent >= 0
            ? data.reservoirPercent.toFixed(1) + '%'
            : 'No reading';

        document.getElementById('distance').textContent =
          data.reservoirDistanceCm >= 0
            ? data.reservoirDistanceCm.toFixed(1) + ' cm'
            : 'No reading';

        document.getElementById('pump').textContent =
          data.pumpOn ? 'ON' : 'OFF';

        document.getElementById('status').textContent =
          data.automaticMode
            ? 'Automatic mode enabled'
            : 'Manual mode enabled';
      } catch (error) {
        document.getElementById('status').textContent =
          'Unable to connect to the ESP32';
      }
    }

    async function setPump(state) {
      await fetch('/api/pump?state=' + state);
      updateStatus();
    }

    updateStatus();
    setInterval(updateStatus, 2000);
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleStatus() {
  server.send(200, "application/json", jsonStatus());
}

void handlePump() {
  if (!server.hasArg("state")) {
    server.send(400, "text/plain", "Missing state");
    return;
  }

  String state = server.arg("state");

  if (state == "on") {
    if (reservoirAvailable()) {
      automaticMode = false;
      setPump(true);
      server.send(200, "text/plain", "Pump turned on");
    } else {
      setPump(false);
      server.send(409, "text/plain", "Reservoir level too low");
    }
    return;
  }

  if (state == "off") {
    automaticMode = false;
    setPump(false);
    server.send(200, "text/plain", "Pump turned off");
    return;
  }

  server.send(400, "text/plain", "Invalid state");
}

void connectToWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected. Open: http://");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // Relay OFF for common active-LOW relay modules.
  digitalWrite(RELAY_PIN, HIGH);

  analogReadResolution(12);

  connectToWiFi();

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/pump", HTTP_GET, handlePump);

  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();

  if (millis() - lastSensorRead >= SENSOR_INTERVAL_MS) {
    lastSensorRead = millis();
    updateSensors();
  }
}
