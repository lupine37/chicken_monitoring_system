#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <DHT.h>
#include "RTClib.h"
#include <Preferences.h>
#include <Keypad.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#define Type DHT11
#define BUZZER 12
#define Threshold 40 /* value for sensitivity */
// Note frequencies in Hertz (for a standard piano)
#define NOTE_C4  262
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_F4  349
#define NOTE_E4  330
#define NOTE_D4  294

// Duration of each note (in milliseconds)
#define DURATION_QUARTER 250
#define DURATION_HALF    500

touch_pad_t touchPin;
const int BLApin = 13;
const int tempPin = 4;
DHT HT(tempPin, Type);
Preferences preferences;
const int firstLine = 17;
const int secondLine = firstLine + 15;
const int thirdLine = secondLine + 15;
const int fourthLine = thirdLine + 15;
U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, 18, 23, 5, 19);
RTC_DS3231 rtc;
bool cursorState = false;
// char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
float tempC;
unsigned long initialTime = 0;
unsigned long DisplayTime = 2000;
unsigned long firstTime = 0;
unsigned long finalTime = 120000;
unsigned long startTime = 0;
unsigned long loopTime = 1000;
unsigned long batteryStart = 11000;
unsigned long batteryTime = 10000;
unsigned long startUpfirst = 0;
unsigned long startUpFinal = 60000;
DateTime now;
DateTime startDate;
struct Date
{
  int day;
  int month;
  int year;
  int hour;
  int minute;
};

bool startUpMode = true;
bool editMode = false;
int yearupg = 0;
int monthupg = 0;
int dayupg = 0;
int hourupg = 0;
int minupg = 0;
int cursorPosition = 0;
int datePosition = 0;
int feedPosition = 0;
int value = 0;

// int redLedPin = 10;
// int greenLedPin = 11;
// int blueLedPin = 12;
int menu = 0;
int addr = 0;
struct feedsStruct {
  int NumOfFeeds[4];
  String feedsType[4];
};
struct chickenStuct {
  String name;
  int NumOfChicks;
  feedsStruct feeds;
};
chickenStuct chicken;
chickenStuct kienyeji = {
  "Kienyeji", 
  0, 
  {
    {0, 0, 0, 0}, 
    {"starter Feeds", "Chick Mash", "Grower Mash", "Kienyeji Mash"}
    }
};
chickenStuct broiler = {
  "Broiler", 
  0, 
  {
    {0, 0, 0},
    {"starter Feeds", "Finisher Mash", "Pellets"}
    }
};
chickenStuct layers = {
  "Layers", 
  0, 
  {
    {0, 0, 0, 0}, 
    {"starter Feeds", "Chick Mash", "Grower Mash", "Layer Mash"}
    }
};
int NumOfChickMash = 0;
int NumOfStarterFeed = 0;
int NumOfFinisherMash = 0;
int NumOfFinisherPellets = 0;
int NumOfGrowerMash = 0;
int NumOfKienyejiMash = 0;
int NumOfLayersMash = 0;
int buzzerTone = 10000;
const int ROW_NUM = 4;
const int COLUMN_NUM = 4;


const int intISR = 32;
const int bit1pin = 27;
const int bit2pin = 26;
const int bit3pin = 25;
const int bit4pin = 33;
volatile boolean newKey = false;
volatile int total = 0;
volatile char key = ' ';
char totVals [] = {'1', '2', '3', 'A', '4', '5', '6', 'B', '7', '8', '9', 'C', '*', '0', '#', 'D'};
boolean keyPressed = false;

const float R1 = 33000;
const float R2 = 120000;
const int batteryPin = 34;
volatile float meanVol = 0.00;
volatile int level = 0;
volatile int batteryLoop = 0;

void TaskFunction(void *pvParameter) {
  while (1)
  {
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
  
}
void IRAM_ATTR readEncoder() {
  total = (digitalRead(bit4pin) << 3) | (digitalRead(bit3pin) << 2) | (digitalRead(bit2pin) << 1) | digitalRead(bit1pin);
  newKey = true;
}

volatile char getNewKey() {
  char Key = ' ';
  if (newKey) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    Key = totVals[total];
  }
  newKey = false;
  return Key;
}





