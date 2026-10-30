#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>

// =====================================================
// WIFI
// =====================================================

const char* WIFI_SSID = "OPPO Reno11 5G";
const char* WIFI_PASSWORD = "Tejas1209111";

WebServer server(80);

// =====================================================
// PINS
// =====================================================

#define CAROUSEL_SERVO_PIN 18
#define GATE1_SERVO_PIN 19
#define GATE2_SERVO_PIN 25

#define IR_PIN 34
#define BUZZER_PIN 23
#define LED_PIN 2

// =====================================================
// LCD
// =====================================================

LiquidCrystal_I2C lcd(0x27, 16, 2);

// =====================================================
// SERVOS
// =====================================================

Servo carouselServo;
Servo gate1Servo;
Servo gate2Servo;

// Change these if your physical gate mechanism needs
// different angles.

const int GATE_CLOSED = 0;
const int GATE_OPEN = 90;

// =====================================================
// SCHEDULE
// =====================================================

struct MedicineSchedule {

  int slot;

  String medicine;

  String time;
};

MedicineSchedule scheduleData[8];

// =====================================================
// TIME
// =====================================================

int currentHour = 0;
int currentMinute = 0;
int currentSecond = 0;

int currentDay = 0;
int currentMonth = 0;
int currentYear = 0;

unsigned long lastTimeTick = 0;

String lastMedicineStatus = "WAITING";
int lastStatusSlot = 0;
String lastStatusMedicine = "";
String lastStatusScheduledTime = "";
String lastStatusTakenTime = "";

// =====================================================
// MEDICINE EVENT
// =====================================================

bool medicineActive = false;

int activeSlot = -1;

unsigned long medicineStartMillis = 0;

const unsigned long MEDICINE_TIMEOUT = 50000;

// =====================================================
// PREVENT REPEATED TRIGGER
// =====================================================

bool slotTriggered[8] = {
  false, false, false, false,
  false, false, false, false
};

int lastProcessedMinute = -1;

// =====================================================
// CORS
// =====================================================

void sendCORS() {

  server.sendHeader(
    "Access-Control-Allow-Origin",
    "*");

  server.sendHeader(
    "Access-Control-Allow-Methods",
    "GET,POST,OPTIONS");

  server.sendHeader(
    "Access-Control-Allow-Headers",
    "Content-Type");
}

// =====================================================
// ROOT
// =====================================================

void handleRoot() {

  sendCORS();

  server.send(
    200,
    "text/plain",
    "Smart Medicine Dispenser FULL SYSTEM ONLINE");
}

const int CAROUSEL_STEP_DELAY = 30;

int currentCarouselAngle = 0;

void moveCarouselSmooth(int targetAngle) {

  if (targetAngle > currentCarouselAngle) {

    for (
      int angle = currentCarouselAngle;
      angle <= targetAngle;
      angle++) {

      carouselServo.write(angle);

      delay(CAROUSEL_STEP_DELAY);
    }
  }

  else if (targetAngle < currentCarouselAngle) {

    for (
      int angle = currentCarouselAngle;
      angle >= targetAngle;
      angle--) {

      carouselServo.write(angle);

      delay(CAROUSEL_STEP_DELAY);
    }
  }

  currentCarouselAngle =
    targetAngle;
}

// =====================================================
// TIME SYNC
// =====================================================

void handleTimeSync() {

  sendCORS();

  if (!server.hasArg("hour") || !server.hasArg("minute") || !server.hasArg("second") || !server.hasArg("day") || !server.hasArg("month") || !server.hasArg("year")) {

    server.send(
      400,
      "text/plain",
      "Missing time parameters");

    return;
  }

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

  lastTimeTick = millis();

  Serial.println();
  Serial.println("========== TIME SYNC ==========");

  Serial.printf(
    "Date: %02d/%02d/%04d\n",
    currentDay,
    currentMonth,
    currentYear);

  Serial.printf(
    "Time: %02d:%02d:%02d\n",
    currentHour,
    currentMinute,
    currentSecond);

  Serial.println("===============================");

  server.send(
    200,
    "text/plain",
    "Time synchronized successfully");
}

// =====================================================
// RECEIVE ALL 8 SLOTS
// =====================================================
// =====================================================
// RECEIVE SINGLE SLOT
// =====================================================

