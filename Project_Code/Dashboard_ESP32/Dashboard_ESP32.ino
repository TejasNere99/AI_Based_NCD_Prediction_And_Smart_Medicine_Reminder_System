#include <WiFi.h>
#include <WebServer.h>

// =====================================================
// WIFI
// =====================================================

const char* WIFI_SSID = "OPPO Reno11 5G";
const char* WIFI_PASSWORD = "Tejas1209111";

// =====================================================
// WEB SERVER
// =====================================================

WebServer server(80);

// =====================================================
// SCHEDULE VARIABLES
// =====================================================

int receivedSlot = 0;

String receivedMedicine = "";
String receivedTime = "";

// =====================================================
// CORS
// =====================================================

void sendCORS() {

  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

// =====================================================
// ROOT
// =====================================================

void handleRoot() {

  sendCORS();

  server.send(
    200,
    "text/plain",
    "Smart Medicine Dispenser ESP32 is ONLINE"
  );
}

// =====================================================
// SCHEDULE API
// =====================================================

void handleSchedule() {

  sendCORS();

  // ---------------------------------------------------
  // Read POST parameters
  // ---------------------------------------------------

  if (!server.hasArg("slot") ||
      !server.hasArg("medicine") ||
      !server.hasArg("time")) {

    server.send(
      400,
      "text/plain",
      "Missing schedule parameters"
    );

    return;
  }

  receivedSlot = server.arg("slot").toInt();

  receivedMedicine = server.arg("medicine");

  receivedTime = server.arg("time");

  // ---------------------------------------------------
  // Serial output
  // ---------------------------------------------------

  Serial.println();
  Serial.println("================================");
  Serial.println("       SCHEDULE RECEIVED");
  Serial.println("================================");

  Serial.print("Slot     : ");
  Serial.println(receivedSlot);

  Serial.print("Medicine : ");
  Serial.println(receivedMedicine);

  Serial.print("Time     : ");
  Serial.println(receivedTime);

  Serial.println("================================");
  Serial.println();

  // ---------------------------------------------------
  // Response to Dashboard
  // ---------------------------------------------------

  server.send(
    200,
    "text/plain",
    "Schedule received successfully"
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
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println(" SMART MEDICINE DISPENSER");
  Serial.println(" TEST 7B - WIFI COMMUNICATION");
  Serial.println("================================");

  // ---------------------------------------------------
  // WiFi
  // ---------------------------------------------------

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

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

  server.on("/", HTTP_GET, handleRoot);

  server.on(
    "/api/schedule",
    HTTP_POST,
    handleSchedule
  );

  server.on(
    "/api/schedule",
    HTTP_OPTIONS,
    handleOptions
  );

  // ---------------------------------------------------
  // Start Server
  // ---------------------------------------------------

  server.begin();

  Serial.println("HTTP Server Started");

  Serial.println();
  Serial.println("Waiting for dashboard...");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  server.handleClient();
}