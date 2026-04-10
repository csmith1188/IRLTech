#include "Tank.h"

void setup() {

  Serial.begin(115200);

  initMotors();
  initIR();
  initWiFi();
  initWebServer();
  initOTA();

  Serial.println("Tank Ready");

}

void loop() {

  handleOTA();
  handleWebClient();

}