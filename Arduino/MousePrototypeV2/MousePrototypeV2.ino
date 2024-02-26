#include <ArduinoSTL.h>
#include <Keyboard.h>
#include <Mouse.h>
////#include <LiquidCrystal_I2C.h>
#include <vector>
#include <unordered_map>

#define is_pressed LOW

// NOTE: prob may not need these since we can just say its only on 
// const int POWER_SWITCH = 4;
// const int POWER_LED = 5;
// const int FINGERPRINT_LED = 6;

// MOUSE POINTER LOCATION PINS
const int MOUSE_X = A1; // pin 20
const int MOUSE_Y = A2; // pin 21

// MOUSE BUTTON PINS
    // primary
const int LEFT_CLICK = 1;   // M1 -- TX0
const int RIGHT_CLICK = 0;  // M2 -- AX1
const int MIDDLE_CLICK = 18; // M3
    // scrolling https://forum.arduino.cc/t/mouse-scroll-wheel-encoder-to-arduino/68935
const int SCROLL_UP = 14;
const int SCROLL_DOWN = 16;
    // side panel - basic
const int MOUSE_4 = 2;
const int MOUSE_5 = 3;
    // side panel - MOBA panel
const int MOUSE_6 = 4;
const int MOUSE_7 = 5;
const int MOUSE_8 = 6;
const int MOUSE_9 = 7;
const int MOUSE_10 = 8;
const int MOUSE_11 = 9;
const int MOUSE_12 = 10;
    // side panel - fingerprint
const int FINGERPRINT_SENSOR = 15;

// stores the current profile in the mouse
std::string PROFILE = "DEFAULT";

// CONFIG DATA
std::vector<int> dpi_values{800, 1200, 1600, 2400, 5000}; // default values
int current_dpi = 800; // default dpi
  // default button values
    // int = pin number
    // char = const global
    // acii table = https://www.commfront.com/pages/ascii-chart#:~:text=ASCII%20stands%20for%20American%20Standard,codes%20and%20Extended%20ASCII%20codes.
std::unordered_map<int, std::vector<char>> button_map{
  {LEFT_CLICK, {MOUSE_LEFT}},
  {RIGHT_CLICK, {MOUSE_RIGHT}},
  {MIDDLE_CLICK, {MOUSE_MIDDLE}},
  {MOUSE_4, {KEY_LEFT_ALT, KEY_RIGHT_ARROW}}, //pg back is alt+left
  {MOUSE_5, {KEY_LEFT_ALT, KEY_LEFT_ARROW}}, //pg fwd is alt+right
  {MOUSE_6, {'I'}},
  {MOUSE_7, {'n'}},
  {MOUSE_8, {'a'}},
  {MOUSE_9, {'\''}},
  {MOUSE_10, {'n'}},
  {MOUSE_11, {'i'}},
  {MOUSE_12, {'s'}},
  {FINGERPRINT_SENSOR, {}} // profiles saved
}
  // FINGERPRINT PROFILES - https://www.electroniclinic.com/biometric-fingerprint-scanner-with-arduino-pro-micro-capacitive-fingerprint-sensor-r557/
  // need to include libraries necessary & set up storage

//// LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x3F, 16, 2); // NOTE: DELETE LATER

// ENCODER (SCROLLING) VALUES
int _SCROLL_UP = 1;
int _SCROLL_DOWN = -1;

// do-not-touch variables
int mouseOn = 1;
int mousePressed = 0;
int mouseXZero = 0;
int mouseYZero = 0;
int scrolling = 0;
int dpiIndex = 0;
int buttonPressed = 0;
int fingerprintTimer = 0;
int fingerprintHeld = 0;
int fingerprintUnlocked = 0;

// user-defined variables
// int extremeCursorSpeed = 5;
int scrollSpeed = 100;
////int dpiValues[] = {400, 1000, 1600};
int mouseDelay = 10;
////char keys[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, 'n'};

