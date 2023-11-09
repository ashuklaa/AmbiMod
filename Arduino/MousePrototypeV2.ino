#include <Mouse.h>
#include <LiquidCrystal_I2C.h>

#define is_pressed LOW

const int POWER_SWITCH = 4;
const int POWER_LED = 5;
const int LEFT_CLICK = 15;
const int RIGHT_CLICK = 10;
const int TEST_HOLD = 18;
const int SCROLL_UP = 14;
const int SCROLL_DOWN = 16;
const int MIDDLE_CLICK = 21;
const int FINGERPRINT = 7;
const int FINGERPRINT_LED = 6;
const int MOUSE_X = A1;
const int MOUSE_Y = A2;

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 2);

int mousePressed = 0;
int mouseXZero = 0;
int mouseYZero = 0;
int scrolling = 0;
int dpiIndex = 0;
int buttonPressed = 0;
int fingerprintTimer = 0;
int fingerprintHeld = 0;
int fingerprintUnlocked = 0;

int extremeCursorSpeed = 10;
int scrollSpeed = 1;
int dpiValues[] = {400, 1000, 1600};
int mouseDelay = 10;

void setup()
{
  pinMode(POWER_SWITCH, INPUT);
  pinMode(POWER_LED, OUTPUT);
  pinMode(LEFT_CLICK, INPUT_PULLUP);
  pinMode(RIGHT_CLICK, INPUT_PULLUP);
  pinMode(TEST_HOLD, INPUT_PULLUP);
  pinMode(SCROLL_UP, INPUT_PULLUP);
  pinMode(SCROLL_DOWN, INPUT_PULLUP);
  pinMode(MIDDLE_CLICK, INPUT_PULLUP);
  pinMode(FINGERPRINT, INPUT_PULLUP);
  pinMode(FINGERPRINT_LED, OUTPUT);
  pinMode(MOUSE_X, INPUT);
  pinMode(MOUSE_Y, INPUT);

  mouseXZero = readCursor(MOUSE_X);
  mouseYZero = readCursor(MOUSE_Y);

  Serial.begin(9600);
  Mouse.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("DPI: " + String(dpiValues[dpiIndex]));
}

void loop()
{
  while (digitalRead(POWER_SWITCH) == is_pressed)
  {
    digitalWrite(POWER_LED, HIGH); // emulated mouse on

    // check joystick
    int mouseX = readCursor(MOUSE_X) - mouseXZero;
    int mouseY = readCursor(MOUSE_Y) - mouseYZero;

    Mouse.move(mouseX, mouseY, 0);

    // clicks are not emulated on keyboard to show how to hold clicks
    readClick(LEFT_CLICK, MOUSE_LEFT);
    readClick(RIGHT_CLICK, MOUSE_RIGHT);
    readClick(MIDDLE_CLICK, MOUSE_MIDDLE);

    // scroll is not emulated on keyboard to test continuous scrolling
    readScroll();

    // side panel emulations on keyboard
    if (Serial.available() > 0)
    {
      switch (Serial.read())
      {
        case 115: // 's' key acts as DPI button
          changeDPI();
          break;

        case 49:  // '1' key acts as M4
          readSidePanel('Q');
          break;
          
        case 50:  // '2' key acts as M5
          readSidePanel('W');
          break;
          
        case 51:  // '3' key acts as M6
          readSidePanel('E');
          break;
          
        case 52:  // '4' key acts as M7
          readSidePanel('R');
          break;
          
        case 53:  // '5' key acts as M8
          readSidePanel('T');
          break;
          
        case 54:  // '6' key acts as M9
          readSidePanel('Y');
          break;
          
        case 55:  // '7' key acts as M10
          readSidePanel('U');
          break;
          
        case 56:  // '8' key acts as M11
          readSidePanel('I');
          break;
          
        case 57:  // '9' key acts as M12
          readSidePanel('O');
          break;
      }
    }

    // test hold button to show how holding side panel buttons should work
    readSidePanelHold(TEST_HOLD, 'P');

    // fingerprint button
    readFingerprint();

    delay(mouseDelay);
  }

  digitalWrite(POWER_LED, LOW);  // emulated mouse off
}

int readCursor(int axis)
{
  float distance = map(analogRead(axis), 0, 1023, -extremeCursorSpeed, extremeCursorSpeed); // map analog readings to usable speed
  return distance;
}

void readClick(int pin, char button)
{
  if (digitalRead(pin) == is_pressed) // if button is pressed, and button was not already pressed, press button
  {
    if (!Mouse.isPressed(button))
    {
      Mouse.press(button);
      Serial.println("Button pressed.");
    }
  }
  else  // if button is not pressed, but button was already pressed, release button
  {
    if (Mouse.isPressed(button))
    {
      Mouse.release(button);
      Serial.println("Button released.");
    }
  }
}

void readScroll() // not having the nested if condition allows holding because it will not stop you if the button was pressed already or not
{
  if (digitalRead(SCROLL_UP) == is_pressed) // if scroll up is pressed, scroll up
  {
    Mouse.move(0, 0, 1 * scrollSpeed);
  }
  else if(digitalRead(SCROLL_DOWN) == is_pressed) // if scroll down is pressed, scroll down
  {
    Mouse.move(0, 0, -1 * scrollSpeed);
  }
  else  // if nothing is pressed, no scrolling
  {
    Mouse.move(0, 0, 0);
  }
}

void changeDPI()
{
	dpiIndex++; // update dpi index
	if (dpiIndex == sizeof(dpiValues) / sizeof(int))  // out of bounds prevention
	{
		dpiIndex = 0;
	}
  lcd.clear();  // write dpi value to lcd and to serial monitor
	lcd.setCursor(0, 0);
  lcd.print("DPI: " + String(dpiValues[dpiIndex]));
	Serial.println("DPI: " + String(dpiValues[dpiIndex]));
}

void readSidePanel(char keybind)
{
  Serial.println(keybind);  // user should be able to change this value
}

void readSidePanelHold(int pin, char keybind)
{
  if (digitalRead(pin) == is_pressed) // if button is pressed, and button was not already pressed, press button
  {
    if (!buttonPressed)
    {
      buttonPressed = 1;
      Serial.println(String(keybind) + " is pressed.");
    }
  }
  else  // if button is not pressed, but button was already pressed, release button
  {
    if (buttonPressed)
    {
      buttonPressed = 0;
      Serial.println(String(keybind) + " is released.");
    }
  }
}

void readFingerprint()
{
  if (digitalRead(FINGERPRINT) == is_pressed)
  {
    fingerprintTimer += mouseDelay;

    if (fingerprintHeld == 0) // if not already held, let user know it is scanning
    {
      fingerprintHeld = 1;
      Serial.println("Scanning fingerprint...");
    }

    if (fingerprintTimer >= 3000) // if held fingerprint button for at least 3 seconds, turn on led (unlock)
    {
      digitalWrite(FINGERPRINT_LED, HIGH);

      if (fingerprintUnlocked == 0)
      {
        fingerprintUnlocked = 1;
        Serial.println("Unlocked.");
      }
    }
  }
  else  // reset timer and turn off led (lock) if not held
  {
    fingerprintTimer = 0;
    fingerprintHeld = 0;
    fingerprintUnlocked = 0;
    digitalWrite(FINGERPRINT_LED, LOW);
  }
}