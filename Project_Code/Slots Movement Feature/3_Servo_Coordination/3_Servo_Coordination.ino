#include <ESP32Servo.h>

// -----------------------------
// Servo Objects
// -----------------------------

Servo gate1Servo;
Servo gate2Servo;
Servo carouselServo;

// -----------------------------
// Pin Configuration
// -----------------------------

const int GATE1_PIN = 18;
const int GATE2_PIN = 19;
const int CAROUSEL_PIN = 21;

// -----------------------------
// Gate Positions
// -----------------------------

const int GATE_CLOSED = 0;
const int GATE_OPEN = 90;

// -----------------------------
// Slot → Carousel Angle
// -----------------------------

const int slotAngle[8] = {
  0,    // Slot 1
  45,   // Slot 2
  90,   // Slot 3
  135,  // Slot 4
  0,    // Slot 5
  45,   // Slot 6
  90,   // Slot 7
  135   // Slot 8
};

// -----------------------------
// Carousel Speed Control
// -----------------------------
//
// Higher value = slower movement
// Lower value = faster movement
//
// 10 = Fast
// 20 = Medium
// 30 = Slow
// 50 = Very Slow
// 80 = Extremely Slow
//

const int CAROUSEL_STEP_DELAY = 30;

// Current carousel position
int currentCarouselAngle = 0;

// -----------------------------
// Select Correct Gate
// -----------------------------

int getGate(int slot) {

  if (slot >= 1 && slot <= 4) {
    return 1;
  }

  if (slot >= 5 && slot <= 8) {
    return 2;
  }

  return 0;
}

// -----------------------------
// Slowly Move Carousel
// -----------------------------

void moveCarouselSlowly(int targetAngle) {

  // Moving forward
  if (targetAngle > currentCarouselAngle) {

    for (int angle = currentCarouselAngle;
         angle <= targetAngle;
         angle++) {

      carouselServo.write(angle);

      delay(CAROUSEL_STEP_DELAY);
    }
  }

  // Moving backward
  else if (targetAngle < currentCarouselAngle) {

    for (int angle = currentCarouselAngle;
         angle >= targetAngle;
         angle--) {

      carouselServo.write(angle);

      delay(CAROUSEL_STEP_DELAY);
    }
  }

  // Update current position
  currentCarouselAngle = targetAngle;
}

// -----------------------------
// Dispense Function
// -----------------------------

void dispenseSlot(int slot) {

  if (slot < 1 || slot > 8) {
    Serial.println("Invalid Slot!");
    return;
  }

  int angle = slotAngle[slot - 1];
  int gate = getGate(slot);

  Serial.println();
  Serial.println("============================");

  Serial.print("Selected Slot: ");
  Serial.println(slot);

  Serial.print("Carousel Angle: ");
  Serial.print(angle);
  Serial.println(" degrees");

  Serial.print("Selected Gate: ");
  Serial.println(gate);

  // -----------------------------
  // Step 1: Move Carousel
  // -----------------------------

  Serial.println("Moving Carousel...");

  moveCarouselSlowly(angle);

  // Wait after reaching position
  delay(1500);

  Serial.println("Carousel Position Reached");

  // -----------------------------
  // Step 2: Open Correct Gate
  // -----------------------------

  if (gate == 1) {

    Serial.println("Gate 1 OPEN");

    gate1Servo.write(GATE_OPEN);

    delay(3000);

    Serial.println("Gate 1 CLOSE");

    gate1Servo.write(GATE_CLOSED);
  }

  else if (gate == 2) {

    Serial.println("Gate 2 OPEN");

    gate2Servo.write(GATE_OPEN);

    delay(3000);

    Serial.println("Gate 2 CLOSE");

    gate2Servo.write(GATE_CLOSED);
  }

  Serial.println("Dispensing Complete");

  delay(2000);
}

// -----------------------------
// Setup
// -----------------------------

void setup() {

  Serial.begin(115200);

  // Servo PWM configuration
  gate1Servo.setPeriodHertz(50);
  gate2Servo.setPeriodHertz(50);
  carouselServo.setPeriodHertz(50);

  // Attach servos
  gate1Servo.attach(GATE1_PIN, 500, 2400);
  gate2Servo.attach(GATE2_PIN, 500, 2400);
  carouselServo.attach(CAROUSEL_PIN, 500, 2400);

  // Initial positions
  gate1Servo.write(GATE_CLOSED);
  gate2Servo.write(GATE_CLOSED);

  carouselServo.write(0);
  currentCarouselAngle = 0;

  Serial.println();
  Serial.println("================================");
  Serial.println("3-SERVO DISPENSING TEST");
  Serial.println("================================");

  Serial.print("Carousel Step Delay: ");
  Serial.print(CAROUSEL_STEP_DELAY);
  Serial.println(" ms");

  delay(2000);
}

// -----------------------------
// Main Test
// -----------------------------

void loop() {

  // Test Slot 1
  dispenseSlot(1);

  // Test Slot 2
  dispenseSlot(2);

  // Test Slot 3
  dispenseSlot(3);

  // Test Slot 4
  dispenseSlot(4);

  // Test Slot 5
  dispenseSlot(5);

  // Test Slot 6
  dispenseSlot(6);

  // Test Slot 7
  dispenseSlot(7);

  // Test Slot 8
  dispenseSlot(8);

  Serial.println();
  Serial.println("================================");
  Serial.println("FULL 8-SLOT CYCLE COMPLETED");
  Serial.println("================================");

  delay(3000);
}