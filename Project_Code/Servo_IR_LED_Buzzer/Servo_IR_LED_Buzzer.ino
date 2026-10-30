// #include <ESP32Servo.h>

// // =====================================================
// // SERVO OBJECTS
// // =====================================================

// Servo gate1Servo;
// Servo gate2Servo;
// Servo carouselServo;

// // =====================================================
// // PIN CONFIGURATION
// // =====================================================

// const int GATE1_PIN = 18;
// const int GATE2_PIN = 19;
// const int CAROUSEL_PIN = 23;

// const int IR_PIN = 34;
// const int LED_PIN = 26;
// const int BUZZER_PIN = 25;

// // =====================================================
// // SERVO POSITIONS
// // =====================================================

// const int GATE_CLOSED = 0;
// const int GATE_OPEN = 90;

// // =====================================================
// // MEDICINE WAIT TIME
// // =====================================================

// const unsigned long MEDICINE_TIMEOUT = 50000;

// // =====================================================
// // IR SENSOR LOGIC
// // =====================================================

// const int IR_DETECTED = LOW;

// // =====================================================
// // BUZZER
// // =====================================================

// const int BUZZER_FREQUENCY = 2000;

// // =====================================================
// // SLOT → CAROUSEL ANGLE
// // =====================================================

// const int slotAngle[8] = {
//   0,
//   45,
//   90,
//   135,
//   0,
//   45,
//   90,
//   135
// };

// // =====================================================
// // BUZZER FUNCTIONS
// // =====================================================

// void buzzerOn() {

//   ledcAttach(BUZZER_PIN, 2000, 8);
//   ledcWriteTone(BUZZER_PIN, 2000);

//   Serial.println("BUZZER ON");
// }


// void buzzerOff() {

//   ledcWriteTone(BUZZER_PIN, 0);
//   delay(10);

//   ledcDetach(BUZZER_PIN);

//   pinMode(BUZZER_PIN, OUTPUT);
//   digitalWrite(BUZZER_PIN, LOW);

//   Serial.println("BUZZER OFF");
// }

// // =====================================================
// // GET GATE
// // =====================================================

// int getGate(int slot) {

//   if (slot >= 1 && slot <= 4) {
//     return 1;
//   }

//   if (slot >= 5 && slot <= 8) {
//     return 2;
//   }

//   return 0;
// }

// // =====================================================
// // DISPENSE SLOT
// // =====================================================

// void dispenseSlot(int slot) {

//   Serial.println();
//   Serial.println("========================================");

//   int angle = slotAngle[slot - 1];
//   int gate = getGate(slot);

//   Serial.print("Selected Slot: ");
//   Serial.println(slot);

//   Serial.print("Carousel Angle: ");
//   Serial.println(angle);

//   Serial.print("Gate: ");
//   Serial.println(gate);

//   // ---------------------------------------------------
//   // Close both gates
//   // ---------------------------------------------------

//   gate1Servo.write(GATE_CLOSED);
//   gate2Servo.write(GATE_CLOSED);

//   delay(500);

//   // ---------------------------------------------------
//   // Move carousel
//   // ---------------------------------------------------

//   Serial.println("Moving carousel...");

//   carouselServo.write(angle);

//   delay(1500);

//   Serial.println("Carousel position reached.");

//   // ---------------------------------------------------
//   // Open correct gate
//   // ---------------------------------------------------

//   if (gate == 1) {

//     Serial.println("Gate 1 OPEN");

//     gate1Servo.write(GATE_OPEN);

//   }

//   else if (gate == 2) {

//     Serial.println("Gate 2 OPEN");

//     gate2Servo.write(GATE_OPEN);

//   }

//   // ---------------------------------------------------
//   // LED + BUZZER ON
//   // ---------------------------------------------------

//   digitalWrite(LED_PIN, HIGH);

//   buzzerOn();

