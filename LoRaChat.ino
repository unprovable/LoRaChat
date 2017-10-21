// Includes
#include <Wire.h> 
#include <SSD1306.h> // you need to install the ESP8266 oled driver for SSD1306 
                     // by Daniel Eichorn and Fabrice Weinberg to get this file! 
                     // It's in the arduino library manager :-D

#include <SPI.h>
#include <LoRa.h>    // this is the one by Sandeep Mistry, 
                     // (also in the Arduino Library manager :D )

// display descriptor
SSD1306 display(0x3c, 4, 15);

// definitions
//SPI defs for screen
#define SS 18
#define RST 14
#define DI0 26
// #define BAND 429E6

// LoRa Settings 
#define BAND 434500000.00
#define spreadingFactor 9
// #define SignalBandwidth 62.5E3
#define SignalBandwidth 31.25E3
#define codingRateDenominator 8
#define preambleLength 8

// we also need the following config data:
// GPIO5 — SX1278’s SCK
// GPIO19 — SX1278’s MISO
// GPIO27 — SX1278’s MOSI
// GPIO18 — SX1278’s CS
// GPIO14 — SX1278’s RESET
// GPIO26 — SX1278’s IRQ(Interrupt Request)

// misc vars
String msg;
String displayName;
String sendMsg;
char chr;
int i=0;

void setup() {
  // reset the screen
  pinMode(16,OUTPUT);
  digitalWrite(16, LOW); // set GPIO16 low to reset OLED
  delay(50);
  digitalWrite(16, HIGH);
  Serial.begin(115200);
  while (!Serial); //If just the the basic function, must connect to a computer

// Initialising the UI will init the display too.
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(5,5,"LoRa Chat Node");
  display.display();
  
  SPI.begin(5,19,27,18);
  LoRa.setPins(SS,RST,DI0);
  Serial.println("LoRa Chat Node");
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  
  Serial.print("LoRa Spreading Factor: ");
  Serial.println(spreadingFactor);
  LoRa.setSpreadingFactor(spreadingFactor);
  
  Serial.print("LoRa Signal Bandwidth: ");
  Serial.println(SignalBandwidth);
  LoRa.setSignalBandwidth(SignalBandwidth);

  LoRa.setCodingRate4(codingRateDenominator);

  LoRa.setPreambleLength(preambleLength);
  
  Serial.println("LoRa Initial OK!");
  display.drawString(5,20,"LoRaChat is running!");
  display.display();
  delay(2000);
  // get screen name as Nick
  Serial.print("Please enter display name: ");
  while (displayName.length() == 0) {
    displayName = Serial.readString();
  }
  displayName.trim(); // remove the newline
  Serial.print("Nick: "); Serial.println(displayName);
  display.drawString(5, 20, "Nickname set:");
  display.drawString(20, 30, displayName);
  displayName += "> ";
  delay(1000);
}

void loop() {
  // Receive a message first...
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    display.clear();
    display.drawString(3, 0, "Received Message!");
    display.display();
    while (LoRa.available()) {
      String data = LoRa.readString();
      display.drawString(20, 22, data);
      display.display();
      Serial.println(data);
    }
  } // once we're done there, we read bytes from Serial
  if (Serial.available()) {
    chr = Serial.read();
    Serial.print(chr); // so the user can see what they're doing :P
    if (chr == '\n' || chr == '\r') {
      msg += chr; // should maybe terminate my strings properly....
                  // ssshhhhhhh ;)
      Serial.print("Me: ");Serial.println(msg);
      // assemble message
      sendMsg += displayName;
      sendMsg += msg;
      // send message
      LoRa.beginPacket();
      LoRa.print(sendMsg);
      LoRa.endPacket();
      display.clear();
      display.drawString(1, 0, sendMsg);
      display.display();
      // clear the strings and start again :D
      msg = "";
      sendMsg = "";
    }
    else {
      msg += chr;
    }
  }
}

