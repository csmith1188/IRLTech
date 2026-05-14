#include "Tank.h"

void setup() {

  Serial.begin(115200);

  initTurret();   // FIRST (important)
  initMotors();

  initIR();
  initIRReceiver();
  initWiFi();
  initWebServer();
  initOTA();

  Serial.println("Tank Ready");
  Serial.println("IR ready");
}

void loop() {

  handleOTA();
  handleWebClient();
  handleIRReceiver();
}