static const unsigned char chicken_icon[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xc0, 0xef, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
   0xfb, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0xff, 0x07,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xd7, 0x07, 0x00, 0x00,
   0x00, 0x00, 0x20, 0x00, 0x00, 0xe0, 0x09, 0x07, 0x00, 0x00, 0x00, 0x00,
   0xf0, 0x01, 0x00, 0xc0, 0xb8, 0x1d, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x03,
   0x00, 0x60, 0x9c, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x30, 0x07, 0x00, 0x60,
   0xc0, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x0c, 0x00, 0x30, 0xe0, 0x07,
   0x00, 0x00, 0x00, 0x00, 0xef, 0x1d, 0x00, 0x30, 0xe0, 0x0f, 0x00, 0x00,
   0x00, 0x00, 0xc6, 0x1f, 0x00, 0x18, 0xe1, 0x07, 0x00, 0x00, 0x00, 0x00,
   0x8c, 0x1f, 0x00, 0x0e, 0xc2, 0x07, 0x00, 0x00, 0x00, 0x00, 0x3e, 0xff,
   0x40, 0x07, 0x20, 0x07, 0x00, 0x00, 0x00, 0x80, 0xfb, 0xcf, 0xff, 0x1f,
   0x40, 0x07, 0x00, 0x00, 0x00, 0x80, 0xe3, 0x07, 0x97, 0xdd, 0xab, 0x06,
   0x00, 0x00, 0x00, 0x00, 0xce, 0xc7, 0x00, 0xf8, 0xf7, 0x07, 0x00, 0x00,
   0x00, 0x00, 0xfe, 0x5d, 0xb0, 0xab, 0xff, 0x0f, 0x00, 0x00, 0x00, 0x00,
   0xff, 0x71, 0xb0, 0x0c, 0x1e, 0x06, 0x00, 0x00, 0x00, 0x80, 0xc3, 0xa7,
   0xa8, 0x3a, 0x7b, 0x0e, 0x00, 0x00, 0x00, 0x00, 0xff, 0xfc, 0x9c, 0x73,
   0xce, 0x07, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xa0, 0xcb, 0xe9, 0x33, 0x07,
   0x00, 0x00, 0x00, 0x00, 0xc0, 0xa8, 0x1f, 0x59, 0xf7, 0x03, 0x00, 0x00,
   0x00, 0x00, 0xc0, 0xdd, 0x7c, 0x7d, 0xff, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x80, 0xa7, 0xf7, 0xef, 0xea, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe3,
   0xeb, 0xcb, 0xef, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf7, 0xf7, 0xae,
   0x7f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x7f, 0xaf, 0x3e, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0xde, 0xbd, 0x5f, 0x0f, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0xf8, 0xbf, 0xfe, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0xf0, 0xff, 0xff, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0,
   0xff, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xff, 0x03,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x03, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x7c, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x40, 0xa8, 0xff, 0xff, 0x8b, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xfc,
   0xff, 0xff, 0x8f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00 };

void wakeup() {
  esp_sleep_wakeup_cause_t wakeup_reason;
  wakeup_reason = esp_sleep_get_wakeup_cause();
  if (wakeup_reason == ESP_SLEEP_WAKEUP_TOUCHPAD)
  {
    startUpMode = false;
  } else {
    u8g2.firstPage();
    do
    {
      u8g2.drawStr(10, 10, "Chicken Monitoring");
      u8g2.drawStr(20, 20, "System");
      u8g2.drawXBMP(24, 25, 80, 40, chicken_icon);
    } while (u8g2.nextPage());
    chickenMelody();
  }
  
}

void callback() {
  
}

