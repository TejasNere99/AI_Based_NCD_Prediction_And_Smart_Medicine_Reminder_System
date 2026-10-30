#include <ESP32Servo.h>

// =====================================================
// SERVO OBJECTS
// =====================================================

Servo gate1Servo;
Servo gate2Servo;
Servo carouselServo;

// =====================================================
// PIN CONFIGURATION
// =====================================================

const int GATE1_PIN = 18;
const int GATE2_PIN = 19;
const int CAROUSEL_PIN = 23;

const int IR_PIN = 34;
const int LED_PIN = 26;

// =====================================================
// SERVO ANGLES
// =====================================================

const int GATE_CLOSED = 0;
const int GATE_OPEN = 90;

// =====================================================
// SMOOTH MOVEMENT SPEED
// =====================================================

const int GATE_STEP_DELAY = 20;
const int CAROUSEL_STEP_DELAY = 15;

// =====================================================
// TIMING
// =====================================================

const unsigned long HAND_TIMEOUT = 50000;  // 50 seconds
const unsigned long GATE_OPEN_TIME = 5000; // 5 seconds

// =====================================================
// IR SENSOR LOGIC
// =====================================================

// LOW = hand/object detected
// HIGH = no detection

const int IR_DETECTED = LOW;

// =====================================================
// SLOT ANGLES: TOTAL 6 SLOTS
// =====================================================

const int slotAngle[6] = {
  0,    // S1 -> Gate 1
  60,   // S2 -> Gate 1
  120,  // S3 -> Gate 1
  0,    // S4 -> Gate 2
  60,   // S5 -> Gate 2
  120   // S6 -> Gate 2
};

// =====================================================
// CURRENT SERVO POSITIONS
// =====================================================

int currentCarouselAngle = 0;
int gate1Angle = GATE_CLOSED;
int gate2Angle = GATE_CLOSED;

// =====================================================
// GET GATE FOR SLOT
// =====================================================

int getGate(int slot) {

  if (slot >= 1 && slot <= 3) {
    return 1;
  }

  if (slot >= 4 && slot <= 6) {
    return 2;
  }

  return 0;
}

// =====================================================
// SMOOTH GATE MOVEMENT
// =====================================================