void setup()
{
  //// pinMode(POWER_SWITCH, INPUT);
  //// pinMode(POWER_LED, OUTPUT);
  pinMode(LEFT_CLICK, INPUT_PULLUP);
  pinMode(RIGHT_CLICK, INPUT_PULLUP);
  //// pinMode(TEST_HOLD, INPUT_PULLUP);
  pinMode(SCROLL_UP, INPUT_PULLUP);
  pinMode(SCROLL_DOWN, INPUT_PULLUP);
  pinMode(MIDDLE_CLICK, INPUT_PULLUP);
  pinMode(FINGERPRINT_SENSOR, INPUT_PULLUP);
  pinMode(MOUSE_4, INPUT_PULLUP);
  pinMode(MOUSE_5, INPUT_PULLUP);
  pinMode(MOUSE_6, INPUT_PULLUP);
  pinMode(MOUSE_7, INPUT_PULLUP);
  pinMode(MOUSE_8, INPUT_PULLUP);
  pinMode(MOUSE_9, INPUT_PULLUP);
  pinMode(MOUSE_10, INPUT_PULLUP);
  pinMode(MOUSE_11, INPUT_PULLUP);
  pinMode(MOUSE_12, INPUT_PULLUP);
  //// pinMode(FINGERPRINT_LED, OUTPUT);
  pinMode(MOUSE_X, INPUT);
  pinMode(MOUSE_Y, INPUT);

  // begin lcd NOTE: delete later
  ////lcd.init();
  ////lcd.setCursor(0, 0);
  ////lcd.print("DPI: " + String(dpiValues[dpiIndex]));

  // begin mouse processes
  ////digitalWrite(POWER_LED, HIGH);
  Serial.begin(9600);
  Keyboard.begin();
  Mouse.begin();
  //// lcd.display(); // NOTE: delete
  //// lcd.backlight(); // NOTE: delete
  
  // callibrate mouse -- location read
  mouseXZero = analogRead(MOUSE_X);
  mouseYZero = analogRead(MOUSE_Y);
}

void loop()
{
  while (digitalRead(POWER_SWITCH) == is_pressed)
  {
    // turn emulated mouse on
    if (mouseOn == 0)
    {
      // begin mouse processes
      //// digitalWrite(POWER_LED, HIGH);
      Serial.begin(9600);
      Keyboard.begin();
      Mouse.begin();
      //// lcd.display();
      //// lcd.backlight();
      
      // callibrate mouse
      mouseXZero = analogRead(MOUSE_X);
      mouseYZero = analogRead(MOUSE_Y);

      mouseOn = 1;
    }

    // check joystick
    int mouseX = readCursor(MOUSE_X, mouseXZero);
    int mouseY = readCursor(MOUSE_Y, mouseYZero);

    Mouse.move(mouseX, mouseY, 0);

    // clicks are not emulated on keyboard to show how to hold clicks
    readClick(LEFT_CLICK, MOUSE_LEFT);
    readClick(RIGHT_CLICK, MOUSE_RIGHT);
    readClick(MIDDLE_CLICK, MOUSE_MIDDLE);

    // scroll is not emulated on keyboard to test continuous scrolling
    readScroll();
    
    }

    // test hold button to show how holding side panel buttons should work
    // readSidePanelHold(TEST_HOLD, 'P');

    // test side panel macro (example: opens new chrome window)
    readSidePanelMacro(TEST_HOLD, keys);

    // fingerprint button
    readFingerprint();

    delay(mouseDelay);
  

  // turn emulated mouse off
  if (mouseOn == 1)
  {
    // end mouse processes
    ////digitalWrite(POWER_LED, LOW);
    Serial.end();
    Keyboard.end();
    Mouse.end();
    ////lcd.noDisplay();
    ////lcd.noBacklight();

    mouseOn = 0;
  }
  }

// split string by a given delimiter
std::vector<string> split(std::string str, std::string delimiter) {

  std::stringstream ss(str);
  std::vector<std:string> config;
  string token;
  while (getline(ss, token, delimiter)) {
    config.push_back(token);
  }
  return config;
}

