#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include <Wire.h>

#define GPS_RX_PIN 10
#define GPS_TX_PIN 11
#define GPS_BAUD 115200

SoftwareSerial ssGPS(GPS_RX_PIN, GPS_TX_PIN);
TinyGPS tgps;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for Serial port
  }

  ssGPS.begin(GPS_BAUD);
  Wire.begin();

  Serial.println("WS-M181 GPS test (TinyGPS classic)");
  Serial.println("Lat,Lon,Sats,FixAge");
}

void loop() {
  while (ssGPS.available()) {
    tgps.encode(ssGPS.read());
  }

  long lat = 0, lon = 0;
  unsigned long fix_age = 0;

  tgps.get_position(&lat, &lon, &fix_age);  // lat/lon in 1/1000000 degrees

  float lat_f = lat / 1000000.0;
  float lon_f = lon / 1000000.0;

  uint16_t sats = tgps.satellites();

  Serial.print(lat_f, 7);
  Serial.print(",");
  Serial.print(lon_f, 7);
  Serial.print(",");
  Serial.print(sats);
  Serial.print(",");
  Serial.println(fix_age);

  delay(500);
}
