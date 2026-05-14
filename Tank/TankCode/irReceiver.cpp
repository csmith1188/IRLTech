#include "Tank.h"
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define IR_RECEIVE_PIN 19

IRrecv irrecv(IR_RECEIVE_PIN);
decode_results results;

void initIRReceiver() {
  Serial.println("Starting IR Receiver...");
  irrecv.enableIRIn();
}

void handleIRReceiver() {

  if (irrecv.decode(&results)) {

    uint32_t value = results.value;

    // Ignore noise / repeat codes
    if (value == 0xFF45BA) {

      Serial.println("HIT");
      right();
    }

    irrecv.resume();
  }
}