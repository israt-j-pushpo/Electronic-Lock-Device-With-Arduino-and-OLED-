#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---- Key Pins ----
#define KEY_A 4
#define KEY_B 5
#define KEY_C 6
#define KEY_D 7

#define RELAY_STATUS_PIN 3
#define LOCKOUT_OUT_PIN 8

#define MAX_ATTEMPTS 4
#define LOCKOUT_TIME 60000UL

String correctCode = "ABDC";
String inputCode = "";

int wrongCount = 0;
bool lockoutActive = false;
unsigned long lockoutStart = 0;

void setup() {

  Serial.begin(9600);

  pinMode(KEY_A, INPUT_PULLUP);
  pinMode(KEY_B, INPUT_PULLUP);
  pinMode(KEY_C, INPUT_PULLUP);
  pinMode(KEY_D, INPUT_PULLUP);

  pinMode(RELAY_STATUS_PIN, INPUT);
  pinMode(LOCKOUT_OUT_PIN, OUTPUT);
  digitalWrite(LOCKOUT_OUT_PIN, LOW);

  Wire.begin();   // important

  // ---- Try 0x3C first ----
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {

    Serial.println("0x3C failed, trying 0x3D...");

    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      Serial.println("OLED not found!");
      while (1);   // stop here
    }
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  showMessage("Enter Code");
}

void loop() {

  if (lockoutActive) {
    handleLockout();
    return;
  }

  checkKeyPress();

  if (digitalRead(RELAY_STATUS_PIN) == HIGH) {
    showMessage("LOCK OPEN");
  }
}

// -------- Key Detection --------

void checkKeyPress() {

  if (digitalRead(KEY_A) == LOW) addKey('A');
  if (digitalRead(KEY_B) == LOW) addKey('B');
  if (digitalRead(KEY_C) == LOW) addKey('C');
  if (digitalRead(KEY_D) == LOW) addKey('D');
}

void addKey(char key) {

  delay(200);

  inputCode += key;

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Input:");
  display.setCursor(0, 30);
  display.println(inputCode);
  display.display();

  if (inputCode.length() == 4) {

    if (inputCode == correctCode) {
      showMessage("Correct Code");
      delay(1000);
    }
    else {
      wrongCount++;
      showWrong();
    }

    inputCode = "";
  }
}

// -------- Lockout --------

void handleLockout() {

  unsigned long elapsed = millis() - lockoutStart;

  if (elapsed >= LOCKOUT_TIME) {
    lockoutActive = false;
    wrongCount = 0;
    digitalWrite(LOCKOUT_OUT_PIN, LOW);
    showMessage("Enter Code");
  }
  else {
    int remain = (LOCKOUT_TIME - elapsed) / 1000;
    showCountdown(remain);
  }
}

// -------- Display --------

void showWrong() {

  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("Wrong Code!");
  display.setCursor(0, 30);
  display.print("Attempt: ");
  display.print(wrongCount);
  display.display();

  delay(1000);

  if (wrongCount >= MAX_ATTEMPTS) {
    lockoutActive = true;
    lockoutStart = millis();
    digitalWrite(LOCKOUT_OUT_PIN, HIGH);
  }

  showMessage("Enter Code");
}

void showMessage(String msg) {
  display.clearDisplay();
  display.setCursor(0, 25);
  display.println(msg);
  display.display();
}

void showCountdown(int sec) {
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("LOCKED OUT!");
  display.setCursor(0, 30);
  display.print("Wait: ");
  display.print(sec);
  display.println(" sec");
  display.display();
}
