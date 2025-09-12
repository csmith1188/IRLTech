#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>
#include <Adafruit_PN532.h>

// ---------------- LCD ----------------
const uint8_t LCD_ADDR = 0x27;              // change to 0x3F if needed
LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

// ---------------- Keypad ----------------
const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
// Wiring: rows on D4..D7, cols on A3..A0
byte rowPins[ROWS] = {4, 5, 6, 7};
byte colPins[COLS] = {A3, A2, A1, A0};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- Servo ----------------
Servo myServo;
const int SERVO_PIN = 9;
const unsigned long DOWN_MS = 1000;
const unsigned long UP_MS   = 1000;

// ---------------- Limit Switch ----------------
const int SWITCH_PIN = 8;                   // OUT -> D8
const int LED_PIN    = LED_BUILTIN;
bool ACTIVE_LOW = true;
const unsigned long DEBOUNCE_MS = 5;
int lastReading = HIGH, stableState = HIGH;
unsigned long lastChange = 0;

// ---------------- PN532 (I2C) ----------------
#define PN532_IRQ   2
#define PN532_RESET 3
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);
const uint16_t PN532_TIMEOUT_MS = 10;       // keep loop snappy

// --------------- Helpers ----------------
void showHome() {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Tap card...");
  lcd.setCursor(0,1); lcd.print("Keys: ");
}

void printUIDToLCD(const uint8_t *uid, uint8_t uidLen) {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print("Card detected");
  lcd.setCursor(0,1);
  for (uint8_t i = 0; i < uidLen; i++) {
    if (i) lcd.print(' ');
    if (uid[i] < 16) lcd.print('0');
    lcd.print(uid[i], HEX);
  }
}

void setup() {
  Serial.begin(9600);
  delay(200);
  Wire.begin();

  // LCD
  lcd.init(); lcd.backlight();

  // Limit switch
  pinMode(SWITCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Keypad
  keypad.setDebounceTime(15);

  // PN532
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println(F("PN532 not found (check I2C mode/wiring/IRQ/RST)."));
    lcd.clear(); lcd.setCursor(0,0); lcd.print("PN532 NOT FOUND");
    lcd.setCursor(0,1); lcd.print("Check wiring");
    while (1) { delay(1000); }
  }
  Serial.print(F("PN532 FW: "));
  Serial.print((versiondata >> 16) & 0xFF); Serial.print('.');
  Serial.println((versiondata >> 8) & 0xFF);
  nfc.SAMConfig(); // ISO14443A

  showHome();
}

void loop() {
  // ---------- Limit switch (debounced), Serial + LED only ----------
  int reading = digitalRead(SWITCH_PIN);
  if (reading != lastReading) {
    lastChange = millis();
    lastReading = reading;
  }
  if (millis() - lastChange > DEBOUNCE_MS && reading != stableState) {
    stableState = reading;
    bool pressed = ACTIVE_LOW ? (stableState == LOW) : (stableState == HIGH);
    digitalWrite(LED_PIN, pressed);
    Serial.println(pressed ? "P" : "R");
  }

  // ---------- Keypad handling (EVENT API) ----------
  static int lcdCol = 6; // after "Keys: "
  if (keypad.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++) {
      if (!keypad.key[i].stateChanged) continue;
      char k = keypad.key[i].kchar;
      byte s = keypad.key[i].kstate;

      if (s == PRESSED) {
        Serial.print("KEY: "); Serial.println(k);

        if (k == '*') {
          // Clear typing area
          lcd.setCursor(6, 1);
          lcd.print("          "); // up to end
          lcdCol = 6;
          lcd.setCursor(lcdCol, 1);
        } else if (k == 'A') {
          lcd.clear(); lcd.setCursor(0,0); lcd.print("going down");
          myServo.attach(SERVO_PIN);
          myServo.write(164);                 // adjust for your servo
          delay(DOWN_MS);
          myServo.detach();
          lcd.setCursor(0,1); lcd.print("done");
          delay(400);
          showHome(); lcdCol = 6;
        } else if (k == 'B') {
          lcd.clear(); lcd.setCursor(0,0); lcd.print("going up");
          myServo.attach(SERVO_PIN);
          myServo.write(10);                  // adjust for your servo
          delay(UP_MS);
          myServo.detach();
          lcd.setCursor(0,1); lcd.print("done");
          delay(400);
          showHome(); lcdCol = 6;
        } else {
          // Print pressed key to LCD
          lcd.setCursor(lcdCol, 1);
          lcd.print(k);
          lcdCol++;
          if (lcdCol >= 16) {                 // wrap & clear typing area
            lcd.setCursor(6,1); lcd.print("          ");
            lcdCol = 6;
            lcd.setCursor(lcdCol,1);
          }
        }
      }
    }
  }

  // ---------- PN532 read with SHORT TIMEOUT ----------
  uint8_t uid[7]; uint8_t uidLength = 0;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, PN532_TIMEOUT_MS)) {
    Serial.print(F("UID: "));
    for (uint8_t i = 0; i < uidLength; i++) {
      if (uid[i] < 16) Serial.print('0');
      Serial.print(uid[i], HEX);
      Serial.print(' ');
    }
    Serial.println();

    printUIDToLCD(uid, uidLength);
    delay(900);
    showHome();
  }

  // keep loop light
  delay(1);
}
