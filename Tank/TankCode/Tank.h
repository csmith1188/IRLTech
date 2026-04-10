#ifndef TANK_H
#define TANK_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoOTA.h>

// WIFI
extern const char* ssid;
extern const char* password;

// WEB SERVER
extern WebServer server;

// MOTOR SPEED
extern int motorSpeed;

// IR FUNCTIONS
void initIR();
void sendIROn();
void sendIROff();

// MOTOR FUNCTIONS
void initMotors();
void forward();
void backward();
void left();
void right();
void stopMotors();
void setSpeed(int spd);

// WIFI + SERVER
void initWiFi();
void initWebServer();
void handleWebClient();

// OTA
void initOTA();
void handleOTA();

#endif