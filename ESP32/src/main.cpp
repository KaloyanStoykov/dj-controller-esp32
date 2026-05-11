#include <Arduino.h>


// void setup() {
//   pinMode(red, OUTPUT);
//   pinMode(yellow, OUTPUT);
//   pinMode(green, OUTPUT);

// }

// void loop() {
//   // Red ON, yellow OFF, green OFF
//   digitalWrite(red, HIGH);
//   digitalWrite(yellow, LOW);
//   digitalWrite(green, LOW);
//   delay(10000);

//   // Red OFF, yellow ON, green OFF
//   digitalWrite(red, LOW);
//   digitalWrite(yellow, HIGH);
//   digitalWrite(green, LOW);
//   delay(2000);

//   // Red OFF, yellow OFF, green ON
//   digitalWrite(red, LOW);
//   digitalWrite(yellow, LOW);
//   digitalWrite(green, HIGH);
//   delay(5000);

//   digitalWrite(red, LOW);
//   digitalWrite(yellow, HIGH);
//   digitalWrite(green, LOW);
//   delay(2000);
// }

#include <Adafruit_ST7789.h> 
#include <SPI.h>

#define TFT_CS     5
#define TFT_DC     2
#define TFT_RST    4
#define BTN_SELECT 13 // Switch between Top/Bottom
#define BTN_CYCLE  15 // Change the text of the selected one

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

String topMessages[] = {"Sensor Data", "Temp: 24C", "Humidity: 45%", "Voltage: 3.3V"};
String botMessages[] = {"Status: OK", "WiFi: ON", "Battery: 88%", "Uptime: 12m"};

int topIdx = 0;
int botIdx = 0;
int msgCount = 4;

// UI State
bool isTopSelected = true; 
bool lastSelectState = HIGH;
bool lastCycleState = HIGH;

// Helper to determine color based on focus
uint16_t getFocusColor(bool forTop) {
  if (forTop == isTopSelected) return ST77XX_GREEN; // Active color
  return 0x7BEF; // Light Grey for inactive
}

void refreshUI() {
  // Draw Top
  tft.fillRect(0, 0, 240, 119, ST77XX_BLACK); 
  tft.setCursor(10, 60); 
  tft.setTextColor(getFocusColor(true));
  tft.setTextSize(2);
  tft.print(topMessages[topIdx]);

  // Draw Line
  tft.drawFastHLine(0, 120, 240, ST77XX_WHITE);

  // Draw Bottom
  tft.fillRect(0, 121, 240, 119, ST77XX_BLACK); 
  tft.setCursor(10, 180); 
  tft.setTextColor(getFocusColor(false));
  tft.setTextSize(2);
  tft.print(botMessages[botIdx]);
}

void setup() {
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_CYCLE, INPUT_PULLUP);
  
  tft.init(240, 240);
  tft.setRotation(2);
  tft.fillScreen(ST77XX_BLACK);
  
  refreshUI();
}

void loop() {
  bool selectVal = digitalRead(BTN_SELECT);
  bool cycleVal = digitalRead(BTN_CYCLE);

  // --- BUTTON 1: SWITCH FOCUS ---
  if (selectVal == LOW && lastSelectState == HIGH) {
    delay(50);
    if (digitalRead(BTN_SELECT) == LOW) {
      isTopSelected = !isTopSelected; // Toggle focus
      refreshUI();
      while(digitalRead(BTN_SELECT) == LOW) delay(10);
    }
  }
  lastSelectState = selectVal;

  // --- BUTTON 2: CYCLE MESSAGE ---
  if (cycleVal == LOW && lastCycleState == HIGH) {
    delay(50);
    if (digitalRead(BTN_CYCLE) == LOW) {
      if (isTopSelected) {
        topIdx = (topIdx + 1) % msgCount;
      } else {
        botIdx = (botIdx + 1) % msgCount;
      }
      refreshUI();
      while(digitalRead(BTN_CYCLE) == LOW) delay(10);
    }
  }
  lastCycleState = cycleVal;
}

