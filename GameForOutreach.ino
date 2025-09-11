#include <Adafruit_NeoPixel.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <SPI.h>

// ----- TFT Setup -----
#define TFT_CS   10
#define TFT_DC    9
#define TFT_RST   2
#define light_value 100
Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240

// ----- Pins -----
#define LED_PIN1     7
#define LED_PIN2     6
#define LED_PIN3     5
#define LED_PIN4     4
#define SENSOR_PIN1  A0
#define SENSOR_PIN2  A1
#define SENSOR_PIN3  A2
#define SENSOR_PIN4  A3

// ----- Game Config -----
#define NUM_PIXELS     45
#define THRESHOLD1      650
#define THRESHOLD2      550
#define THRESHOLD3      550
#define THRESHOLD4      615
#define LOCKOUT_TIME   1000

Adafruit_NeoPixel ring1(NUM_PIXELS, LED_PIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring2(NUM_PIXELS, LED_PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring3(NUM_PIXELS, LED_PIN3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ring4(NUM_PIXELS, LED_PIN4, NEO_GRB + NEO_KHZ800);

bool hit1 = false, hit2 = false, hit3 = false, hit4 = false;
bool greenHold1 = false, greenHold2 = false, greenHold3 = false, greenHold4 = false;
bool smile1 = false, smile2 = false, smile3 = false, smile4 = false;
unsigned long smileStart1 = 0, smileStart2 = 0, smileStart3 = 0, smileStart4 = 0;
unsigned long greenStart1 = 0, greenStart2 = 0, greenStart3 = 0, greenStart4 = 0;
unsigned long lastTrigger1 = 0, lastTrigger2 = 0, lastTrigger3 = 0, lastTrigger4 = 0;

int score = 0;

void setup() {
  Serial.begin(9600);
  display.begin();
  display.setRotation(1);
  display.fillScreen(ILI9341_BLACK);
  drawEvilSmiley();
  drawScore();

  ring1.begin(); ring2.begin(); ring3.begin(); ring4.begin();
  ring1.setBrightness(light_value); ring2.setBrightness(light_value); ring3.setBrightness(light_value); ring4.setBrightness(light_value);
  ring1.show(); ring2.show(); ring3.show(); ring4.show();

  setAll(ring1, 255, 0, 0);
  setAll(ring2, 255, 0, 0);
  setAll(ring3, 255, 0, 0);
  setAll(ring4, 255, 0, 0);
}

void loop() {
  int val1 = analogRead(SENSOR_PIN1);
  int val2 = analogRead(SENSOR_PIN2);
  int val3 = analogRead(SENSOR_PIN3);
  int val4 = analogRead(SENSOR_PIN4);

  if (!hit1 && millis() - lastTrigger1 > LOCKOUT_TIME && val1 < THRESHOLD1) {
    trigger(ring1, smile1, hit1, greenHold1, smileStart1, greenStart1, lastTrigger1);
    score += NUM_PIXELS; drawScore(); showOuch();
  }
  if (!hit2 && millis() - lastTrigger2 > LOCKOUT_TIME && val2 < THRESHOLD2) {
    trigger(ring2, smile2, hit2, greenHold2, smileStart2, greenStart2, lastTrigger2);
    score += NUM_PIXELS; drawScore(); showOuch();
  }
  if (!hit3 && millis() - lastTrigger3 > LOCKOUT_TIME && val3 < THRESHOLD3) {
    trigger(ring3, smile3, hit3, greenHold3, smileStart3, greenStart3, lastTrigger3);
    score += NUM_PIXELS; drawScore(); showOuch();
  }
  if (!hit4 && millis() - lastTrigger4 > LOCKOUT_TIME && val4 < THRESHOLD4) {
    trigger(ring4, smile4, hit4, greenHold4, smileStart4, greenStart4, lastTrigger4);
    score += NUM_PIXELS; drawScore(); showOuch();
  }

  updateReset(ring1, smile1, greenHold1, greenStart1, smileStart1, hit1);
  updateReset(ring2, smile2, greenHold2, greenStart2, smileStart2, hit2);
  updateReset(ring3, smile3, greenHold3, greenStart3, smileStart3, hit3);
  updateReset(ring4, smile4, greenHold4, greenStart4, smileStart4, hit4);

  if (hit1 && hit2 && hit3 && hit4) {
    flashAllGreenTwice();
    score = 0;
    drawScore();
    resetGame();
  }

    Serial.print("T1, T2, T3, and T4 values:  ");
    Serial.print(val1);
    Serial.print("---");
    Serial.print(val2);
    Serial.print("---");
    Serial.print(val3);
    Serial.print("---");
    Serial.println(val4);

  delay(20);
}

void trigger(Adafruit_NeoPixel &ring, bool &smile, bool &hit, bool &hold, unsigned long &sTime, unsigned long &gTime, unsigned long &lTime) {
  blink(ring, 5);
  chase(ring);
  blink(ring, 5);
  showSmile(ring);
  sTime = millis();
  hit = true; hold = false; smile = true;
  lTime = millis();
}

void updateReset(Adafruit_NeoPixel &ring, bool &smile, bool &hold, unsigned long &gStart, unsigned long &sStart, bool &hit) {
  if (smile && millis() - sStart >= 2000 && !hold) {
    setAll(ring, 0, 255, 0);
    gStart = millis(); hold = true; smile = false;
  }
}

void flashAllGreenTwice() {
  for (int i = 0; i < 2; i++) {
    setAll(ring1, 0, 255, 0); setAll(ring2, 0, 255, 0);
    setAll(ring3, 0, 255, 0); setAll(ring4, 0, 255, 0);
    delay(500);
    setAll(ring1, 0, 0, 0); setAll(ring2, 0, 0, 0);
    setAll(ring3, 0, 0, 0); setAll(ring4, 0, 0, 0);
    delay(500);
  }
}

void resetGame() {
  hit1 = hit2 = hit3 = hit4 = false;
  greenHold1 = greenHold2 = greenHold3 = greenHold4 = false;
  smile1 = smile2 = smile3 = smile4 = false;
  setAll(ring1, 255, 0, 0);
  setAll(ring2, 255, 0, 0);
  setAll(ring3, 255, 0, 0);
  setAll(ring4, 255, 0, 0);
  drawEvilSmiley();
}

void setAll(Adafruit_NeoPixel &ring, uint8_t r, uint8_t g, uint8_t b) {
  for (int i = 0; i < ring.numPixels(); i++) ring.setPixelColor(i, ring.Color(r, g, b));
  ring.show();
}

void blink(Adafruit_NeoPixel &ring, int times) {
  for (int i = 0; i < times; i++) {
    setAll(ring, 0, 255, 0); 
    setAll(ring, 0, 0, 0); 
  }
}

void chase(Adafruit_NeoPixel &ring) {
  for (int i = 0; i < ring.numPixels(); i++) {
    ring.clear();
    ring.setPixelColor(i, ring.Color(0, 255, 0));
    ring.show();
    delay(40);
  }
}

void showSmile(Adafruit_NeoPixel &ring) {
  ring.clear();
  ring.setPixelColor(5, ring.Color(0, 255, 0));
  ring.setPixelColor(39, ring.Color(0, 255, 0));
  for (int i = 15; i <= 29; i++) ring.setPixelColor(i, ring.Color(0, 255, 0));
  ring.show();
}

void drawScore() {
  display.fillRect(0, 0, 130, 30, ILI9341_BLACK);
  display.setCursor(5, 5);
  display.setTextColor(ILI9341_YELLOW);
  display.setTextSize(2);
  display.print("Score: ");
  display.print(score);
}

void showOuch() {
  display.fillScreen(ILI9341_BLACK);
  drawScore();
  display.setTextColor(ILI9341_WHITE);
  display.setTextSize(4);
  int x = (SCREEN_WIDTH - 6 * 24) / 2;
  int y = (SCREEN_HEIGHT / 2) - 20;
  display.setCursor(x, y);
  display.println("OUCH!");
  delay(1500);
  drawEvilSmiley();
  drawScore();
}

void drawEvilSmiley() {
  display.fillScreen(ILI9341_BLACK);
  int cx = SCREEN_WIDTH / 2;
  int cy = SCREEN_HEIGHT / 2;
  int r = 80;
  int spike = 25;

  display.drawCircle(cx, cy, r, ILI9341_WHITE);
  display.drawLine(cx, cy - r, cx, cy - r - spike, ILI9341_WHITE);
  display.drawLine(cx, cy + r, cx, cy + r + spike, ILI9341_WHITE);
  display.drawLine(cx - r, cy, cx - r - spike, cy, ILI9341_WHITE);
  display.drawLine(cx + r, cy, cx + r + spike, cy, ILI9341_WHITE);
  display.drawLine(cx - 55, cy - 55, cx - 55 - spike, cy - 55 - spike, ILI9341_WHITE);
  display.drawLine(cx + 55, cy - 55, cx + 55 + spike, cy - 55 - spike, ILI9341_WHITE);
  display.drawLine(cx - 55, cy + 55, cx - 55 - spike, cy + 55 + spike, ILI9341_WHITE);
  display.drawLine(cx + 55, cy + 55, cx + 55 + spike, cy + 55 + spike, ILI9341_WHITE);
  display.fillCircle(cx - 30, cy - 25, 10, ILI9341_WHITE);
  display.fillCircle(cx + 30, cy - 25, 10, ILI9341_WHITE);
  display.drawLine(cx - 45, cy - 45, cx - 15, cy - 35, ILI9341_WHITE);
  display.drawLine(cx + 15, cy - 35, cx + 45, cy - 45, ILI9341_WHITE);
  display.drawLine(cx - 40, cy + 40, cx - 20, cy + 55, ILI9341_WHITE);
  display.drawLine(cx - 20, cy + 55, cx + 20, cy + 55, ILI9341_WHITE);
  display.drawLine(cx + 20, cy + 55, cx + 40, cy + 40, ILI9341_WHITE);
}