void handleSingleSchedule() {

  sendCORS();

  if (
    !server.hasArg("slot") || !server.hasArg("medicine") || !server.hasArg("time")) {

    server.send(
      400,
      "text/plain",
      "Missing slot, medicine or time");

    return;
  }

  int slot =
    server.arg("slot").toInt();

  String medicine =
    server.arg("medicine");

  String time =
    server.arg("time");

  // -----------------------------------------------
  // Validate slot
  // -----------------------------------------------

  if (
    slot < 1 || slot > 8) {

    server.send(
      400,
      "text/plain",
      "Invalid slot number");

    return;
  }

  // -----------------------------------------------
  // Store slot
  // -----------------------------------------------

  scheduleData[slot - 1].slot =
    slot;

  scheduleData[slot - 1].medicine =
    medicine;

  scheduleData[slot - 1].time =
    time;

  slotTriggered[slot - 1] =
    false;

  Serial.println();
  Serial.println(
    "======= SINGLE SLOT RECEIVED =======");

  Serial.print(
    "Slot: ");

  Serial.println(
    slot);

  Serial.print(
    "Medicine: ");

  Serial.println(
    medicine);

  Serial.print(
    "Time: ");

  Serial.println(
    time);

  Serial.println(
    "====================================");

  server.send(
    200,
    "text/plain",
    "Slot " + String(slot) + " stored successfully");
}
// =====================================================
// RECEIVE CONFIGURED SCHEDULE
// =====================================================

void handleCompleteSchedule() {

  sendCORS();

  // ===================================================
  // CLEAR OLD SCHEDULE FIRST
  // ===================================================

  for (int i = 0; i < 8; i++) {

    scheduleData[i].slot =
      i + 1;

    scheduleData[i].medicine =
      "";

    scheduleData[i].time =
      "";

    slotTriggered[i] =
      false;
  }

  // ===================================================
  // READ ONLY PROVIDED SLOTS
  // ===================================================

  int configuredCount = 0;

  for (int i = 0; i < 8; i++) {

    String slotKey =
      "slot" + String(i + 1);

    String medicineKey =
      "medicine" + String(i + 1);

    String timeKey =
      "time" + String(i + 1);

    // -----------------------------------------------
    // Empty slot = ignore
    // -----------------------------------------------

    if (
      !server.hasArg(medicineKey) || !server.hasArg(timeKey)) {

      continue;
    }

    String medicine =
      server.arg(medicineKey);

    String time =
      server.arg(timeKey);

    // -----------------------------------------------
    // Ignore incomplete/empty slot
    // -----------------------------------------------

    if (
      medicine.length() == 0 || time.length() == 0) {

      continue;
    }

    int slot =
      i + 1;

    if (server.hasArg(slotKey)) {

      slot =
        server.arg(slotKey).toInt();
    }

    if (
      slot < 1 || slot > 8) {

      continue;
    }

    // -----------------------------------------------
    // Store
    // -----------------------------------------------

    scheduleData[slot - 1].slot =
      slot;

    scheduleData[slot - 1].medicine =
      medicine;

    scheduleData[slot - 1].time =
      time;

    slotTriggered[slot - 1] =
      false;

    configuredCount++;
  }

  // ===================================================
  // SERIAL OUTPUT
  // ===================================================

  Serial.println();
  Serial.println(
    "======= SCHEDULE RECEIVED =======");

  Serial.print(
    "Configured slots: ");

  Serial.println(
    configuredCount);

  for (int i = 0; i < 8; i++) {

    if (
      scheduleData[i].medicine.length() > 0 && scheduleData[i].time.length() > 0) {

      Serial.print(
        "Slot ");

      Serial.print(
        scheduleData[i].slot);

      Serial.print(
        " | ");

      Serial.print(
        scheduleData[i].medicine);

      Serial.print(
        " | ");

      Serial.println(
        scheduleData[i].time);
    }
  }

  Serial.println(
    "=================================");

  // ===================================================
  // RESPONSE
  // ===================================================

  server.send(
    200,
    "text/plain",
    String(configuredCount) + " configured slot(s) stored successfully");
}

void handleStatus() {

  sendCORS();

  String json = "{";

  json += "\"status\":\"" + lastMedicineStatus + "\",";
  json += "\"slot\":" + String(lastStatusSlot) + ",";
  json += "\"medicine\":\"" + lastStatusMedicine + "\",";
  json += "\"scheduledTime\":\"" + lastStatusScheduledTime + "\",";
  json += "\"takenTime\":\"" + lastStatusTakenTime + "\"";

  json += "}";

  server.send(
    200,
    "application/json",
    json);
}