void moveGateSmooth(
  Servo &gateServo,
  int &currentAngle,
  int targetAngle
) {

  if (targetAngle > currentAngle) {

    for (int angle = currentAngle;
         angle <= targetAngle;
         angle++) {

      gateServo.write(angle);
      delay(GATE_STEP_DELAY);
    }
  }

  else if (targetAngle < currentAngle) {

    for (int angle = currentAngle;
         angle >= targetAngle;
         angle--) {

      gateServo.write(angle);
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

    for (int angle = currentCarouselAngle;
         angle <= targetAngle;
         angle++) {

      carouselServo.write(angle);
      delay(CAROUSEL_STEP_DELAY);
    }
  }

  else if (targetAngle < currentCarouselAngle) {

    for (int angle = currentCarouselAngle;
         angle >= targetAngle;
         angle--) {

      carouselServo.write(angle);
      delay(CAROUSEL_STEP_DELAY);
    }
  }

  currentCarouselAngle = targetAngle;
}

// =====================================================
// DISPENSE ONE SLOT
// =====================================================

void dispenseSlot(int slot) {

  Serial.println();
  Serial.println("========================================");

  // Validate slot

  if (slot < 1 || slot > 6) {
    Serial.println("ERROR: Invalid slot!");
    return;
  }

  // Get slot angle and gate

  int angle = slotAngle[slot - 1];
  int gate = getGate(slot);

  Serial.print("Selected Slot: S");
  Serial.println(slot);

  Serial.print("Carousel Target: ");
  Serial.print(angle);
  Serial.println(" degrees");

  Serial.print("Selected Gate: ");
  Serial.println(gate);

  // STEP 1: CLOSE BOTH GATES

  Serial.println("Closing both gates...");

  moveGateSmooth(
    gate1Servo, gate1Angle, GATE_CLOSED
  );

  moveGateSmooth(
    gate2Servo, gate2Angle, GATE_CLOSED
  );

  delay(300);

  // STEP 2: MOVE CAROUSEL

  Serial.println("Moving carousel...");

  moveCarouselSmooth(angle);

  Serial.println("Carousel position reached.");

  // STEP 3: WAIT FOR HAND DETECTION

  Serial.println("Waiting for hand detection...");
  Serial.println("Gate remains CLOSED.");

  digitalWrite(LED_PIN, HIGH);

  unsigned long startTime = millis();
  bool handDetected = false;

  while (millis() - startTime < HAND_TIMEOUT) {

    if (digitalRead(IR_PIN) == IR_DETECTED) {

      handDetected = true;

      Serial.println("HAND DETECTED!");
      break;
    }

    delay(50);
  }

  // STEP 4: TIMEOUT - KEEP GATE CLOSED

  if (!handDetected) {

    digitalWrite(LED_PIN, LOW);

    Serial.println("HAND NOT DETECTED.");
    Serial.println("Gate remains CLOSED.");
    Serial.println("Moving to next slot...");

    delay(1000);
    return;
  }

  // STEP 5: OPEN ONLY THE SELECTED GATE

  if (gate == 1) {

    Serial.println("Gate 1 OPENING...");

    moveGateSmooth(
      gate1Servo, gate1Angle, GATE_OPEN
    );
  }

  else if (gate == 2) {

    Serial.println("Gate 2 OPENING...");

    moveGateSmooth(
      gate2Servo, gate2Angle, GATE_OPEN
    );
  }

  // STEP 6: KEEP GATE OPEN FOR 5 SECONDS

  Serial.println("Gate OPEN.");
  Serial.println("Waiting 5 seconds...");

  delay(GATE_OPEN_TIME);

  // STEP 7: CLOSE SELECTED GATE

  if (gate == 1) {

    Serial.println("Gate 1 CLOSING...");

    moveGateSmooth(
      gate1Servo, gate1Angle, GATE_CLOSED
    );
  }

  else if (gate == 2) {

    Serial.println("Gate 2 CLOSING...");

    moveGateSmooth(
      gate2Servo, gate2Angle, GATE_CLOSED
    );
  }

  digitalWrite(LED_PIN, LOW);

  Serial.println("Gate CLOSED.");
  Serial.println("LED OFF.");

  // STEP 8: NEXT SLOT AFTER GATE CLOSES

  delay(1000);

  Serial.println("Dispensing sequence completed.");
  Serial.println("Ready for next slot.");
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);

  // Configure servos

  gate1Servo.setPeriodHertz(50);
  gate2Servo.setPeriodHertz(50);
  carouselServo.setPeriodHertz(50);

  gate1Servo.attach(GATE1_PIN, 500, 2400);
  gate2Servo.attach(GATE2_PIN, 500, 2400);
  carouselServo.attach(CAROUSEL_PIN, 500, 2400);

  // Initial positions

  gate1Servo.write(GATE_CLOSED);
  gate2Servo.write(GATE_CLOSED);
  carouselServo.write(0);

  currentCarouselAngle = 0;
  gate1Angle = GATE_CLOSED;
  gate2Angle = GATE_CLOSED;

  delay(2000);

  Serial.println();
  Serial.println("========================================");
  Serial.println("SMART MEDICINE DISPENSER");
  Serial.println("3 SERVOS + IR HAND DETECTION + LED");
  Serial.println("========================================");

  Serial.println("Gate 1: Slots S1-S3");
  Serial.println("Gate 2: Slots S4-S6");
  Serial.println("System Ready.");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop() {

  // FIRST HALF: GATE 1

  dispenseSlot(1);
  dispenseSlot(2);
  dispenseSlot(3);

  // SECOND HALF: GATE 2

  dispenseSlot(4);
  dispenseSlot(5);
  dispenseSlot(6);

  // COMPLETE CYCLE

  Serial.println();
  Serial.println("========================================");
  Serial.println("FULL 6-SLOT CYCLE COMPLETED");
  Serial.println("========================================");

  delay(3000);
}