// converts the string to designated char
std::vector<char> string_to_mk_char(std::string str) {

  std::string macro_delimiter = "+";
  std::vector<std::string> temp_keybinds = split(str, macro_delimiter);
  std::vector<char> keybinds;

  for (int i = 0; i < temp_keybinds.size(); i++) {
    
    switch (temp_keybinds[i]) {
        // MOUSE
      case "MOUSE_LEFT": keybinds.push_back(MOUSE_LEFT);
      case "MOUSE_RIGHT": keybinds.push_back(MOUSE_RIGHT);
      case "MOUSE_MIDDLE": keybinds.push_back(MOUSE_MIDDLE);
        // KEYBOARD MODIFIERS
      case "KEY_LEFT_CTRL": keybinds.push_back(KEY_LEFT_CTRL);
      case "KEY_LEFT_SHIFT": keybinds.push_back(KEY_LEFT_SHIFT);
      case "KEY_LEFT_ALT": keybinds.push_back(KEY_LEFT_ALT);
      case "KEY_LEFT_GUI": keybinds.push_back(KEY_LEFT_GUI);
      case "KEY_RIGHT_CTRL": keybinds.push_back(KEY_RIGHT_CTRL);
      case "KEY_RIGHT_SHIFT": keybinds.push_back(KEY_RIGHT_SHIFT);
      case "KEY_RIGHT_ALT": keybinds.push_back(KEY_RIGHT_ALT);
      case "KEY_RIGHT_GUI": keybinds.push_back(KEY_RIGHT_GUI);
        // KEYBOARD - SPECIAL KEYS
      case "KEY_TAB": keybinds.push_back(KEY_TAB);
      case "KEY_CAPS_LOCK": keybinds.push_back(KEY_CAPS_LOCK);
      case "KEY_BACKSPACE": keybinds.push_back(KEY_BACKSPACE);
      case "KEY_RETURN": keybinds.push_back(KEY_RETURN);
      case "KEY_MENU": keybinds.push_back(KEY_MENU);
        // KEYBOARD - NAVIGATION CLUSTER
      case "KEY_INSERT": keybinds.push_back(KEY_INSERT);
      case "KEY_DELETE": keybinds.push_back(KEY_DELETE);
      case "KEY_HOME": keybinds.push_back(KEY_HOME);
      case "KEY_END": keybinds.push_back(KEY_END);
      case "KEY_PAGE_UP": keybinds.push_back(KEY_PAGE_UP);
      case "KEY_PAGE_DOWN": keybinds.push_back(KEY_PAGE_DOWN);
      case "KEY_UP_ARROW": keybinds.push_back(KEY_UP_ARROW);
      case "KEY_DOWN_ARROW": keybinds.push_back(KEY_DOWN_ARROW);
      case "KEY_LEFT_ARROW": keybinds.push_back(KEY_LEFT_ARROW);
      case "KEY_RIGHT_ARROW": keybinds.push_back(KEY_RIGHT_ARROW);
        // KEYBOARD - NUMERIC KEYPAD
      case "KEY_NUM_LOCK": keybinds.push_back(KEY_NUM_LOCK);
      case "KEY_KP_SLASH": keybinds.push_back(KEY_KP_SLASH);
      case "KEY_KP_ASTERISK": keybinds.push_back(KEY_KP_ASTERISK);
      case "KEY_KP_MINUS": keybinds.push_back(KEY_KP_MINUS);
      case "KEY_KP_PLUS": keybinds.push_back(KEY_KP_PLUS);
      case "KEY_KP_ENTER": keybinds.push_back(KEY_KP_ENTER);
      case "KEY_KP_1": keybinds.push_back(KEY_KP_1);
      case "KEY_KP_2": keybinds.push_back(KEY_KP_2);
      case "KEY_KP_3": keybinds.push_back(KEY_KP_3);
      case "KEY_KP_4": keybinds.push_back(KEY_KP_4);
      case "KEY_KP_5": keybinds.push_back(KEY_KP_5);
      case "KEY_KP_6": keybinds.push_back(KEY_KP_6);
      case "KEY_KP_7": keybinds.push_back(KEY_KP_7);
      case "KEY_KP_8": keybinds.push_back(KEY_KP_8);
      case "KEY_KP_9": keybinds.push_back(KEY_KP_9);
      case "KEY_KP_0": keybinds.push_back(KEY_KP_0);
      case "KEY_KP_DOT": keybinds.push_back(KEY_KP_DOT);
        // KEYBOARD - ESCAPE AND FUNCTION KEYS
      case "KEY_ESC": keybinds.push_back(KEY_ESC);
      case "KEY_F1": keybinds.push_back(KEY_F1);
      case "KEY_F2": keybinds.push_back(KEY_F2);
      case "KEY_F3": keybinds.push_back(KEY_F3);
      case "KEY_F4": keybinds.push_back(KEY_F4);
      case "KEY_F5": keybinds.push_back(KEY_F5);
      case "KEY_F6": keybinds.push_back(KEY_F6);
      case "KEY_F7": keybinds.push_back(KEY_F7);
      case "KEY_F8": keybinds.push_back(KEY_F8);
      case "KEY_F9": keybinds.push_back(KEY_F9);
      case "KEY_F10": keybinds.push_back(KEY_F10);
      case "KEY_F11": keybinds.push_back(KEY_F11);
      case "KEY_F12": keybinds.push_back(KEY_F12);
      case "KEY_F13": keybinds.push_back(KEY_F13);
      case "KEY_F14": keybinds.push_back(KEY_F14);
      case "KEY_F15": keybinds.push_back(KEY_F15);
      case "KEY_F16": keybinds.push_back(KEY_F16);
      case "KEY_F17": keybinds.push_back(KEY_F17);
      case "KEY_F18": keybinds.push_back(KEY_F18);
      case "KEY_F19": keybinds.push_back(KEY_F19);
      case "KEY_F20": keybinds.push_back(KEY_F20);
      case "KEY_F21": keybinds.push_back(KEY_F21);
      case "KEY_F22": keybinds.push_back(KEY_F22);
      case "KEY_F23": keybinds.push_back(KEY_F23);
      case "KEY_F24": keybinds.push_back(KEY_F24);
        // KEYBOARD - FUNCTION CONTROL KEYS
      case "KEY_PRINT_SCREEN": keybinds.push_back(KEY_PRINT_SCREEN);
      case "KEY_SCROLL_LOCK": keybinds.push_back(KEY_SCROLL_LOCK);
      case "KEY_PAUSE": keybinds.push_back(KEY_PAUSE);
      // covers the characters on the keyboard
      case default: keybinds.push_back(char (temp_keybinds[i]));
    }

  }
  
  return keybinds;

}

