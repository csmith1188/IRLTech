#include "Tank.h"

#define ENA 25
#define IN1 26
#define IN2 27

#define ENB 33
#define IN3 32
#define IN4 23

int motorSpeed = 200;

void initMotors(){

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  
  pinMode(IN4, OUTPUT);

  // NEW ESP32 LEDC API
  ledcAttach(ENA, 5000, 8);
  ledcAttach(ENB, 5000, 8);

  stopMotors();
}

void setSpeed(int spd){
  ledcWrite(ENA, spd);
  ledcWrite(ENB, spd);
}

void forward(){

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

  setSpeed(motorSpeed);

}

void backward(){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);

  setSpeed(motorSpeed);

}

void left(){

  digitalWrite(IN1,LOW);
  digitalWrite(IN2,HIGH);

  digitalWrite(IN3,LOW);
  digitalWrite(IN4,HIGH);

  setSpeed(motorSpeed);

}

void right(){

  digitalWrite(IN1,HIGH);
  digitalWrite(IN2,LOW);

  digitalWrite(IN3,HIGH);
  digitalWrite(IN4,LOW);

  setSpeed(motorSpeed);

}

void stopMotors(){

  ledcWrite(ENA,0);
  ledcWrite(ENB,0);

}