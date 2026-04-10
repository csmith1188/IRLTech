#include "Tank.h"
#include <IRremoteESP8266.h>
#include <IRsend.h>

const uint16_t irPin = 4;
IRsend irsend(irPin);

void initIR() {
  irsend.begin();
}

// Example Elegoo-style commands
void sendIROn() {
  irsend.sendNEC(0x00FF45BA, 32);  // Example: Power button
}

void sendIROff() {
  irsend.sendNEC(0x00FF15EA, 32);  // Example: another button
}