// =====================================================
// OPTIONS
// =====================================================

void handleOptions() {

  sendCORS();

  server.send(204);
}

// =====================================================
// CAROUSEL ANGLE
// =====================================================

int getCarouselAngle(int slot) {

  switch (slot) {

    case 1:
    case 5:
      return 0;

    case 2:
    case 6:
      return 45;

    case 3:
    case 7:
      return 90;

    case 4:
    case 8:
      return 135;

    default:
      return 0;
  }
}

// =====================================================
// GATE SELECTION
// =====================================================

bool isGate1(int slot) {

  return slot >= 1 && slot <= 4;
}

// =====================================================
// OPEN GATE
// =====================================================

void openGate(int slot) {

  if (isGate1(slot)) {

    Serial.println("Opening GATE 1");

    gate1Servo.write(GATE_OPEN);

  } else {

    Serial.println("Opening GATE 2");

    gate2Servo.write(GATE_OPEN);
  }
}

// =====================================================
// CLOSE GATE
// =====================================================

void closeGates() {

  gate1Servo.write(GATE_CLOSED);

  gate2Servo.write(GATE_CLOSED);
}

// =====================================================
// DISPENSE MEDICINE
// =====================================================

void startMedicineDispensing(int slot) {
  lastStatusSlot = slot;

  lastStatusMedicine =
    scheduleData[slot - 1].medicine;

  lastStatusScheduledTime =
    scheduleData[slot - 1].time;

  lastStatusTakenTime = "";

  lastMedicineStatus = "DISPENSING";
  medicineActive = true;

  activeSlot = slot;

  medicineStartMillis = millis();
  // ===================================================
  // SEND WHATSAPP REMINDER
  // ===================================================

  sendMedicineEventToBackend(
    activeSlot,
    lastStatusMedicine,
    "DISPENSING",
    lastStatusScheduledTime,
    "");
  int angle =
    getCarouselAngle(slot);

  Serial.println();
  Serial.println("================================");
  Serial.println("      MEDICINE DISPENSING");
  Serial.println("================================");

  Serial.print("Slot: ");
  Serial.println(slot);

  Serial.print("Medicine: ");
  Serial.println(scheduleData[slot - 1].medicine);

  Serial.print("Carousel Angle: ");
  Serial.println(angle);

  // ---------------------------------------------------
  // Move carousel
  // ---------------------------------------------------

  moveCarouselSmooth(angle);

  delay(700);

  // ---------------------------------------------------
  // Alert
  // ---------------------------------------------------

  digitalWrite(LED_PIN, HIGH);

  // Passive buzzer
  tone(BUZZER_PIN, 2000);

  // ---------------------------------------------------
  // Open correct gate
  // ---------------------------------------------------

  openGate(slot);

  Serial.println("Medicine dispensed.");
  Serial.println("Waiting for IR confirmation...");
}

// =====================================================
// STOP ALERT
// =====================================================

void stopAlert() {

  noTone(BUZZER_PIN);

  digitalWrite(
    LED_PIN,
    LOW);
}