// loads configuration into the firmware
void read_config() {
  if (Serial.available() > 0) {
    config = Serial.readString(); // data recieved from the companion program

    // check to verify serial string recieved is from the correct program
    if (config == 'Hello') {
      Serial.write('World'); // send signal back to application

      std::string config_delimiter = ";";
      std::string button_delimiter = ":";

      config = Serial.readString(); // read again for actual mouse configuration
      std::vector<std::string> config_read = split(config, config_delimiter);

      for (int i = 0; i < config_read.size(); i++) {

        std::vector<std::string> button_keybind = split(config_read[i], button_delimiter);

        std::vector<char> button_value = string_to_mk_char(button_keybind[1]); // contains the macro/button mapping

        // switch case updates the keybinds in the unordered map
        switch(button_keybind[0]) {
          
          case "PROFILE": PROFILE = button_keybind[1]; // whatever the profile's name is
          case "LEFT_CLICK": button_map.put(LEFT_CLICK, button_value);
          case "RIGHT_CLICK": button_map.put(RIGHT_CLICK, button_value);
          case "MIDDLE_CLICK": button_map.put(MIDDLE_CLICK, button_value);
          case "MOUSE_4": button_map.put(MOUSE_4, button_value);
          case "MOUSE_5": button_map.put(MOUSE_5, button_value);
          case "MOUSE_6": button_map.put(MOUSE_6, button_value);
          case "MOUSE_7": button_map.put(MOUSE_7, button_value);
          case "MOUSE_8": button_map.put(MOUSE_8, button_value);
          case "MOUSE_9": button_map.put(MOUSE_9, button_value);
          case "MOUSE_10": button_map.put(MOUSE_10, button_value);
          case "MOUSE_11": button_map.put(MOUSE_11, button_value);
          case "MOUSE_12": button_map.put(MOUSE_12, button_value);
          case "FINGERPRINT_SENSOR": button_map.put(FINGERPRINT_SENSOR, button_value);

        }
      }
    }
  }
}

int readCursor(int axis, int offset)
{
  float distance = map(analogRead(axis) - offset, -offset, 1023 - offset, -dpiValues[dpiIndex]/200, dpiValues[dpiIndex]/200); // map analog readings to usable speed

  // prevent small movements
  if (abs(analogRead(axis) - offset) < 5)
  {
    distance = 0;
  }

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

void readSidePanelMacro(int pin, char key[])
{
  if (digitalRead(pin) == is_pressed) // if button is pressed, and button was not already pressed, press button
  {
    if (!buttonPressed)
    {
      buttonPressed = 1;
      if (key[0] != '\0')
      {
        Keyboard.press(key[0]);
      }
      
      if (key[1] != '\0')
      {
        Keyboard.press(key[1]);
      }
      
      if (key[2] != '\0')
      {
        Keyboard.press(key[2]);
      }
    }
  }
  else  // if button is not pressed, but button was already pressed, release button
  {
    if (buttonPressed)
    {
      buttonPressed = 0;
      Keyboard.releaseAll();
    }
  }
}