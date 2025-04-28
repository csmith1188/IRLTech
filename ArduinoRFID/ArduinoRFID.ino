// for SPI Communication
// include the SPI library
#include <SPI.h>
#include <Wire.h>
// include the PN532 library
// #include <PN532_SPI.h>
#include <Adafruit_PN532.h>
// include the NfcAdapter library
#include <NfcAdapter.h>
// include the NdefMessage library
#include <NdefMessage.h>
#include <NdefRecord.h>

// PN532_SPI interface(SPI, 10);            // create a PN532 SPI interface with the SPI CS terminal located at digital pin 10
// NfcAdapter nfcAdapter = NfcAdapter(interface);  // create an NFC adapter object
Adafruit_PN532 nfc(10);
String tagId = "None";
NdefMessage message = NdefMessage();            // createa a message object

unsigned int count = message.getRecordCount();  // get the number of records
void setup(void) {
  //serial.write and serial.send
  Serial.begin(115200);
  Serial.println("System initialized");

  // initialize the NFC adapter
  // nfcAdapter.begin();
  SPI.begin();
  nfc.begin();
  nfc.SAMConfig();
  Serial.print("\n");

  // print the commands that can be used
  Serial.print("Here are some commands that can be used:");
  Serial.print("\n");
  Serial.print("To read entire tag:");
  Serial.print("\n");
  Serial.print("read");
  Serial.print("\n");
  Serial.print("\n");
  Serial.print("To read specific records:");
  Serial.print("\n");
  Serial.print("r0/recordtoRead"); // read record 0
  Serial.print("\n");
  Serial.print("r1/recordtoRead"); // read record 1
  Serial.print("\n");
  Serial.print("r2/recordtoRead"); // read record 2
  Serial.print("\n");
  Serial.print("r3/recordtoRead"); // read record 3
  Serial.print("\n");
  Serial.print("\n");
  Serial.print("To write to specific records:");
  Serial.print("\n");
  Serial.print("w0/MessagetoWrite"); // write to record 0
  Serial.print("\n");
  Serial.print("w1/MessagetoWrite"); // write to record 1
  Serial.print("\n");
  Serial.print("w2/MessagetoWrite"); // write to record 2
  Serial.print("\n");
  Serial.print("w3/MessagetoWrite"); // write to record 3
  Serial.print("\n");
  Serial.print("\n");
  Serial.print("To erase tag:");
  Serial.print("\n");
  Serial.print("erase");
  Serial.print("\n");
  Serial.print("\n");

  Serial.print("Place your tag...");
}

void writeRawData(uint8_t page, uint8_t *data) {
    if (nfc.ntag2xx_WritePage(page, data)) {
        Serial.print("Wrote to page ");
        Serial.println(page);
    } else {
        Serial.println("Write failed!");
    }
}

void loop() {
  uint8_t data[4] = { 0xDE, 0xAD, 0xBE, 0xEF };  // Data to write

    // Write to page 4
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLength;

    while (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength)) {
      Serial.println("Tag detected");
      if (nfc.ntag2xx_WritePage(4, data)) {
        Serial.println("Write successful to page 4");
      } else {
        Serial.println("Write failed!");
      }
      // uint8_t buffer[4];
      // if (nfc.ntag2xx_ReadPage(4, buffer)) {
      // Serial.print("Page 4 Data: ");
      // for (int i = 0; i < 4; i++) {
      //     Serial.print(buffer[i], HEX);
      //     Serial.print(" ");
      // }
    // Serial.println();
    }
