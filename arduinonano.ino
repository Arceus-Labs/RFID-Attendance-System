#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <RTClib.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN);
SoftwareSerial espSerial(2, 3); // RX, TX (TX = D3 to ESP RX)

RTC_DS3231 rtc;

void setup() {
  SPI.begin();
  rfid.PCD_Init();
  espSerial.begin(9600);

  Wire.begin();
  rtc.begin();

  // 🔥 RUN ONCE then comment
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
  }
  uid.toUpperCase();

  DateTime now = rtc.now();

  String date = String(now.day()) + "/" +
                String(now.month()) + "/" +
                String(now.year());

  String time = String(now.hour()) + ":" +
                String(now.minute()) + ":" +
                String(now.second());

  espSerial.print(uid);
  espSerial.print(",");
  espSerial.print(date);
  espSerial.print(",");
  espSerial.println(time);

  delay(1200);
  rfid.PICC_HaltA();
}