// =====================================================
// MEDICINE TAKEN
// =====================================================
void sendMedicineEventToBackend(
  int slot,
  String medicine,
  String status,
  String scheduledTime,
  String takenTime) {

  WiFiClient client;

  // =================================================
  // YOUR LAPTOP IP ADDRESS
  // =================================================

  const char* backendHost =
    "10.196.71.103";

  const int backendPort = 5000;

  // =================================================
  // CONNECT TO NODE.JS SERVER
  // =================================================

  Serial.println();
  Serial.println(
    "Connecting to medicine backend...");

  if (!client.connect(
        backendHost,
        backendPort)) {

    Serial.println(
      "Backend connection FAILED!");

    return;
  }

  Serial.println(
    "Backend connected!");

  // =================================================
  // CREATE JSON
  // =================================================

  String json = "{";

  json += "\"slot\":" + String(slot) + ",";

  json += "\"medicine\":\"" + medicine + "\",";

  json += "\"status\":\"" + status + "\",";

  json += "\"scheduledTime\":\"" + scheduledTime + "\",";

  json += "\"takenTime\":\"" + takenTime + "\"";

  json += "}";

  // =================================================
  // HTTP REQUEST
  // =================================================

  client.println(
    "POST /api/medicine-event HTTP/1.1");

  client.print(
    "Host: ");

  client.println(
    backendHost);

  client.println(
    "Content-Type: application/json");

  client.print(
    "Content-Length: ");

  client.println(
    json.length());

  client.println(
    "Connection: close");

  client.println();

  // =================================================
  // SEND JSON
  // =================================================

  client.println(json);

  Serial.println();
  Serial.println(
    "Medicine event sent:");

  Serial.println(json);

  // =================================================
  // READ SERVER RESPONSE
  // =================================================

  unsigned long timeout =
    millis();

  while (
    client.connected() && millis() - timeout < 3000) {

    while (
      client.available()) {

      String line =
        client.readStringUntil(
          '\n');

      Serial.println(line);

      timeout = millis();
    }
  }

  client.stop();

  Serial.println(
    "Backend connection closed.");
}
void medicineTaken() {

  Serial.println();
  Serial.println("******** MEDICINE TAKEN ********");

  stopAlert();

  closeGates();

  lastMedicineStatus = "TAKEN";

  lastStatusTakenTime =
    String(currentHour < 10 ? "0" : "") + String(currentHour) + ":" + String(currentMinute < 10 ? "0" : "") + String(currentMinute) + ":" + String(currentSecond < 10 ? "0" : "") + String(currentSecond);
  Serial.println("DEBUG: Calling WhatsApp backend...");
  sendMedicineEventToBackend(
    activeSlot,
    lastStatusMedicine,
    "TAKEN",
    lastStatusScheduledTime,
    lastStatusTakenTime);
  medicineActive = false;

  Serial.println("Gate closed.");
  Serial.println("Ready for next medicine.");
}

// =====================================================
// MEDICINE NOT TAKEN
// =====================================================

void medicineNotTaken() {

  Serial.println();
  Serial.println("****** MEDICINE NOT TAKEN ******");

  stopAlert();

  closeGates();
  lastMedicineStatus = "NOT_TAKEN";

  sendMedicineEventToBackend(
    activeSlot,
    lastStatusMedicine,
    "NOT_TAKEN",
    lastStatusScheduledTime,
    "");

  lastStatusTakenTime = "";

  medicineActive = false;

  Serial.println("Gate closed.");
  Serial.println("Ready for next medicine.");
}

// =====================================================
// CHECK IR
// =====================================================

void checkMedicinePickup() {

  if (!medicineActive)
    return;

  // Assuming sensor gives LOW when object detected.
  bool detected =
    digitalRead(IR_PIN) == LOW;

  if (detected) {

    medicineTaken();

    return;
  }

  // ---------------------------------------------------
  // Timeout
  // ---------------------------------------------------

  if (
    millis() - medicineStartMillis
    >= MEDICINE_TIMEOUT) {

    medicineNotTaken();
  }
}

// =====================================================
// TIME MATCH
// =====================================================

void checkSchedule() {

  if (medicineActive)
    return;

  // Process only once per minute
  if (currentMinute == lastProcessedMinute)
    return;

  lastProcessedMinute =
    currentMinute;

  for (int i = 0; i < 8; i++) {

    if (scheduleData[i].time.length() == 0)
      continue;

    String currentTime = "";

    if (currentHour < 10)
      currentTime += "0";

    currentTime +=
      String(currentHour);

    currentTime += ":";

    if (currentMinute < 10)
      currentTime += "0";

    currentTime +=
      String(currentMinute);

    // -------------------------------------------------
    // Match
    // -------------------------------------------------

    if (
      scheduleData[i].time == currentTime) {

      if (!slotTriggered[i]) {

        slotTriggered[i] = true;

        Serial.println();
        Serial.println("================================");
        Serial.println("       MEDICINE TIME MATCH");
        Serial.println("================================");

        Serial.print("Slot: ");
        Serial.println(scheduleData[i].slot);

        Serial.print("Medicine: ");
        Serial.println(scheduleData[i].medicine);

        Serial.print("Scheduled: ");
        Serial.println(scheduleData[i].time);

        Serial.print("Current: ");
        Serial.println(currentTime);

        Serial.println("================================");

        startMedicineDispensing(
          scheduleData[i].slot);
      }
    }
  }
}