int mapFloat(float x, float in_min, float in_max, int out_min, int out_max) {
  if (x <= in_min) {
    return out_min;
  }
  else if (x >= in_max) {
    return out_max;
  }
  else {
    return static_cast<int>((x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
  }
  
}

void displayBatteryLevel() {
  float value = ((float)analogRead(batteryPin) * (3.3 /4095)) * ((R1 + R2) / R2);
  meanVol = meanVol + value;
  batteryLoop++;
  if ((millis() - batteryStart) >= batteryTime) {
    volatile float voltage = 0.00;
    voltage = meanVol / batteryLoop;
    level = mapFloat(voltage, 3.20, 4.20, 0, 100);
    batteryLoop = 0;
    meanVol = 0.0;
    batteryStart = millis();
  }
  
  u8g2.drawFrame(115, 0, 12, 6);
  u8g2.drawLine(128, 4, 128, 2);
  int barWidth = 0;
  if (level == 100) barWidth = 8;
  else if (level >= 75) barWidth = 7;
  else if (level >= 50) barWidth = 4;
  else if (level >= 25) barWidth = 2;
  else if (level >= 5) barWidth = 1;
  else {
    barWidth = 0;
  }
  u8g2.drawBox(117, 2, barWidth, 2);
  delay(200);
}

void setChickenInfo(int chickType) {
  switch (chickType)
  {
  case 1:
    preferences.putBytes("chicken", &kienyeji, sizeof(kienyeji));
    break;
  case 2:
    preferences.putBytes("chicken", &broiler, sizeof(broiler));
    break;
  case 3:
    preferences.putBytes("chicken", &layers, sizeof(layers));
    break;
  default:
    break;
  }
  
}

void editChickens() {
  int chickSelect = 0;
  char key = getNewKey();
  
  if ((key - '0') >= 1 && (key - '0') <= 3)
  {
    chickSelect = key - '0';
    setChickenInfo(chickSelect);
    preferences.getBytes("chicken", &chicken, sizeof(chicken));
    u8g2.firstPage();
    do
    {
      u8g2.drawStr(0, firstLine, "Thank You For choosing");
      u8g2.setCursor(0, secondLine);
      u8g2.print(chicken.name);
      u8g2.drawStr(0, thirdLine, "Press # to Continue");
      u8g2.drawStr(0, fourthLine, "Press * to Retry");
    } while (u8g2.nextPage());
    
    
  }
  else if(key == '#') {
    // getChickFeedsInfo();
    editMode = false;
    startUpMode = false;
    preferences.getBytes("chicken", &chicken, sizeof(chicken));
    preferences.end();
  }
  else if (key == '*')
  {
    editMode = false;
  }
  
  delay(200);
}

void startUp() {
  u8g2.firstPage();
  do
  {
    displayBatteryLevel();
    u8g2.drawStr(0, firstLine, "Press # to edit ");
    u8g2.drawStr(0, secondLine, "Type of Chickens");
  } while (u8g2.nextPage());
  char key = getNewKey();
  if (key == '#')
  {
    editMode = true;
    u8g2.firstPage();
    do
    {
      displayBatteryLevel();
      u8g2.drawStr(0, firstLine, "Press chick's No");
      u8g2.drawStr(0, secondLine, "1. KIENYEJI");
      u8g2.drawStr(0, thirdLine, "2. BROILER");
      u8g2.drawStr(0, fourthLine, "3. LAYERS");
    } while (u8g2.nextPage());
  }
  while(editMode) {
    editChickens();
  }
  if ((millis() - startUpfirst) >= startUpFinal)
  {
    preferences.getBytes("chicken", &chicken, sizeof(chicken));
    preferences.end();
    startUpMode = false;
    startUpfirst = millis();
    firstTime = millis();
  }
}

void playNote(int pin, int note, int duration) {
  tone(pin, note, duration);
  delay(duration);
  noTone(pin);
}

void chickenMelody() {
  playNote(BUZZER, NOTE_C4, DURATION_HALF);
  playNote(BUZZER, NOTE_C4, DURATION_HALF);
  playNote(BUZZER, NOTE_G4, DURATION_HALF);
  playNote(BUZZER, NOTE_G4, DURATION_HALF);
  playNote(BUZZER, NOTE_A4, DURATION_HALF);
  playNote(BUZZER, NOTE_A4, DURATION_HALF);
  playNote(BUZZER, NOTE_G4, DURATION_QUARTER);
}

void setup()
{
  Serial.begin(115200);
  preferences.begin("chickenApp", false);
  pinMode(intISR, INPUT);
  pinMode(bit1pin, INPUT);
  pinMode(bit2pin, INPUT);
  pinMode(bit3pin, INPUT);
  pinMode(bit4pin, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(BLApin, OUTPUT);
  pinMode(batteryPin, INPUT);
  attachInterrupt(intISR, readEncoder, HIGH);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_te);
  digitalWrite(BLApin, HIGH);
  wakeup();
  touchAttachInterrupt(T3, callback, Threshold);
  
  esp_sleep_enable_touchpad_wakeup();
  
  
  pinMode(BUZZER, OUTPUT);
  // pinMode(redLedPin, OUTPUT);
  // pinMode(blueLedPin, OUTPUT);
  // pinMode(greenLedPin, OUTPUT);
  // digitalWrite(redLedPin, LOW);
  // digitalWrite(greenLedPin, LOW);
  // digitalWrite(blueLedPin, LOW);

  HT.begin();
  if (!rtc.begin())
  {
    // Serial.println("Couldn't find RTC");
    // Serial.flush();
    // u8g2.clear();
    u8g2.drawStr(0, secondLine, "Could't find RTC!");
    delay(2000);
    while (1)
      delay(10);
  }
  
  if (rtc.lostPower())
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  now = rtc.now();
  preferences.getBytes("Date", &startDate, sizeof(startDate));
  preferences.getBytes("chicken", &chicken, sizeof(chicken));

  // xTaskCreatePinnedToCore(
  //   TaskFunction, 
  //   "StackOverflowTask", 
  //   10000, 
  //   NULL, 
  //   1, 
  //   NULL, 
  //   0
  // );
  
  initialTime = millis();
  firstTime = millis();
  startTime = millis();
  startUpfirst = millis();
  
  
}
void loop()
{
  if(startUpMode){
    startUp();
  } else {
    now = rtc.now();
    char key = getNewKey();
    if (key == 'A')
    {
      menu = menu + 1;
      keyPressed = true;
    }
    else if (key == 'B')
    {
      menu = menu - 1;
      keyPressed = true;
    }
    switch (menu)
    {
    case 0:
      HomePage();
      break;
    case 1:
      SetNoOfChicks(key);
      break;
    case 2:
      CountFeeds(key);
      break;
    case 3:
      ChangeNowDate(key);
      break;
    case 4:
      ChangeStartTime(key);
      break;
    default:
      menu = 0;
      keyPressed = false;
      break;
    }
    if ((millis() - firstTime) >= finalTime)
    {
      menu = 0;
      firstTime = millis();
      digitalWrite(BLApin, LOW);
      delay(200);
      esp_deep_sleep_start();
    }
  }
  UBaseType_t stackHighWaterMark;
  stackHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
}

void checkTemperature(int Dys)
{
  int highestTemp = 0;
  int lowestTemp = 0;
  tempC = HT.readTemperature();
  if (Dys <= 7)
  {
    highestTemp = 35;
    lowestTemp = 30;
  }
  else if (Dys >= 7 && Dys <= 14)
  {
    highestTemp = 30;
    lowestTemp = 26;
  }
  else if (Dys >= 14 && Dys <= 21)
  {
    highestTemp = 27;
    lowestTemp = 23;
  }
  else
  {
    highestTemp = 25;
    lowestTemp = 18;
  }
  // if(tempC >= highestTemp){
  //     digitalWrite(redLedPin, HIGH);
  //     digitalWrite(blueLedPin, LOW);
  //     digitalWrite(greenLedPin, LOW);
  // }
  // else if(tempC <= lowestTemp){
  //     digitalWrite(blueLedPin, HIGH);
  //     digitalWrite(redLedPin, LOW);
  //     digitalWrite(greenLedPin, LOW);
  // }
  // else{
  //     digitalWrite(greenLedPin, HIGH);
  //     digitalWrite(redLedPin, LOW);
  //     digitalWrite(blueLedPin, LOW);
  // }
}

void displayFeeds(int Dys)
{
  u8g2.setFont(u8g2_font_haxrcorp4089_t_cyrillic);
  u8g2.setCursor(0, fourthLine);
  u8g2.print("FEEDS: ");
  String chickenType = chicken.name;
  if(Dys <= 21) {
    u8g2.print(chicken.feeds.feedsType[0]);
  } else{
      if(chickenType == "Kienyeji" || chickenType == "Layers") {
        if(Dys <= 60) {
          u8g2.print(chicken.feeds.feedsType[1]);
        } else if(Dys <= 150) {
          u8g2.print(chicken.feeds.feedsType[2]);
        } else {
          u8g2.print(chicken.feeds.feedsType[3]);
        }
    } else if(chickenType == "Broiler") {
      u8g2.print(chicken.feeds.feedsType[1]);
      u8g2.print("|");
      u8g2.print(chicken.feeds.feedsType[2]);
      
    }
  }
  u8g2.setFont(u8g2_font_ncenB08_te);
}
void HomePage()
{
  int dayForNow = now.unixtime() / 86400L;
  int daysforStart = startDate.unixtime() / 86400L;
  int timeDiff = dayForNow - daysforStart;
  checkTemperature(timeDiff);
  if ((millis() - startTime) > loopTime)
  {
    u8g2.firstPage();
    do
    {
      displayBatteryLevel();
      u8g2.setCursor(0, firstLine);
      u8g2.print("No of Chicks: ");
      // u8g2.drawXBMP(0, 6, 8, 8, chicken);
      // u8g2.print(" ");
      u8g2.print(chicken.NumOfChicks);
      u8g2.setCursor(0, secondLine);
      u8g2.print("Temperature: ");
      u8g2.print(tempC);
      u8g2.print(" °");
      u8g2.setCursor(0, thirdLine);
      u8g2.print("Chicks Age : ");
      if (timeDiff <= 60) {
        u8g2.print(timeDiff);
        u8g2.print(" Days");
      } else {
        u8g2.print(timeDiff/7);
        u8g2.print(" weeks");
      }
      displayFeeds(timeDiff);
    } while (u8g2.nextPage());
    startTime = millis();
  }
  if ((millis() - initialTime) > DisplayTime)
  {
    if (timeDiff <= 5)
    {
      u8g2.firstPage();
      do
      {
        displayBatteryLevel();
        u8g2.setCursor(3, firstLine);
        u8g2.print("STARTING DOSE");
        u8g2.setCursor(0, secondLine);
        u8g2.print("1. Chick Start");
        u8g2.setCursor(0, thirdLine);
        u8g2.print("2. Glucose");
        u8g2.setCursor(0, fourthLine);
        u8g2.print("3. Liquid Parafin");
        delay(2000);
      } while (u8g2.nextPage());
    }
    else if (timeDiff == 7 || timeDiff == 21)
    {
      u8g2.firstPage();
      do
      {
        tone(BUZZER, buzzerTone);
        u8g2.setCursor(32, secondLine);
        u8g2.print("GUBORU");
        u8g2.setCursor(30, thirdLine);
        u8g2.print("VACCINE !");
        delay(1000);
        noTone(BUZZER);
      } while (u8g2.nextPage());
    }
    else if (timeDiff == 14 || timeDiff == 28)
    {
      u8g2.firstPage();
      do
      {
        tone(BUZZER, buzzerTone);
        u8g2.setCursor(29, secondLine);
        u8g2.print("NEW CASTLE");
        u8g2.setCursor(30, thirdLine);
        u8g2.print("VACCINE !");
        delay(1000);
        noTone(BUZZER);
      } while (u8g2.nextPage());
    }
    else if (timeDiff == 53 || (timeDiff%180) == 0) {
      u8g2.firstPage();
      do
      {
        tone(BUZZER, buzzerTone);
        u8g2.setCursor(32, secondLine);
        u8g2.print("DEWORMER");
        delay(1000);
        noTone(BUZZER);
        } while (u8g2.nextPage());
    }
    else if(timeDiff == 60) {
      u8g2.firstPage();
      do
      {
        tone(BUZZER, buzzerTone);
        u8g2.setCursor(32, secondLine);
        u8g2.print("FOWL POX");
        u8g2.setCursor(30, thirdLine);
        u8g2.print("VACCINE!");
        delay(1000);
        noTone(BUZZER);
      } while (u8g2.nextPage());
    }
    else if(timeDiff == 74) {
      u8g2.firstPage();
      do
      {
        tone(BUZZER, buzzerTone);
        u8g2.setCursor(32, secondLine);
        u8g2.print("TYPHOID");
        u8g2.setCursor(30, thirdLine);
        u8g2.print("VACCINE !");
        delay(1000);
        noTone(BUZZER);
      } while (u8g2.nextPage());
    }
    initialTime = millis();
  }
}

void SetNoOfChicks(char key) 
{
  preferences.begin("chickenApp", false);
  int NumOfChicks = 0;
  u8g2.firstPage();
  do
  {
    displayBatteryLevel();
    u8g2.drawStr(0, firstLine, "PRESS # TO EDIT");
    u8g2.drawStr(0, secondLine, "NO. OF CHICKS");
    u8g2.setCursor(0, thirdLine);
    u8g2.print(chicken.NumOfChicks);
  } while (u8g2.nextPage());
  
  if (key == '#')
  {
    editMode = true;
  }

  while (editMode)
  {
    key = getNewKey();
    static String num = ""; // Static variable to store the entered number.
    if (key >= '0' && key <= '9')
    {
      num += key;                // Concatenate the pressed digit to the existing number
      NumOfChicks = num.toInt(); // Convert the string to an integer
      
    } else if (key == '*'){
      num = "";
      NumOfChicks = 0;
    } else if (key == '#') {
      chicken.NumOfChicks = NumOfChicks;
      preferences.putBytes("chicken", &chicken, sizeof(chicken));
      preferences.end();
      editMode = false;
      firstTime = millis();
    }
  
  u8g2.firstPage();
  do
  {
    displayBatteryLevel();
    u8g2.setCursor(0, firstLine);
    u8g2.print("Change No of Chicks");
    u8g2.setCursor(0, secondLine);
    u8g2.print(NumOfChicks);
  } while (u8g2.nextPage());
  delay(200);
  }
}

void CountFeeds(char key) {
  preferences.begin("chickenApp", false);
  u8g2.setFont(u8g2_font_ncenR08_te);
  int displayedFeeds[] = {26, 36, 46, 56};
  int numOfFeeds = sizeof(chicken.feeds.feedsType) / sizeof(chicken.feeds.feedsType[0]);
  if (key == '#') {
    value = 0;
    editMode = true;
    feedPosition = 0;
    cursorPosition = 0;
  }
  u8g2.firstPage();
  do
  {
    displayBatteryLevel();
    u8g2.drawStr(0, 16, "PRESS # TO EDIT");
    
    for (int i = 0; i < numOfFeeds; i++)
    {
      if (chicken.feeds.feedsType[i] != "")
      {
        u8g2.setCursor(0, displayedFeeds[i]);
        u8g2.print("No of ");
        u8g2.print(chicken.feeds.feedsType[i]);
        u8g2.print(": ");
        u8g2.print(chicken.feeds.NumOfFeeds[i]);
      }
    }
    
  } while (u8g2.nextPage());
  
  while (editMode) {
    u8g2.firstPage();
    do
    {
      displayBatteryLevel();
      u8g2.drawStr(0, 16, "PRESS # TO SAVE");
      for (int i = 0; i < numOfFeeds; i++)
      {
        if (chicken.feeds.feedsType[i] != "")
        {
          u8g2.setCursor(0, displayedFeeds[i]);
          u8g2.print("No of ");
          u8g2.print(chicken.feeds.feedsType[i]);
          u8g2.print(": ");
          if(feedPosition == i) {
            printCursor(chicken.feeds.NumOfFeeds[feedPosition]);
          } else{
            u8g2.print(chicken.feeds.NumOfFeeds[i]);
          }
        }
      }
    } while (u8g2.nextPage());
    key = getNewKey();
    if (key >= '0' && key <= '9') {
      value = value * 10 + (key - '0');
      chicken.feeds.NumOfFeeds[feedPosition]= value;
      cursorPosition++;
    }
    
    if (key == 'C') {
      value = 0;
      feedPosition--;
      cursorPosition = 0;
    } else if (key == 'D') {
      value = 0;
      feedPosition++;
      cursorPosition = 0;
    } 
    if (key == '#') {
      
      preferences.putBytes("chicken", &chicken, sizeof(chicken));
      preferences.end();
      editMode = false;
      firstTime = millis();
    }
    if (feedPosition < 0 || feedPosition > numOfFeeds) {
      feedPosition = 0;
    }
    if (cursorPosition < 0) {
      cursorPosition = 0;
    }
    delay(200);

  }
  u8g2.setFont(u8g2_font_ncenB08_te);
}

void ChangeStartTime(char key)
{
  yearupg = startDate.year();
  monthupg = startDate.month();
  dayupg = startDate.day();
  hourupg = startDate.hour();
  minupg = startDate.minute();
  preferences.begin("chickenApp", false);
  editMode = false;
  if (key == '#')
  {
    editMode = true;
    value = 0;
    cursorPosition = 0;
    datePosition = 0;
  }
  while (editMode == true)
  {
    u8g2.firstPage(); // Start drawing on the display
    do
    {
      displayBatteryLevel();
      u8g2.drawStr(0, firstLine, "PRESS # TO SAVE");
      u8g2.drawStr(3, secondLine, "THE ARRIVAL DATE");
      u8g2.setCursor(0, thirdLine); // Set the cursor position on the display
      u8g2.print("Edit Date: ");
      if (datePosition == 0)
      {
        printCursor(dayupg); // Display the day with cursor
      }
      else
      {
        u8g2.print(dayupg);
      }
      u8g2.print("-");
      if (datePosition == 1)
      {
        printCursor(monthupg); // Display the month with cursor
      }
      else
      {
        u8g2.print(monthupg);
      }
      u8g2.print("-");
      if (datePosition == 2)
      {
        printCursor(yearupg); // Display the year with cursor
      }
      else
      {
        u8g2.print(yearupg);
      }
      u8g2.setCursor(0, fourthLine);
      u8g2.print("Edit Time: ");
      if (datePosition == 3)
      {
        printCursor(hourupg); // Display the hour with cursor
      }
      else
      {
        u8g2.print(hourupg);
      }
      u8g2.print(":");
      if (datePosition == 4)
      {
        printCursor(minupg); // Display the minute with cursor
      }
      else
      {
        u8g2.print(minupg);
      }
      u8g2.print(":");
      u8g2.print(0);
    } while (u8g2.nextPage()); // Continue drawing on the next page of the display
    key = getNewKey();
    updateDate(key);
    if (key == '#')
    {
      startDate = DateTime(yearupg, monthupg, dayupg, hourupg, minupg, 0);
      preferences.putBytes("Date", &startDate, sizeof(startDate));
      preferences.end();
      editMode = false;
      firstTime = millis();
    }

    delay(200); // Delay to control the loop speed
  }
  if ((millis() - startTime) > loopTime)
  {
    u8g2.firstPage();
    do
    {
      displayBatteryLevel();
      u8g2.drawStr(0, firstLine, "PRESS '#' TO EDIT");
      u8g2.drawStr(0, secondLine, "ARRIVAL DATE");
      u8g2.setCursor(0, thirdLine);
      u8g2.print("Date: ");
      u8g2.print(dayupg);
      u8g2.print("-");
      u8g2.print(monthupg);
      u8g2.print("-");
      u8g2.print(yearupg);
    } while (u8g2.nextPage());
    startTime = millis();
  }
}

void ChangeNowDate(char key)
{
  yearupg = now.year();
  monthupg = now.month();
  dayupg = now.month();
  hourupg = now.hour();
  minupg = now.minute();
  editMode = false;
  if (key == '#')
  {
    editMode = true;
    value = 0;
    cursorPosition = 0;
    datePosition = 0;
  }
  while (editMode == true)
  {
    u8g2.firstPage(); // Start drawing on the display
    do
    {
      displayBatteryLevel();
      u8g2.drawStr(0, firstLine, "PRESS # TO SAVE");
      u8g2.setCursor(0, secondLine); // Set the cursor position on the display
      u8g2.print("Edit Date: ");
      if (datePosition == 0)
      {
        printCursor(dayupg); // Display the day with cursor
      }
      else
      {
        u8g2.print(dayupg);
      }
      u8g2.print("-");
      if (datePosition == 1)
      {
        printCursor(monthupg); // Display the month with cursor
      }
      else
      {
        u8g2.print(monthupg);
      }
      u8g2.print("-");
      if (datePosition == 2)
      {
        printCursor(yearupg); // Display the year with cursor
      }
      else
      {
        u8g2.print(yearupg);
      }
      u8g2.setCursor(0, thirdLine);
      u8g2.print("Edit Time: ");
      if (datePosition == 3)
      {
        printCursor(hourupg); // Display the hour with cursor
      }
      else
      {
        u8g2.print(hourupg);
      }
      u8g2.print(":");
      if (datePosition == 4)
      {
        printCursor(minupg); // Display the minute with cursor
      }
      else
      {
        u8g2.print(minupg);
      }
      u8g2.print(":");
      u8g2.print(0);
    } while (u8g2.nextPage()); // Continue drawing on the next page of the display
    key = getNewKey();
    updateDate(key);
    if (key == '#')
    {
      rtc.adjust(DateTime(yearupg, monthupg, dayupg, hourupg, minupg, 0));
      firstTime = millis();
      editMode = false;
    }

    delay(200); // Delay to control the loop speed
  }
  if ((millis() - startTime) > loopTime)
  {
    u8g2.firstPage();
    do
    {
      displayBatteryLevel();
      u8g2.drawStr(0, firstLine, "PRESS # TO EDIT");
      u8g2.drawStr(0, secondLine, "CURRENT DATE");
      u8g2.setCursor(0, thirdLine);
      u8g2.print("Date: ");
      u8g2.print(now.day());
      u8g2.print("-");
      u8g2.print(now.month());
      u8g2.print("-");
      u8g2.print(now.year());
      u8g2.setCursor(0, fourthLine);
      u8g2.print("Time: ");
      u8g2.print(now.hour());
      u8g2.print(":");
      u8g2.print(now.minute());
      u8g2.print(":");
      u8g2.print(0);
    } while (u8g2.nextPage());
    startTime = millis();
  }
}

int getDigitCount(int number)
{
  int count = 0;
  do
  {
    count++;
    number /= 10;
  } while (number != 0);
  return count;
}

// // Function to display the cursor on the digit being edited and the rest of the digit of the date number
void printCursor(int number)
{
  int count = getDigitCount(number);
  for (int i = 0; i < count; i++)
  {
    if (i == cursorPosition)
    {
      if (cursorState == true) {
        u8g2.print('_'); // Display underscore for the current digit being edited
      } else {
        u8g2.print((number / int(pow(10, count - 1 - i))) % 10);
      }
    }
    else
    {
      u8g2.print((number / int(pow(10, count - 1 - i))) % 10); // Display the digit
    }
  }
  if ((millis() - startTime) > 500) {
    cursorState = !cursorState;
    startTime = millis();
  }
}

void updateDate(char key)
{
  int dateCount = 0;
  switch (datePosition)
  {
  case 0: // Day
    if (key >= '0' && key <= '9')
    {
      value = value * 10 + (key - '0');
      dayupg = constrain(value, 1, 31); // Update the day value
      cursorPosition++;
    }

    // if the position of the cursor is greater that the number of days redirect to month
    if (cursorPosition >= 2)
    {
      datePosition++;
      cursorPosition = 0;
      value = 0;
    }
    // if the position of the cursor is less that number of days redirect to year
    else if (cursorPosition < 0)
    {
      datePosition = 2;
      cursorPosition = 0;
      value = 0;
    }
    break;
  case 1: // Month
    if (key >= '0' && key <= '9')
    {
      value = value * 10 + (key - '0');
      monthupg = constrain(value, 1, 12);
      cursorPosition++;
    }
    if (cursorPosition >= 2)
    {
      datePosition++;
      cursorPosition = 0;
      value = 0;
    }
    else if (cursorPosition < 0)
    {
      datePosition--;
      cursorPosition = 0;
      value = 0;
    }

    break;
  case 2: // Year
    if (key >= '0' && key <= '9')
    {
      // edit the digit where the cursor is positioned accordance to the key pressed
      editDigit(yearupg, key, cursorPosition);
      cursorPosition++;
    }
    if (cursorPosition >= 4)
    {
      datePosition = 0;
      cursorPosition = 0;
      value = 0;
    }
    else if (cursorPosition < 0)
    {
      datePosition--;
      cursorPosition = 0;
      value = 0;
    }
    break;

  case 3: // Hour
    if (key >= '0' && key <= '9')
    {
      value = value * 10 + (key - '0');
      hourupg = constrain(value, 0, 23);
      cursorPosition++;
    }

    if (cursorPosition >= 2)
    {
      datePosition++;
      cursorPosition = 0;
      value = 0;
    }
    else if (cursorPosition < 0)
    {
      datePosition = 4;
      cursorPosition = 0;
      value = 0;
    }
    break;

  case 4: // Minute
    if (key >= '0' && key <= '9')
    {
      value = value * 10 + (key - '0');
      minupg = constrain(value, 0, 59);
      cursorPosition++;
    }

    if (cursorPosition >= 2)
    {
      datePosition = 3;
      cursorPosition = 0;
      value = 0;
    }
    else if (cursorPosition < 0)
    {
      datePosition--;
      cursorPosition = 0;
      value = 0;
    }
    break;
  }
  // navigate the cursor to the date Section
  if (key == 'C')
  {
    value = 0;
    datePosition = 0;
    cursorPosition = 0;
  }
  // navigate the cursor to the time section
  else if (key == 'D')
  {
    value = 0;
    datePosition = 3;
    cursorPosition = 0;
  }
  // navigate the cursor to the next date/time being edited
  else if (key == 'A')
  {
    value = 0;
    datePosition++;
    cursorPosition = 0;
  }
  // navigate the cursor to the previous date/time being edited
  else if (key == 'B')
  {
    value = 0;
    datePosition--;
    cursorPosition = 0;
  }

  if (datePosition < 0)
  {
    datePosition = 0;
    cursorPosition = 0;
    value = 0;
  }
  else if (datePosition > 4)
  {
    datePosition = 0;
    cursorPosition = 0;
    value = 0;
  }
}

void editDigit(int &number, char digit, int position)
{
  // calculate the position of the digit being updated
  int power = int(pow(10, (getDigitCount(number) - 1 - position)));
  if (digit == '0' && cursorPosition == 0)
  {
    ;
  }
  else
  {
    // update the digit of the number
    number = number - (number / power % 10) * power + (digit - '0') * power;
  }
}