//   Serial.println("LED ON");
//   Serial.println("BUZZER ON");
//   Serial.println("Waiting for medicine pickup...");

//   // ---------------------------------------------------
//   // Start timer
//   // ---------------------------------------------------

//   unsigned long startTime = millis();

//   bool medicineTaken = false;

//   // ---------------------------------------------------
//   // Wait for IR OR 50 seconds
//   // ---------------------------------------------------

//   while (millis() - startTime < MEDICINE_TIMEOUT) {

//     int irState = digitalRead(IR_PIN);

//     if (irState == IR_DETECTED) {

//       medicineTaken = true;

//       Serial.println();
//       Serial.println("*** MEDICINE TAKEN ***");

//       break;
//     }

//     delay(50);
//   }

//   // ---------------------------------------------------
//   // ALERT OFF
//   // ---------------------------------------------------

//   buzzerOff();

//   digitalWrite(LED_PIN, LOW);

//   Serial.println("BUZZER OFF");
//   Serial.println("LED OFF");

//   // ---------------------------------------------------
//   // RESULT
//   // ---------------------------------------------------

//   if (medicineTaken) {

//     Serial.println("STATUS: MEDICINE TAKEN");

//   }

//   else {

//     Serial.println("*** MEDICINE NOT TAKEN ***");
//     Serial.println("50 second timeout reached.");

//   }

//   // ---------------------------------------------------
//   // CLOSE CORRECT GATE
//   // ---------------------------------------------------

//   if (gate == 1) {

//     Serial.println("Gate 1 CLOSE");

//     gate1Servo.write(GATE_CLOSED);

//   }

//   else if (gate == 2) {

//     Serial.println("Gate 2 CLOSE");

//     gate2Servo.write(GATE_CLOSED);

//   }

//   delay(1000);

//   Serial.println("Moving to next slot...");
// }

// // =====================================================
// // SETUP
// // =====================================================

// void setup() {

//   Serial.begin(115200);

//   // ---------------------------------------------------
//   // GPIO
//   // ---------------------------------------------------

//   pinMode(IR_PIN, INPUT);

//   pinMode(LED_PIN, OUTPUT);

//   digitalWrite(LED_PIN, LOW);

//   // ---------------------------------------------------
//   // BUZZER INITIALIZATION
//   // ---------------------------------------------------

//   ledcAttach(BUZZER_PIN, 2000, 8);

//   buzzerOff();

//   // ---------------------------------------------------
//   // SERVO INITIALIZATION
//   // ---------------------------------------------------

//   gate1Servo.setPeriodHertz(50);
//   gate2Servo.setPeriodHertz(50);
//   carouselServo.setPeriodHertz(50);

//   gate1Servo.attach(GATE1_PIN, 500, 2400);

//   gate2Servo.attach(GATE2_PIN, 500, 2400);

//   carouselServo.attach(CAROUSEL_PIN, 500, 2400);

//   // ---------------------------------------------------
//   // INITIAL POSITIONS
//   // ---------------------------------------------------

//   gate1Servo.write(GATE_CLOSED);

//   gate2Servo.write(GATE_CLOSED);

//   carouselServo.write(0);

//   delay(2000);

//   Serial.println();
//   Serial.println("========================================");
//   Serial.println(" SMART MEDICINE DISPENSER");
//   Serial.println(" TEST 5");
//   Serial.println(" 3 SERVO + IR + LED + BUZZER");
//   Serial.println("========================================");

//   Serial.println("System Ready.");
// }

// // =====================================================
// // LOOP
// // =====================================================

// void loop() {

//   dispenseSlot(1);

//   dispenseSlot(2);

//   dispenseSlot(3);

//   dispenseSlot(4);

//   dispenseSlot(5);

//   dispenseSlot(6);

//   dispenseSlot(7);

//   dispenseSlot(8);

//   Serial.println();
//   Serial.println("========================================");
//   Serial.println(" FULL 8-SLOT TEST COMPLETED");
//   Serial.println("========================================");