// }
      

  // while (Serial.available() > 0) {


    // String inputString = Serial.readStringUntil('\n');  // Read the incoming data as a string
    // inputString.trim(); // Remove leading and trailing whitespaces
    // if (inputString.equals("read")) { // read the entire tag
    //   readNFC();
    // }
    // if (inputString[0] == 'r' && inputString[1] == '0') { 
    //   // read record 0
    //   // dedicated to first half of API Key
    //   // formbeta.yorktechapps.com/apikey
    //   readRecord(0);
    // } else if (inputString[0] == 'r' && inputString[1] == '1') { 
    //   // read record 1
    //   // dedicated to second half of API Key
    //   // formbeta.yorktechapps.com/apikey
    //   readRecord(1);
    // } else if (inputString[0] == 'r' && inputString[1] == '2') { 
    //   // read record 2
    //   // no dedicated data
    //   readRecord(2);
    // } else if (inputString[0] == 'r' && inputString[1] == '3') {
    //   // read record 3
    //   // no dedicated data
    //   readRecord(3);
    // } else if (inputString[0] == 'r' && inputString[1] == '4') {
    //   readRecord(4);
    // } else if (inputString[0] == 'w' && inputString[1] == '0' && inputString[2] == '/') { // write to record 0
    //   // find anything after / in the inputString
    //   String writeString = inputString.substring(inputString.indexOf("/") + 1);
    //   writeRecord(0, writeString);
    // } else if (inputString[0] == 'w' && inputString[1] == '1' && inputString[2] == '/') { // write to record 1
    //   // find anything after / in the inputString
    //   String writeString = inputString.substring(inputString.indexOf("/") + 1);
    //   writeRecord(1, writeString);
    // } else if (inputString[0] == 'w' && inputString[1] == '2' && inputString[2] == '/') { // write to record 2
    //   // find anything after / in the inputString
    //   String writeString = inputString.substring(inputString.indexOf("/") + 1);
    //   writeRecord(2, writeString);
    // } else if (inputString[0] == 'w' && inputString[1] == '3' && inputString[2] == '/') { // write to record 3
    //   // find anything after / in the inputString
    //   String writeString = inputString.substring(inputString.indexOf("/") + 1);
    //   writeRecord(3, writeString);
    // } else if (inputString[0] == 'w' && inputString[1] == '4' && inputString[2] == '/') {
    //   String writeString = inputString.substring(inputString.indexOf("/") + 1);
    //   writeRecord(4, writeString);
    // } else if (inputString.equals("erase")) { // erase the tag
    //   nfcAdapter.erase();
    // }
   
  // }
  delay(3000);
}

// a function that will read the entire nfc tag
// void readNFC() {
//   // check if a tag is present
//   if (nfcAdapter.tagPresent()) {
//     // read the tag
//     NfcTag tag = nfcAdapter.read();
//     tag.print();

//     // get the tag id
//     tagId = tag.getUidString();

//     // get the tag
//     NdefMessage message = tag.getNdefMessage();

//     // print the number of records on the tag
//     int recordCount = message.getRecordCount();
//     Serial.print("Number of records on the tag: ");
//     Serial.println(recordCount);
//   }
//   delay(100);
// }

// a function that removes non-numeric characters from a string
String removeNonNumeric(String str) {
  String newStr = "";
  for (char c : str) {
    if (isdigit(c)) newStr += c;
  }
  return newStr;
};

// a function that will read specific records
// void readRecord(int recordNumber) {
//   if (nfcAdapter.tagPresent()) {
//     // read the tag
//     NfcTag tag = nfcAdapter.read();

//     // print the tag type and the UID
//     // Serial.println(tag.getTagType());
//     // Serial.print("UID: ");
//     // Serial.println(tag.getUidString());

//     if (tag.hasNdefMessage())  // every tag won't have a message
//     {
//       // get the NDEF message
//       NdefMessage message = tag.getNdefMessage();

//       // Print the number of records on the tag
//       // Serial.print("\nThis NFC Tag contains an NDEF Message with ");
//       // print the number of records on the tag
//       // Serial.print(message.getRecordCount());
//       // Serial.print(" NDEF Record");
//       // if (message.getRecordCount() != 1) {
//       //   Serial.print("s");
//       // }
//       // Serial.println(".");

//       // cycle through the records, printing some info from each
//       int recordCount = message.getRecordCount();
//       // for (int i = 0; i < recordCount; i++) {
//       // Serial.print("\nNDEF Record ");
//       // Serial.println(recordNumber + 1);
//       NdefRecord record = message.getRecord(recordNumber);
//       // NdefRecord record = message[i]; // alternate syntax
      
