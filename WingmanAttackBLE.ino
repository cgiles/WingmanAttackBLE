//----------
//Convert a midi/joystick port Logitech Wingman Attack 1 into a bluetooth controller
//
//wire color :
//  Analog :
//    Violet : Throttle Pin:36
//    Brown  : Axis Y   Pin:39
//    Grey   : Axis X   Pin:32
//    Red    : VCC
//  Switch :
//    Orange : S1   Pin:5
//    Green  : S2   Pin:18
//    Yellow : S3   Pin:23
//    White  : S4   Pin:19
//    Black  : GND
//
//  Battery Pin:33
//----------
//Use this library : https://github.com/lemmingDev/ESP32-BLE-Gamepad
//----------
#include <BleGamepad.h>

#define S1IN 5
#define S2IN 18
#define S3IN 23
#define S4IN 19

#define TIN 36
#define YIN 39
#define XIN 32

#define BATIN 33

#define LEDOUT 2

#define DEBOUNCE 30

BleGamepad wingman("WingmanAttack", "Logitech", 100);


int tVal = 0;
int xVal = 0;
int yVal = 0;
int bVal = 0;

int tPos = 0;
int xPos = 0;
int yPos = 0;

int bLev = 0;
int p_bLev = 0;

int debounceTimes[4];

int buttons[] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4};



void IRAM_ATTR S1Update() {
  buttonChangeState(S1IN, 1);
}
void IRAM_ATTR S2Update() {
  buttonChangeState(S2IN, 2);
}
void IRAM_ATTR S3Update() {
  buttonChangeState(S3IN, 3);
}
void IRAM_ATTR S4Update() {
  buttonChangeState(S4IN, 4);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  pinMode(S1IN, INPUT_PULLUP);
  pinMode(S2IN, INPUT_PULLUP);
  pinMode(S3IN, INPUT_PULLUP);
  pinMode(S4IN, INPUT_PULLUP);

  

  attachInterrupt(S1IN, S1Update, CHANGE);
  attachInterrupt(S2IN, S2Update, CHANGE);
  attachInterrupt(S3IN, S3Update, CHANGE);
  attachInterrupt(S4IN, S4Update, CHANGE);

  pinMode(LEDOUT,OUTPUT);

  digitalWrite(LEDOUT,HIGH);

  Serial.println("Starting BLE work!");
  bLev = GetBatteryLevel();
  p_bLev = bLev;
  wingman.begin();
  wingman.setBatteryLevel(bLev);
}

void loop() {
  if (wingman.isConnected()) {
    tVal = analogRead(TIN);
    yVal = analogRead(YIN);
    xVal = analogRead(XIN);

    tPos = map(tVal, 0, 4095, 0, 127);
    yPos = map(yVal, 0, 4095, -127, 127);
    xPos = map(xVal, 0, 4095, -127, 127);

    bLev = GetBatteryLevel();
    if (bLev != p_bLev) {
      p_bLev = bLev;
      wingman.setBatteryLevel(bLev);
    }
    wingman.setAxes(xPos, yPos, 0, 0, 0, tPos, DPAD_CENTERED);
    Serial.print(" Axis X :");
    Serial.print(xVal);
    Serial.print(" Axis Y :");
    Serial.print(yVal);
    Serial.print(" Axis Z :");
    Serial.print(tVal);
    Serial.print(" Battery :");
    Serial.println(bLev);
    
    wingman.setBatteryLevel(bLev);
    delay(8);
  }
}
void buttonChangeState(int pin, int buttonId) {
  if (millis() - DEBOUNCE >= debounceTimes[buttonId]) {
    debounceTimes[buttonId] = millis();
    int state = digitalRead(pin);
    if (state == LOW) {
      wingman.press(buttons[buttonId]);
    } else if (state == HIGH) {
      wingman.release(buttons[buttonId]);
    }
  }
}

int GetBatteryLevel() {
  return map(analogRead(BATIN), 0, 4095, 0, 100);
}