//   delay(3000);
// }



#include <ESP32Servo.h>

// =====================================================
// SERVO OBJECTS
// =====================================================

Servo gate1Servo;
Servo gate2Servo;
Servo carouselServo;

// =====================================================
// PINS
// =====================================================

const int GATE1_PIN = 18;
const int GATE2_PIN = 19;
const int CAROUSEL_PIN = 23;

const int IR_PIN = 34;
const int LED_PIN = 26;
const int BUZZER_PIN = 25;

// =====================================================
// ANGLES
// =====================================================

const int GATE_CLOSED = 0;
const int GATE_OPEN = 90;

const int GATE_STEP_DELAY = 20;
const int CAROUSEL_STEP_DELAY = 15;

// =====================================================
// TIMING
// =====================================================

const unsigned long ALERT_TIME = 10000; // 10 seconds
const unsigned long GATE_OPEN_TIME = 5000; // 5 seconds

// =====================================================
// IR SENSOR
// =====================================================

// LOW = hand detected on common IR modules
const int IR_DETECTED = LOW;

// =====================================================
// CAROUSEL POSITIONS
// Each gate has 3 logical slots
// =====================================================

const int slotAngle[3] = {
  0,    // S1
  60,   // S2
  120   // S3
};

// =====================================================
// CURRENT POSITIONS
// =====================================================

int currentCarouselAngle = 0;
int gate1Angle = GATE_CLOSED;
int gate2Angle = GATE_CLOSED;

// =====================================================
// SMOOTH GATE MOVEMENT
// =====================================================

void moveGateSmooth(
  Servo &gateServo,
  int &currentAngle,
  int targetAngle
) {
  if (targetAngle > currentAngle) {
    for (int a = currentAngle; a <= targetAngle; a++) {
      gateServo.write(a);
      delay(GATE_STEP_DELAY);
    }
  }
  else if (targetAngle < currentAngle) {
    for (int a = currentAngle; a >= targetAngle; a--) {
      gateServo.write(a);
      delay(GATE_STEP_DELAY);
    }
  }

  currentAngle = targetAngle;
}

// =====================================================
// SMOOTH CAROUSEL MOVEMENT
// =====================================================

void moveCarouselSmooth(int targetAngle) {
  targetAngle = constrain(targetAngle, 0, 180);

  if (targetAngle > currentCarouselAngle) {
    for (int a = currentCarouselAngle; a <= targetAngle; a++) {
      carouselServo.write(a);
      delay(CAROUSEL_STEP_DELAY);
    }
  }
  else if (targetAngle < currentCarouselAngle) {
    for (int a = currentCarouselAngle; a >= targetAngle; a--) {
      carouselServo.write(a);
      delay(CAROUSEL_STEP_DELAY);
    }
  }

  currentCarouselAngle = targetAngle;
}

// =====================================================
// ALERT CONTROL
// =====================================================

void startAlert() {
  digitalWrite(LED_PIN, HIGH);

  // Passive buzzer tone
  tone(BUZZER_PIN, 2000);

  Serial.println("LED ON + BUZZER ON");
}

void stopAlert() {
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  Serial.println("LED OFF + BUZZER OFF");
}

// =====================================================
// PROCESS SELECTED SLOT
// =====================================================

