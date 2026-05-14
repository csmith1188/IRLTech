#include "Tank.h"
#include <ESP32Servo.h>

#define TURRET_PIN 18  // change if needed

Servo turretServo;


int turretAngle = 97;  // center position


void initTurret() {
  Serial.println("Initializing turret...");

  ESP32PWM::allocateTimer(3);  // reserve a separate timer

  turretServo.setPeriodHertz(50);
  turretServo.attach(18, 500, 2400);

  turretServo.write(turretAngle);

  Serial.println("Turret initialized");
}

void setTurretAngle(int angle) {
  turretAngle = constrain(angle, 0, 187);
  turretServo.write(turretAngle);
}

void turretLeft() {
  turretAngle += 30;
  Serial.println(turretAngle);
  if (turretAngle > 187) turretAngle = 187;
  turretServo.write(turretAngle);
}

void turretRight() {
  if (turretAngle == 97) turretAngle -= 7;
  turretAngle -= 30;
  if (turretAngle < 0) turretAngle = 0;
  turretServo.write(turretAngle);
}

void turretCenter() {
  turretAngle = 97;
  turretServo.write(turretAngle);
}