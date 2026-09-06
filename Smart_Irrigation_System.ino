/*
   ============================================================
                 JALRAKSHAK SMART IRRIGATION SYSTEM
   ============================================================

   Project: IoT-Based Smart Irrigation System

   Features:
   - 4-zone irrigation system
   - Soil moisture monitoring
   - Automatic irrigation control
   - Wi-Fi-based monitoring
   - Independent control for each zone

   Microcontroller: ESP32
*/

#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi settings
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

WebServer server(80);

// Soil moisture sensor pins
#define SOIL_SENSOR_1 34
#define SOIL_SENSOR_2 35
#define SOIL_SENSOR_3 32
#define SOIL_SENSOR_4 33

// Relay pins
#define RELAY_1 25
#define RELAY_2 26
#define RELAY_3 27
#define RELAY_4 14

// Moisture threshold (%)
int moistureThreshold = 60;

// Sensor and moisture variables
int sensorValue1, sensorValue2, sensorValue3, sensorValue4;
int moisture1, moisture2, moisture3, moisture4;

void handleRoot();
int convertToPercentage(int sensorValue);
void controlIrrigation(int moisture, int relayPin, String zoneName);

void setup() {
  Serial.begin(115200);

  Serial.println("======================================");
  Serial.println("JALRAKSHAK SMART IRRIGATION SYSTEM");
  Serial.println("======================================");

  pinMode(SOIL_SENSOR_1, INPUT);
  pinMode(SOIL_SENSOR_2, INPUT);
  pinMode(SOIL_SENSOR_3, INPUT);
  pinMode(SOIL_SENSOR_4, INPUT);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(RELAY_4, OUTPUT);

  // Active-LOW relay module: HIGH = OFF
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.begin();

  Serial.println("Web Server Started!");
}

void loop() {
  server.handleClient();

  // Read soil moisture sensors
  sensorValue1 = analogRead(SOIL_SENSOR_1);
  sensorValue2 = analogRead(SOIL_SENSOR_2);
  sensorValue3 = analogRead(SOIL_SENSOR_3);
  sensorValue4 = analogRead(SOIL_SENSOR_4);

  // Convert readings to percentage
  moisture1 = convertToPercentage(sensorValue1);
  moisture2 = convertToPercentage(sensorValue2);
  moisture3 = convertToPercentage(sensorValue3);
  moisture4 = convertToPercentage(sensorValue4);

  Serial.println("\n--------- JALRAKSHAK DATA ---------");
  Serial.printf("Zone 1 Moisture: %d%%\n", moisture1);
  Serial.printf("Zone 2 Moisture: %d%%\n", moisture2);
  Serial.printf("Zone 3 Moisture: %d%%\n", moisture3);
  Serial.printf("Zone 4 Moisture: %d%%\n", moisture4);

  // Control irrigation independently for each zone
  controlIrrigation(moisture1, RELAY_1, "Zone 1");
  controlIrrigation(moisture2, RELAY_2, "Zone 2");
  controlIrrigation(moisture3, RELAY_3, "Zone 3");
  controlIrrigation(moisture4, RELAY_4, "Zone 4");

  delay(3000);
}

int convertToPercentage(int sensorValue) {
  /*
     CALIBRATION REQUIRED:

     Replace these example values with your actual readings.

     Example:
     Dry soil reading = 3500
     Wet soil reading = 1500
  */

  int moisturePercentage = map(
    sensorValue,
    3500,  // Dry soil
    1500,  // Wet soil
    0,
    100
  );

  return constrain(moisturePercentage, 0, 100);
}

void controlIrrigation(int moisture, int relayPin, String zoneName) {

  if (moisture < moistureThreshold) {
    digitalWrite(relayPin, LOW); // Irrigation ON
    Serial.println(zoneName + ": Soil Dry - Irrigation ON");
  }
  else {
    digitalWrite(relayPin, HIGH); // Irrigation OFF
    Serial.println(zoneName + ": Moisture Sufficient - Irrigation OFF");
  }
}

void handleRoot() {

  String html = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>JALRAKSHAK Smart Irrigation</title>
</head>
<body style="font-family:Arial;text-align:center;">
  <h1>JALRAKSHAK</h1>
  <h2>Smart IoT-Based Irrigation System</h2>
)rawliteral";

  html += "<h3>Zone 1</h3><p>Soil Moisture: " + String(moisture1) + "%</p>";
  html += "<h3>Zone 2</h3><p>Soil Moisture: " + String(moisture2) + "%</p>";
  html += "<h3>Zone 3</h3><p>Soil Moisture: " + String(moisture3) + "%</p>";
  html += "<h3>Zone 4</h3><p>Soil Moisture: " + String(moisture4) + "%</p>";

  html += R"rawliteral(
  <br>
  <p>Water Conservation Through Intelligent Irrigation</p>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}