//       // Print the TNF, Type, and Payload Length
//       // Serial.print("  TNF: ");
//       // Serial.println(record.getTnf());
//       // Serial.print("  Type: ");
//       // Serial.println(record.getType());  // will be "" for TNF_EMPTY

//       // The TNF and Type should be used to determine how your application processes the payload
//       // There's no generic processing for the payload, it's returned as a byte[]
//       int payloadLength = record.getPayloadLength();
//       byte payload[payloadLength];
//       record.getPayload(payload);

//       // Print the Hex and Printable Characters
//       // Serial.print("  Payload (HEX): ");
//       // PrintHexChar(payload, payloadLength);

//       // Force the data into a String (might work depending on the content)
//       // Real code should use smarter processing
//       String payloadAsString = "";
//       // a for loop that will convert the payload to a string
//       for (int c = 0; c < payloadLength; c++) {
//         // add the payload to the payloadAsString ( will be individual characters)
//         payloadAsString += (char)payload[c];
//       }
//       // print the payload as a string
//       // Serial.print("  Payload: ");
//       Serial.println(payloadAsString);

//       // Split the payload into wins, losses, and draws
//       // int firstCommaIndex = payloadAsString.indexOf(',');
//       // int secondCommaIndex = payloadAsString.indexOf(',', firstCommaIndex + 1);

//       // Extract the wins, losses, and draws strings
//       // String winsString = payloadAsString.substring(0, firstCommaIndex);
//       // winsString = removeNonNumeric(winsString);
//       // String lossesString = payloadAsString.substring(firstCommaIndex + 1, secondCommaIndex);
//       // String drawsString = payloadAsString.substring(secondCommaIndex + 1);

//       // Convert the strings to integers
//       // int wins = winsString.toInt();
//       // int losses = lossesString.toInt();
//       // int draws = drawsString.toInt();
      
//       // // Print the wins, losses, and draws
//       // Serial.print("Wins: ");
//       // Serial.println(wins);
//       // Serial.print("Losses: ");
//       // Serial.println(losses);
//       // Serial.print("Draws: ");
//       // Serial.println(draws);

//       // gets record id
//       String uid = record.getId();
//       // if the record id is empty, set the record id the recordNumber 
//       if (uid == "") {
//         record.setId(recordNumber, 4);
//       } else if (uid != "") {
//         // if the record id is not empty, print the record id
//         Serial.print("  ID: ");
//         Serial.println(uid);
//       }
//       // }
//     }
//   }
//   delay(3000);
// }

// a function that will write to a specific record
// void writeRecord(int recordNumber, String payload) {

//   // check if a tag is present
//   if (nfcAdapter.tagPresent()) {
//     NfcTag tag = nfcAdapter.read();
//     Serial.println(tag.getTagType());
//     Serial.print("UID: ");
//     Serial.println(tag.getUidString());

//     // check if the tag has an NDEF message
//     if (tag.hasNdefMessage())  // every tag won't have a message
//     {
//       // get the NDEF message
//       NdefMessage message = tag.getNdefMessage();

//       // Create a new NDEF message
//       NdefMessage newMessage = NdefMessage();

//       // Add all existing records to the new message up to the one being edited
//       int recordCount = message.getRecordCount();
//       for (int i = 0; i < recordCount; i++) {
//         NdefRecord record = message.getRecord(i);
//         if (i == recordNumber) {
//           // Add your new record
//           newMessage.addTextRecord(payload);
//         } else {
//           // Add the existing record
//           newMessage.addRecord(record);
//         }
//       }

//       // If the record number is greater than the existing record count, add the new record at the end
//       if (recordNumber >= recordCount) {
//         newMessage.addTextRecord(payload);
//       }

//       // Erase the tag
//       nfcAdapter.erase();

//       // Write the new message to the tag
//       boolean success = nfcAdapter.write(newMessage);
//       if (success) {
//         Serial.println("Success. Try reading this tag with your phone.");
//       } else {
//         Serial.println("Write failed");
//       }
//     }
//   }
//   delay(3000);
// }