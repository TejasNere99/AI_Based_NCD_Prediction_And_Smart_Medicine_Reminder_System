#include <WiFi.h>
#include <WebServer.h>

// =====================================================
// WIFI
// =====================================================

const char* WIFI_SSID = "OPPO Reno11 5G";
const char* WIFI_PASSWORD = "Tejas1209111";

// =====================================================
// SERVER
// =====================================================

WebServer server(80);

// =====================================================
// DASHBOARD TIME
// =====================================================

int currentHour = 0;
int currentMinute = 0;
int currentSecond = 0;

int currentDay = 0;
int currentMonth = 0;
int currentYear = 0;

unsigned long lastSyncMillis = 0;

// =====================================================
// CORS
// =====================================================

void sendCORS() {

  server.sendHeader(
    "Access-Control-Allow-Origin",
    "*"
  );

  server.sendHeader(
    "Access-Control-Allow-Methods",
    "GET,POST,OPTIONS"
  );

  server.sendHeader(
    "Access-Control-Allow-Headers",
    "Content-Type"
  );
}

// =====================================================
// ROOT
// =====================================================

void handleRoot() {

  sendCORS();

  server.send(
    200,
    "text/plain",
    "ESP32 Time Sync Server ONLINE"
  );
}

// =====================================================
// TIME SYNC API
// =====================================================

void handleTimeSync() {

  sendCORS();

  if (!server.hasArg("hour") ||
      !server.hasArg("minute") ||
      !server.hasArg("second") ||
      !server.hasArg("day") ||
      !server.hasArg("month") ||
      !server.hasArg("year")) {

    server.send(
      400,
      "text/plain",
      "Missing time parameters"
    );

    return;
  }

  // ---------------------------------------------------
  // Receive time
  // ---------------------------------------------------

  currentHour =
    server.arg("hour").toInt();

  currentMinute =
    server.arg("minute").toInt();

  currentSecond =
    server.arg("second").toInt();

  currentDay =
    server.arg("day").toInt();

  currentMonth =
    server.arg("month").toInt();

  currentYear =
    server.arg("year").toInt();

  lastSyncMillis = millis();

  // ---------------------------------------------------
  // Serial output
  // ---------------------------------------------------

  Serial.println();
  Serial.println("================================");
  Serial.println("      DASHBOARD TIME SYNC");
  Serial.println("================================");

  Serial.print("Date: ");

  Serial.print(currentDay);
  Serial.print("/");
  Serial.print(currentMonth);
  Serial.print("/");
  Serial.println(currentYear);

  Serial.print("Time: ");

  if (currentHour < 10)
    Serial.print("0");

  Serial.print(currentHour);
  Serial.print(":");

  if (currentMinute < 10)
    Serial.print("0");

  Serial.print(currentMinute);
  Serial.print(":");

  if (currentSecond < 10)
    Serial.print("0");

  Serial.println(currentSecond);

  Serial.println("Time synchronized successfully.");
  Serial.println("================================");

  server.send(
    200,
    "text/plain",
    "Time synchronized successfully"
  );
}

// =====================================================
// OPTIONS
// =====================================================

void handleOptions() {

  sendCORS();

  server.send(204);
}

// =====================================================
// DISPLAY CURRENT TIME
// =====================================================

void printCurrentTime() {

  Serial.print("ESP32 Time: ");

  if (currentHour < 10)
    Serial.print("0");

  Serial.print(currentHour);
  Serial.print(":");

  if (currentMinute < 10)
    Serial.print("0");

  Serial.print(currentMinute);
  Serial.print(":");

  if (currentSecond < 10)
    Serial.print("0");

  Serial.println(currentSecond);
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("========================================");
  Serial.println(" SMART MEDICINE DISPENSER");
  Serial.println(" TEST 7D-1");
  Serial.println(" DASHBOARD TIME → ESP32");
  Serial.println("========================================");

  // ---------------------------------------------------
  // WiFi
  // ---------------------------------------------------

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi Connected!");

  Serial.print("ESP32 IP Address: ");

  Serial.println(WiFi.localIP());

  // ---------------------------------------------------
  // Routes
  // ---------------------------------------------------

  server.on(
    "/",
    HTTP_GET,
    handleRoot
  );

  server.on(
    "/api/time",
    HTTP_POST,
    handleTimeSync
  );

  server.on(
    "/api/time",
    HTTP_OPTIONS,
    handleOptions
  );

  // ---------------------------------------------------
  // Start server
  // ---------------------------------------------------

  server.begin();

  Serial.println("HTTP Server Started");

  Serial.println();
  Serial.println("Waiting for dashboard time...");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  server.handleClient();

  // ---------------------------------------------------
  // Increase time using millis()
  // ---------------------------------------------------

  if (millis() - lastSyncMillis >= 1000) {

    lastSyncMillis += 1000;

    currentSecond++;

    if (currentSecond >= 60) {

      currentSecond = 0;

      currentMinute++;

      if (currentMinute >= 60) {

        currentMinute = 0;

        currentHour++;

        if (currentHour >= 24) {

          currentHour = 0;
        }
      }
    }
  }
}