#include <LiquidCrystal.h>

// LCD pins (4-bit mode): RS, E, D4, D5, D6, D7
const int LCD_RS = 33;
const int LCD_E  = 32;
const int LCD_D4 = 21;
const int LCD_D5 = 22;
const int LCD_D6 = 13;
const int LCD_D7 = 15;

LiquidCrystal lcd(LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7);

// Push button inputs (active LOW with INPUT_PULLUP)
const int btn[4] = {12, 14, 27, 26};

// LED outputs
const int greenLED[4] = {5, 18, 19, 23};
const int redLED[4]   = {2, 4, 16, 17};

// Vehicle count (press button to increment)
volatile int vehicles[4] = {0, 0, 0, 0};

// Timing and scheduling
const int minGreen = 5;    // minimum green seconds per path
const int maxGreen = 30;   // maximum green seconds per path
int currentPath = 0;

// State variables for countdown (non-blocking)
unsigned long pathEndMillis = 0;
int pathCountdownSec = 0;
unsigned long lastLCDUpdate = 0;
unsigned long lcdUpdateInterval = 1000; // ms

// Debounce
unsigned long lastBtnTime[4] = {0,0,0,0};
const unsigned long debounceMs = 200;

void setup() {
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();

  // Pins
  for (int i=0; i<4; i++) {
    pinMode(btn[i], INPUT_PULLUP);
    pinMode(greenLED[i], OUTPUT);
    pinMode(redLED[i], OUTPUT);
    digitalWrite(greenLED[i], LOW);
    digitalWrite(redLED[i], HIGH); // start as red
  }

  // Optional splash
  lcd.setCursor(0, 0);
  lcd.print("Traffic System");
  delay(1200);
  lcd.clear();

  // Start first path immediately
  startPath(currentPath);
}

void loop() {
  // 1) Read buttons (non-blocking, with debounce)
  for (int i=0; i<4; i++) {
    if (digitalRead(btn[i]) == LOW) { // pressed (active LOW)
      unsigned long now = millis();
      if (now - lastBtnTime[i] > debounceMs) {
        vehicles[i]++;                      // increment vehicle count
        lastBtnTime[i] = now;
        // immediate LCD update (we'll force next LCD refresh)
        lastLCDUpdate = 0;
      }
    }
  }

  // 2) Update countdown/state
  unsigned long now = millis();
  if (now >= pathEndMillis) {
    // Current path ended — move to next path
    finishPath(currentPath);
    currentPath = (currentPath + 1) % 4;
    startPath(currentPath);
  }

  // 3) Update LCD occasionally to avoid flicker
  if (now - lastLCDUpdate >= lcdUpdateInterval) {
    updateLCD();
    lastLCDUpdate = now;
  }

  // small sleep — keeps loop responsive but reduces CPU
  delay(10);
}

// Start a path: compute countdown and set LEDs
void startPath(int path) {
  // compute dynamic green time: default minGreen + vehicles/2 (you can change formula)
  int extra = vehicles[path] / 2;               // each 2 vehicles -> +1s
  int gtime = minGreen + extra;
  if (gtime > maxGreen) gtime = maxGreen;
  if (gtime < minGreen) gtime = minGreen;

  pathCountdownSec = gtime;
  pathEndMillis = millis() + (unsigned long)pathCountdownSec * 1000UL;

  // set LEDs: this path green, others red
  for (int i=0; i<4; i++) {
    if (i == path) {
      digitalWrite(greenLED[i], HIGH);
      digitalWrite(redLED[i], LOW);
    } else {
      digitalWrite(greenLED[i], LOW);
      digitalWrite(redLED[i], HIGH);
    }
  }
}

// Called when a path finishes — optional vehicle clearing simulation
void finishPath(int path) {
  // Simulate clearing vehicles during green time:
  // We remove roughly (gtime / 2) vehicles (approx 2 vehicles per second)
  int removed = pathCountdownSec / 2;
  if (removed > 0) {
    vehicles[path] -= removed;
    if (vehicles[path] < 0) vehicles[path] = 0;
  }
}

// Update the LCD display
void updateLCD() {
  // First line: vehicle counts (compact)
  // Try to keep it within 16 chars: "1:5 2:4 3:7 4:2"
  char line1[17];
  snprintf(line1, sizeof(line1), "1:%d 2:%d 3:%d 4:%d",
           vehicles[0], vehicles[1], vehicles[2], vehicles[3]);

  // Second line: show countdown for current path
  // compute remaining seconds more accurately
  long remainingMs = (long)pathEndMillis - (long)millis();
  int remainingSec = 0;
  if (remainingMs > 0) remainingSec = (remainingMs + 500) / 1000; // round
  char line2[17];
  snprintf(line2, sizeof(line2), "%d:%02d  GREEN", currentPath+1, remainingSec);

  // Write to LCD (clear then print)
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}
