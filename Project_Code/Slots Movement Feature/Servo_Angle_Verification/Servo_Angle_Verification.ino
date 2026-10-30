#include <ESP32Servo.h>

Servo carouselServo;

const int SERVO_PIN = 18;

// ================================
// 8-Slot Carousel Angle Mapping
// ================================
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

// ================================
// Speed Control
// ================================

// Increase this value = slower movement
// Decrease this value = faster movement
const int STEP_DELAY = 30;  // milliseconds

// Current servo position
int currentAngle = 0;


// ==========================================
// Move Servo Slowly to Selected Slot
// ==========================================
void moveToSlot(int slot) {

  if (slot < 1 || slot > 8) {
    Serial.println("Invalid Slot!");
    return;
  }

  int targetAngle = slotAngle[slot - 1];

  Serial.println("----------------------------");

  Serial.print("Selected Slot: ");
  Serial.println(slot);

  Serial.print("Target Angle: ");
  Serial.print(targetAngle);
  Serial.println(" degrees");

  Serial.print("Moving from: ");
  Serial.print(currentAngle);
  Serial.println(" degrees");


  // ======================================
  // Move Forward
  // ======================================
  if (targetAngle > currentAngle) {

    for (int angle = currentAngle;
         angle <= targetAngle;
         angle++) {

      carouselServo.write(angle);

      delay(STEP_DELAY);
    }
  }


  // ======================================
  // Move Backward
  // ======================================
  else if (targetAngle < currentAngle) {

    for (int angle = currentAngle;
         angle >= targetAngle;
         angle--) {

      carouselServo.write(angle);

      delay(STEP_DELAY);
    }
  }


  // Update current position
  currentAngle = targetAngle;

  Serial.println("Movement completed!");

  // Wait at slot
  delay(2000);
}


// ==========================================
// SETUP
// ==========================================
void setup() {

  Serial.begin(115200);

  // Servo frequency
  carouselServo.setPeriodHertz(50);

  // Attach servo
  carouselServo.attach(
    SERVO_PIN,
    500,
    2400
  );

  Serial.println("============================");
  Serial.println("8-SLOT CAROUSEL");
  Serial.println("Slow & Smooth Servo Test");
  Serial.println("============================");

  // Start position
  carouselServo.write(currentAngle);

  delay(1000);
}


// ==========================================
// LOOP
// ==========================================
void loop() {

  // Slot 1
  moveToSlot(1);

  // Slot 2
  moveToSlot(2);

  // Slot 3
  moveToSlot(3);

  // Slot 4
  moveToSlot(4);

  // Slot 5
  moveToSlot(5);

  // Slot 6
  moveToSlot(6);

  // Slot 7
  moveToSlot(7);

  // Slot 8
  moveToSlot(8);


  Serial.println("============================");
  Serial.println("One complete cycle finished");
  Serial.println("============================");

  delay(2000);
}