#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define SD_CS 13
#define STUDENT_FILE "/student8py.csv"
#define ATT_FILE "/attendance8py.csv"

#define RXD2 16
#define TXD2 17

#define BUTTON 14
#define RED 25
#define GREEN 26
#define BLUE 27
#define BUZZER 33

bool addUserMode = false;

int pressCount = 0;
unsigned long firstPressTime = 0;
bool lastButtonState = HIGH;

unsigned long holdStartTime = 0;
bool holding = false;

String lastDate = "--/--/----";
String lastTime = "--:--:--";
int sessionStartHour = -1;

// ---------- LED ----------
void ledGreen(){ digitalWrite(RED,LOW); digitalWrite(GREEN,HIGH); digitalWrite(BLUE,LOW); }
void ledBlue(){ digitalWrite(RED,LOW); digitalWrite(GREEN,LOW); digitalWrite(BLUE,HIGH); }
void ledRed(){ digitalWrite(RED,HIGH); digitalWrite(GREEN,LOW); digitalWrite(BLUE,LOW); }
void ledOff(){ digitalWrite(RED,LOW); digitalWrite(GREEN,LOW); digitalWrite(BLUE,LOW); }

// ---------- BUZZER ----------
void successMelody(){
  tone(BUZZER,1000,150); delay(200);
  tone(BUZZER,1500,150); delay(200);
}
void addUserMelody(){
  tone(BUZZER,1200,150); delay(200);
  tone(BUZZER,1200,150); delay(200);
}
void errorMelody(){
  tone(BUZZER,400,500); delay(600);
}

// ---------- OLED ----------
void drawIdleScreen(){
  display.clearDisplay();
  display.drawRect(0,0,128,64,WHITE);
  display.setCursor(20,5);
  display.println("ATTENDANCE SYS");
  display.setCursor(8,20);
  display.print("Date: "); display.println(lastDate);
  display.setCursor(8,30);
  display.print("Time: "); display.println(lastTime);
  display.setCursor(18,50);
  display.println("Scan your card");
  display.display();
}

void drawMessage(String l1,String l2){
  display.clearDisplay();
  display.drawRect(0,0,128,64,WHITE);
  display.setCursor(10,25); display.println(l1);
  display.setCursor(10,40); display.println(l2);
  display.display();
}

// ---------- FILE ----------
bool userExists(String uid){
  File f = SD.open(STUDENT_FILE);
  if(!f) return false;
  while(f.available()){
    String line=f.readStringUntil('\n');
    if(line.indexOf(uid)>=0){ f.close(); return true; }
  }
  f.close();
  return false;
}

bool alreadyMarked(String uid){
  File f=SD.open(ATT_FILE);
  if(!f) return false;
  while(f.available()){
    String line=f.readStringUntil('\n');
    if(line.indexOf(uid)>=0){ f.close(); return true; }
  }
  f.close();
  return false;
}

void resetAllData(){
  SD.remove(STUDENT_FILE);
  SD.remove(ATT_FILE);

  File f1=SD.open(STUDENT_FILE,FILE_WRITE);
  if(f1){ f1.println("UID"); f1.close(); }

  File f2=SD.open(ATT_FILE,FILE_WRITE);
  if(f2){ f2.println("UID,DATE,TIME"); f2.close(); }

  sessionStartHour=-1;

  drawMessage("SYSTEM RESET","Add users again");
  ledRed(); errorMelody();
  delay(2000);
  ledOff();
  drawIdleScreen();
}

// ---------- SETUP ----------
void setup(){
  Serial2.begin(9600,SERIAL_8N1,RXD2,TXD2);

  pinMode(BUTTON,INPUT_PULLUP);
  pinMode(RED,OUTPUT);
  pinMode(GREEN,OUTPUT);
  pinMode(BLUE,OUTPUT);
  pinMode(BUZZER,OUTPUT);

  Wire.begin(21,22);
  display.begin(SSD1306_SWITCHCAPVCC,0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  SD.begin(SD_CS);

  if(!SD.exists(STUDENT_FILE)){
    File f=SD.open(STUDENT_FILE,FILE_WRITE);
    f.println("UID"); f.close();
  }

  if(!SD.exists(ATT_FILE)){
    File f=SD.open(ATT_FILE,FILE_WRITE);
    f.println("UID,DATE,TIME"); f.close();
  }

  drawIdleScreen();
}

// ---------- LOOP ----------
void loop(){

  // Button logic
  bool currentButtonState=digitalRead(BUTTON);

  if(currentButtonState==LOW && !holding){
    holding=true;
    holdStartTime=millis();
  }

  if(currentButtonState==LOW && holding){
    if(millis()-holdStartTime>=7000){
      resetAllData();
      holding=false;
      pressCount=0;
    }
  }

  if(currentButtonState==HIGH) holding=false;

  if(lastButtonState==HIGH && currentButtonState==LOW){
    if(pressCount==0) firstPressTime=millis();
    pressCount++;
    delay(150);
  }

  lastButtonState=currentButtonState;

  if(pressCount>0 && millis()-firstPressTime>3000)
    pressCount=0;

  if(pressCount==3){
    addUserMode=true;
    pressCount=0;
    drawMessage("ADD USER MODE","Scan new card");
    ledBlue(); addUserMelody();
  }

  // RFID data from Nano
  if(Serial2.available()){
    String data=Serial2.readStringUntil('\n');
    data.trim();

    int c1=data.indexOf(',');
    int c2=data.lastIndexOf(',');

    String uid=data.substring(0,c1);
    lastDate=data.substring(c1+1,c2);
    lastTime=data.substring(c2+1);

    int currentHour=lastTime.substring(0,2).toInt();

    if(sessionStartHour==-1)
      sessionStartHour=currentHour;

    int diff=currentHour-sessionStartHour;
    if(diff<0) diff+=24;

    if(diff>=12){
      SD.remove(ATT_FILE);
      File f=SD.open(ATT_FILE,FILE_WRITE);
      if(f){ f.println("UID,DATE,TIME"); f.close(); }
      sessionStartHour=currentHour;
    }

    // Add User Mode
    if(addUserMode){
      if(userExists(uid)){
        drawMessage("Already Exist",uid);
        ledRed(); errorMelody();
      }else{
        File f=SD.open(STUDENT_FILE,FILE_APPEND);
        if(f){ f.println(uid); f.close(); }
        drawMessage("New User Added",uid);
        ledBlue(); addUserMelody();
      }
      addUserMode=false;
      delay(1500);
      ledOff();
      drawIdleScreen();
      return;
    }

    // Attendance Mode
    if(!userExists(uid)){
      drawMessage("Not Registered","Add user first");
      ledRed(); errorMelody();
      delay(1500);
      ledOff();
      drawIdleScreen();
      return;
    }

    if(alreadyMarked(uid)){
      drawMessage("Already Marked",uid);
      ledRed(); errorMelody();
      delay(1500);
      ledOff();
      drawIdleScreen();
      return;
    }

    File f=SD.open(ATT_FILE,FILE_APPEND);
    if(f){ f.println(data); f.close(); }

    drawMessage("Attendance OK",uid);
    ledGreen(); successMelody();
    delay(1500);
    ledOff();
    drawIdleScreen();
  }
}