// =====================================================
// RESET DAILY TRIGGERS
// =====================================================

void resetDailyTriggers() {

  if (
    currentHour == 0 && currentMinute == 0) {

    for (int i = 0; i < 8; i++) {

      slotTriggered[i] = false;
    }
  }
}

// =====================================================
// LCD
// =====================================================

void updateLCD() {

  lcd.setCursor(0, 0);

  lcd.print("TIME ");

  if (currentHour < 10)
    lcd.print("0");

  lcd.print(currentHour);

  lcd.print(":");

  if (currentMinute < 10)
    lcd.print("0");

  lcd.print(currentMinute);

  lcd.print(":");

  if (currentSecond < 10)
    lcd.print("0");

  lcd.print(currentSecond);

  lcd.print(" ");

  // ---------------------------------------------------

  lcd.setCursor(0, 1);

  lcd.print("DATE ");

  if (currentDay < 10)
    lcd.print("0");

  lcd.print(currentDay);

  lcd.print("/");

  if (currentMonth < 10)
    lcd.print("0");

  lcd.print(currentMonth);

  lcd.print("/");

  lcd.print(currentYear % 100);

  lcd.print(" ");
}

// =====================================================
// TIME TICK
// =====================================================

void updateInternalTime() {

  if (
    millis() - lastTimeTick
    >= 1000) {

    lastTimeTick += 1000;

    currentSecond++;

    if (currentSecond >= 60) {

      currentSecond = 0;

      currentMinute++;

      if (currentMinute >= 60) {

        currentMinute = 0;

        currentHour++;

        if (currentHour >= 24) {

          currentHour = 0;

          currentDay++;
        }
      }
    }
  }
}


// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  // ===================================================
  // PINS
  // ===================================================

  pinMode(
    IR_PIN,
    INPUT);

  pinMode(
    LED_PIN,
    OUTPUT);

  pinMode(
    BUZZER_PIN,
    OUTPUT);

  digitalWrite(
    LED_PIN,
    LOW);

  noTone(BUZZER_PIN);

  // ===================================================
  // I2C
  // ===================================================

  Wire.begin(
    21,
    22);

  // ===================================================
  // LCD
  // ===================================================

  lcd.init();

  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Smart Medicine");

  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // ===================================================
  // SERVOS
  // ===================================================

  carouselServo.attach(
    CAROUSEL_SERVO_PIN);

  gate1Servo.attach(
    GATE1_SERVO_PIN);

  gate2Servo.attach(
    GATE2_SERVO_PIN);

  carouselServo.write(0);

  closeGates();

  delay(1000);

  // ===================================================
  // WIFI
  // ===================================================

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD);

  Serial.println();
  Serial.println("========================================");
  Serial.println(" SMART MEDICINE DISPENSER");
  Serial.println(" FULL INTEGRATION TEST");
  Serial.println("========================================");

  Serial.print("Connecting to WiFi");

  while (
    WiFi.status() != WL_CONNECTED) {

    delay(500);

    Serial.print(".");
  }

  Serial.println();

  Serial.println("WiFi Connected!");

  Serial.print(
    "ESP32 IP Address: ");

  Serial.println(
    WiFi.localIP());

  // ===================================================
  // SERVER ROUTES
  // ===================================================

  server.on(
    "/",
    HTTP_GET,
    handleRoot);

  server.on(
    "/api/time",
    HTTP_POST,
    handleTimeSync);

  server.on(
    "/api/time",
    HTTP_OPTIONS,
    handleOptions);

  server.on(
    "/api/schedule/all",
    HTTP_POST,
    handleCompleteSchedule);

  server.on(
    "/api/schedule/all",
    HTTP_OPTIONS,
    handleOptions);

  server.on(
    "/api/schedule",
    HTTP_POST,
    handleSingleSchedule);

  server.on(
    "/api/schedule",
    HTTP_OPTIONS,
    handleOptions);

  server.on(
    "/api/status",
    HTTP_GET,
    handleStatus);

  server.begin();

  Serial.println(
    "HTTP Server Started");

  Serial.println();
  Serial.println(
    "Waiting for dashboard...");
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  server.handleClient();

  updateInternalTime();

  updateLCD();

  checkSchedule();

  checkMedicinePickup();

  resetDailyTriggers();
}