void dispenseSlot(int gateNumber, int slotNumber) {

  if (gateNumber < 1 || gateNumber > 2 ||
      slotNumber < 1 || slotNumber > 3) {
    Serial.println("Invalid command or slot!");
    return;
  }

  int targetAngle = slotAngle[slotNumber - 1];

  Serial.println();
  Serial.println("================================");

  Serial.print("Gate: ");
  Serial.println(gateNumber);

  Serial.print("Slot: S");
  Serial.println(slotNumber);

  Serial.print("Carousel target: ");
  Serial.println(targetAngle);

  // STEP 1: Ensure both gates are closed

  moveGateSmooth(gate1Servo, gate1Angle, GATE_CLOSED);
  moveGateSmooth(gate2Servo, gate2Angle, GATE_CLOSED);

  // STEP 2: Move carousel

  Serial.println("Moving carousel...");
  moveCarouselSmooth(targetAngle);

  Serial.println("Carousel position reached.");

  // STEP 3: LED + buzzer alert

  Serial.println("Starting 10-second alert.");
  Serial.println("Place hand near IR sensor.");

  startAlert();

  unsigned long startTime = millis();
  bool handDetected = false;

  // Keep alert active for 10 seconds.
  // Remember if hand is detected at any point.

  while (millis() - startTime < ALERT_TIME) {

    if (digitalRead(IR_PIN) == IR_DETECTED) {
      handDetected = true;
    }

    delay(20);
  }

  stopAlert();

  // STEP 4: Open gate only if hand was detected

  if (!handDetected) {
    Serial.println("No hand detected during alert.");
    Serial.println("Gate remains CLOSED.");
    Serial.println("Enter next command when ready.");
    return;
  }

  Serial.println("HAND DETECTED!");

  if (gateNumber == 1) {
    Serial.println("Gate 1 opening...");
    moveGateSmooth(gate1Servo, gate1Angle, GATE_OPEN);
  }
  else {
    Serial.println("Gate 2 opening...");
    moveGateSmooth(gate2Servo, gate2Angle, GATE_OPEN);
  }

  Serial.println("Gate open for 5 seconds.");
  delay(GATE_OPEN_TIME);

  // STEP 5: Close selected gate

  if (gateNumber == 1) {
    Serial.println("Gate 1 closing...");
    moveGateSmooth(gate1Servo, gate1Angle, GATE_CLOSED);
  }
  else {
    Serial.println("Gate 2 closing...");
    moveGateSmooth(gate2Servo, gate2Angle, GATE_CLOSED);
  }

  Serial.println("Gate CLOSED.");
  Serial.println("Test completed.");
  Serial.println("Enter next command when ready.");
}

// =====================================================
// SETUP
// =====================================================

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  gate1Servo.setPeriodHertz(50);
  gate2Servo.setPeriodHertz(50);
  carouselServo.setPeriodHertz(50);

  gate1Servo.attach(GATE1_PIN, 500, 2400);
  gate2Servo.attach(GATE2_PIN, 500, 2400);
  carouselServo.attach(CAROUSEL_PIN, 500, 2400);

  gate1Servo.write(GATE_CLOSED);
  gate2Servo.write(GATE_CLOSED);
  carouselServo.write(0);

  currentCarouselAngle = 0;
  gate1Angle = GATE_CLOSED;
  gate2Angle = GATE_CLOSED;

  delay(2000);

  Serial.println();
  Serial.println("SMART MEDICINE DISPENSER TEST");
  Serial.println("Enter one command at a time:");
  Serial.println("G1:S1 | G1:S2 | G1:S3");
  Serial.println("G2:S1 | G2:S2 | G2:S3");
  Serial.println("LED + buzzer: 10 seconds");
  Serial.println("Hand detected -> gate opens");
  Serial.println("No hand -> gate stays closed");
  Serial.println("System Ready.");
}

// =====================================================
// MAIN LOOP - MANUAL COMMANDS ONLY
// =====================================================

void loop() {
  if (Serial.available() > 0) {

    String command = Serial.readStringUntil('\n');
    command.trim();
    command.toUpperCase();

    if (command.length() != 5 ||
        command.charAt(0) != 'G' ||
        command.charAt(2) != ':' ||
        command.charAt(3) != 'S') {

      Serial.println("Invalid command. Example: G1:S1");
      return;
    }

    int gateNumber = command.charAt(1) - '0';
    int slotNumber = command.charAt(4) - '0';

    dispenseSlot(gateNumber, slotNumber);
  }
}

