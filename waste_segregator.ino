#include <Servo.h>

/* ===== PIN CONFIG ===== */
#define IR_PIN        2
#define MOISTURE_D0   4
#define MOISTURE_A0   A0
#define SERVO_PIN     9

Servo servo;

/* ===== SERVO ANGLES ===== */
const int SERVO_REST = 85;
const int SERVO_DRY  = 150;
const int SERVO_WET  = 40;
const int SERVO_DELAY = 15;

/* ===== STATE CONTROL ===== */
bool actionDone = false;

void setup() {
  Serial.begin(9600);

  pinMode(IR_PIN, INPUT);
  pinMode(MOISTURE_D0, INPUT);

  servo.attach(SERVO_PIN);
  servo.write(SERVO_REST);
  delay(500);
  servo.detach();

  Serial.println("SMART BIN READY");
}

/* ===== SERVO MOVE FUNCTION ===== */
void moveServo(int target) {

  servo.attach(SERVO_PIN);
  int current = SERVO_REST;

  // Move to target smoothly
  if (current < target) {
    for (int p = current; p <= target; p++) {
      servo.write(p);
      delay(SERVO_DELAY);
    }
  } else {
    for (int p = current; p >= target; p--) {
      servo.write(p);
      delay(SERVO_DELAY);
    }
  }

  delay(700);  // waste drop time

  // Return to rest
  if (target < SERVO_REST) {
    for (int p = target; p <= SERVO_REST; p++) {
      servo.write(p);
      delay(SERVO_DELAY);
    }
  } else {
    for (int p = target; p >= SERVO_REST; p--) {
      servo.write(p);
      delay(SERVO_DELAY);
    }
  }

  delay(200);
  servo.detach();
}

void loop() {

  // ===== IR CONFIRMATION =====
  if (digitalRead(IR_PIN) == LOW && !actionDone) {

    delay(20);  // small stability delay
    if (digitalRead(IR_PIN) == LOW) {

      // ===== MOISTURE CONFIRMATION =====
      delay(20);  // small stability delay
      bool isWet1 = (digitalRead(MOISTURE_D0) == LOW);
      delay(20);
      bool isWet2 = (digitalRead(MOISTURE_D0) == LOW);

      bool isWet = (isWet1 && isWet2);  // confirm wet detection

      Serial.print("OBJECT DETECTED | TYPE: ");
      Serial.println(isWet ? "WET" : "DRY");

      if (isWet) {
        moveServo(SERVO_WET);
      } else {
        moveServo(SERVO_DRY);
      }

      actionDone = true;
    }
  }

  // ===== RESET WHEN OBJECT REMOVED =====
  if (digitalRead(IR_PIN) == HIGH) {
    actionDone = false;
  }
}