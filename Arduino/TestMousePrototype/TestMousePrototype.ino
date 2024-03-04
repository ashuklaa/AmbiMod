/**
 * NEED TO FIGURE OUT HOW TO DO THREADS HERE TO CONSTANTLY CHECK FOR UPDATES.
 * thread 1 -- check for new configs
 * thread 2 -- read button presses (?)
 * not sure if actually need this, primarily because the flashing would happen, updates would then occur, then mouse proceeds as normal for the 
 * 
 * 
 * NEED TO UPDATE THE VALUE READS FOR THE BUTTONS + ENCODER TO THE BYTE VALUES FROM THE REGISTER
*/


#include <Keyboard.h>
#include <Mouse.h>
#include <ArduinoSTL.h>
#include <PMW3360.h>
#include <Adafruit_Fingerprint.h>
#include <Hashtable.h>

#define is_pressed LOW

std::vector<char> mouse_keys = {MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE};
std::vector<char> keyboard_modifiers = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI, KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, 
                      KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_TAB, KEY_CAPS_LOCK, KEY_BACKSPACE, KEY_RETURN, KEY_MENU,
                      KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_UP_ARROW, 
                      KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_NUM_LOCK, KEY_KP_SLASH, KEY_KP_ASTERISK,
                      KEY_KP_MINUS, KEY_KP_PLUS, KEY_KP_ENTER, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4, KEY_KP_5,
                      KEY_KP_6, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_0, KEY_KP_DOT, KEY_ESC, KEY_F1, KEY_F2,
                      KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_F13,
                      KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18, KEY_F19, KEY_F20, KEY_F21, KEY_F22, KEY_F23,
                      KEY_F24, KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE};
std::vector<char> keyboard_alpha = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                        'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
std::vector<char> keyboard_num = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
std::vector<char> keyboard_symbols_ns = {'`', '-', '=', '[', ']', '\\', ';', '\'', ',', '.', '/'};
std::vector<char> keyboard_symbols_s = {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}', '|', ':', '\"', '<', '>', '?'};

Hashtable<char, std::string> mouse_keyboard_keys;

// MOUSE POINTER LOCATION PINS
const int MOUSE_X = A1; // pin 20
const int MOUSE_Y = A2; // pin 21

// NOTE: ALL BUTTONS WILL BECOME uint8_t DATA TYPE 

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

// PMW3360 Sensor :: https://github.com/SunjunKim/PMW3360/tree/master
//PMW3360 sensor;
//Adafruit_Fingerprint finger = Adafruit_Fingerprint();

// stores the current profile in the mouse
std::string PROFILE = "DEFAULT";

// CONFIG DATA
std::vector<int> dpi_values{800, 1200, 1600, 2400, 5000}; // default values
int current_dpi = 800; // default dpi
  // default button values
    // int = pin number
    // char = const global
    // acii table = https://www.commfront.com/pages/ascii-chart#:~:text=ASCII%20stands%20for%20American%20Standard,codes%20and%20Extended%20ASCII%20codes.

std::vector<int> button_set = {LEFT_CLICK, RIGHT_CLICK, MIDDLE_CLICK, MOUSE_4, MOUSE_5, MOUSE_6, MOUSE_7, MOUSE_8, MOUSE_9, MOUSE_10, MOUSE_11, MOUSE_12};
std::vector<char> button_binds[] = {{MOUSE_LEFT}, {MOUSE_RIGHT}, {MOUSE_MIDDLE}, {KEY_LEFT_ALT, KEY_RIGHT_ARROW}, {KEY_LEFT_ALT, KEY_LEFT_ARROW}, {'I'}, {'n'}, {'a'}, {'\''}, {'n'}, {'i'}, {'s'}};

Hashtable<int, std::vector<char>> button_map;

/*{
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
  {MOUSE_12, {'s'}}, // profiles saved
}*/


int DEBOUNCE = 10;

  // FINGERPRINT PROFILES - https://github.com/adafruit/Adafruit-Fingerprint-Sensor-Library
  // need to include libraries necessary & set up storage

// FINGERPRINT_SENSOR

// ENCODER (SCROLLING) VALUES
const int _SCROLL_UP = 1;
const int _SCROLL_DOWN = -1;

// do-not-touch variables
int mousePressed = 0;
int scrolling = 0;
int dpiIndex = 0;
int buttonPressed = 0;

// user-defined variables
// int extremeCursorSpeed = 5;
int scrollSpeed = 100;
int mouseDelay = 10;

// NOTE: NEED TO UPDATE
void setup()
{
  pinMode(LEFT_CLICK, INPUT_PULLUP);
  pinMode(RIGHT_CLICK, INPUT_PULLUP);
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
  pinMode(MOUSE_X, INPUT);
  pinMode(MOUSE_Y, INPUT);


  // begin mouse processes
  Serial.begin(9600);
  //sensor.setCPI(current_dpi);
  //finger.begin(57600);
  Keyboard.begin();
  Mouse.begin();

  // INITIALIZE HASHTABLE FOR GENERAL BUTTON CHARS
    // add mouse keys to hashtable
  for(int i = 0; i < mouse_keys.size(); i++){
    mouse_keyboard_keys.put(mouse_keys[i], "MOUSE");
  }
    // add keyboard alphas // upper & lower case
  for(int i = 0; i < keyboard_alpha.size(); i++){
    mouse_keyboard_keys.put(keyboard_alpha[i], "KEYBOARD");
    mouse_keyboard_keys.put(tolower(keyboard_alpha[i]), "KEYBOARD");
  }
    // add keyboard numbers
  for(int i = 0; i < keyboard_num.size(); i++) {
    mouse_keyboard_keys.put(keyboard_num[i], "KEYBOARD");
  }
    // add keyboard symbols (no shift)
  for(int i = 0; i < keyboard_symbols_ns.size(); i++) {
    mouse_keyboard_keys.put(keyboard_symbols_ns[i], "KEYBOARD");
  }
    // add keyboard modifiers specific to Keyboard.h
  for (int i = 0; i < keyboard_modifiers.size(); i++) {
    mouse_keyboard_keys.put(keyboard_modifiers[i], "KEYBOARD");  
  }
    // add keyboard symbols (shift)
  for(int i = 0; i < keyboard_symbols_s.size(); i++) {
    mouse_keyboard_keys.put(keyboard_symbols_s[i], "KEYBOARD");
  }    

  // INITIALIZE HASHTABLE FOR DEFAULT KEYBINDS
  for (int i = 0; i < button_set.size(); i++) {
    button_map.put(button_set[i], button_binds[i]);
  }

}

// split string by a given delimiter
std::vector<std::string> split(std::string str, char delimiter) {
  
  std::vector<std::string> config;

  std::string temp = "";

  // iterate through each character in the string
  for (int i = 0; i < str.length(); i++) {
    // when delimiter is found, reset temp string and add previous temp value to config vector
    if (str[i] == delimiter) {

      config.push_back(temp);
      temp = "";

    } 
    else { // add character to temp string
    
      temp += str[i];
    
    }
  }

  return config;
}

// CONFIG RETURN HELPERS
// for the char to string
std::string mk_keybind_to_string(char button) {

    switch(button){
      case MOUSE_LEFT: return "MOUSE_LEFT";
      case MOUSE_RIGHT: return "MOUSE_RIGHT";
      case MOUSE_MIDDLE: return "MOUSE_MIDDLE";
        // KEYBOARD MODIFIERS
      case KEY_LEFT_CTRL: return "KEY_LEFT_CTRL";
      case KEY_LEFT_SHIFT: return "KEY_LEFT_SHIFT";
      case KEY_LEFT_ALT: return "KEY_LEFT_ALT";
      case KEY_LEFT_GUI: return "KEY_LEFT_GUI";
      case KEY_RIGHT_CTRL: return "KEY_RIGHT_CTRL";
      case KEY_RIGHT_SHIFT: return "KEY_RIGHT_SHIFT";
      case KEY_RIGHT_ALT: return "KEY_RIGHT_ALT";
      case KEY_RIGHT_GUI: return "KEY_RIGHT_GUI";
        // KEYBOARD - SPECIAL KEYS
      case KEY_TAB: return "KEY_TAB";
      case KEY_CAPS_LOCK: return "KEY_CAPS_LOCK";
      case KEY_BACKSPACE: return "KEY_BACKSPACE";
      case KEY_RETURN: return "KEY_RETURN";
      case KEY_MENU: return "KEY_MENU";
        // KEYBOARD - NAVIGATION CLUSTER
      case KEY_INSERT: return "KEY_INSERT";
      case KEY_DELETE: return "KEY_DELETE";
      case KEY_HOME: return "KEY_HOME";
      case KEY_END: return "KEY_END";
      case KEY_PAGE_UP: return "KEY_PAGE_UP";
      case KEY_PAGE_DOWN: return "KEY_PAGE_DOWN";
      case KEY_UP_ARROW: return "KEY_UP_ARROW";
      case KEY_DOWN_ARROW: return "KEY_DOWN_ARROW";
      case KEY_LEFT_ARROW: return "KEY_LEFT_ARROW";
      case KEY_RIGHT_ARROW: return "KEY_RIGHT_ARROW";
        // KEYBOARD - NUMERIC KEYPAD
      case KEY_NUM_LOCK: return "KEY_NUM_LOCK";
      case KEY_KP_SLASH: return "KEY_KP_SLASH";
      case KEY_KP_ASTERISK: return "KEY_KP_ASTERISK";
      case KEY_KP_MINUS: return "KEY_KP_MINUS";
      case KEY_KP_PLUS: return "KEY_KP_PLUS";
      case KEY_KP_ENTER: return "KEY_KP_ENTER";
      case KEY_KP_1: return "KEY_KP_1";
      case KEY_KP_2: return "KEY_KP_2";
      case KEY_KP_3: return "KEY_KP_3";
      case KEY_KP_4: return "KEY_KP_4";
      case KEY_KP_5: return "KEY_KP_5";
      case KEY_KP_6: return "KEY_KP_6";
      case KEY_KP_7: return "KEY_KP_7";
      case KEY_KP_8: return "KEY_KP_8";
      case KEY_KP_9: return "KEY_KP_9";
      case KEY_KP_0: return "KEY_KP_0";
      case KEY_KP_DOT: return "KEY_KP_DOT";
        // KEYBOARD - ESCAPE AND FUNCTION KEYS
      case KEY_ESC: return "KEY_ESC";
      case KEY_F1: return "KEY_F1";
      case KEY_F2: return "KEY_F2";
      case KEY_F3: return "KEY_F3";
      case KEY_F4: return "KEY_F4";
      case KEY_F5: return "KEY_F5";
      case KEY_F6: return "KEY_F6";
      case KEY_F7: return "KEY_F7";
      case KEY_F8: return "KEY_F8";
      case KEY_F9: return "KEY_F9";
      case KEY_F10: return "KEY_F10";
      case KEY_F11: return "KEY_F11";
      case KEY_F12: return "KEY_F12";
      case KEY_F13: return "KEY_F13";
      case KEY_F14: return "KEY_F14";
      case KEY_F15: return "KEY_F15";
      case KEY_F16: return "KEY_F16";
      case KEY_F17: return "KEY_F17";
      case KEY_F18: return "KEY_F18";
      case KEY_F19: return "KEY_F19";
      case KEY_F20: return "KEY_F20";
      case KEY_F21: return "KEY_F21";
      case KEY_F22: return "KEY_F22";
      case KEY_F23: return "KEY_F23";
      case KEY_F24: return "KEY_F24";
        // KEYBOARD - FUNCTION CONTROL KEYS
      case KEY_PRINT_SCREEN: return "KEY_PRINT_SCREEN";
      case KEY_SCROLL_LOCK: return "KEY_SCROLL_LOCK";
      case KEY_PAUSE: return "KEY_PAUSE";
    }

}

// for buttons to strings
std::string button_pin_to_string(int button) {

  switch(button) {
    case LEFT_CLICK: return "LEFT_CLICK";
    case RIGHT_CLICK: return "RIGHT_CLICK";
    case MIDDLE_CLICK: return "MIDDLE_CLICK";
    case MOUSE_4: return "MOUSE_4";
    case MOUSE_5: return "MOUSE_5";
    case MOUSE_6: return "MOUSE_6";
    case MOUSE_7: return "MOUSE_7";
    case MOUSE_8: return "MOUSE_8";
    case MOUSE_9: return "MOUSE_9";
    case MOUSE_10: return "MOUSE_10";
    case MOUSE_11: return "MOUSE_11";
    case MOUSE_12: return "MOUSE_12";
  }

}


// CONFIG READ HELPERS
// convert string from config to char value & add to vector
std::vector<char> string_to_mk_char(std::string str) {

  char macro_delimiter = '+';
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

// FOR KEYBINDS ONLY
std::string keybind_to_string(int button, std::vector<char> keybinds) {

  std::string keybind_string = "";

  for (int i = 0; i < keybinds.size(); i++) {
    keybind_string += mk_keybind_to_string(keybind[i]);
    if (i != keybinds.size()-1) { // adds macro delimiter
      keybind_string += "+"; 
    }
  }

  return keybind_string;

}


// BUTTON READ HELPERS
// helper function for button reads
std::vector<char> get_button_keybind(int pin) {

  return button_map.getElement(pin);

}

// returns if the keybind is a mouse or keyboard operation
std::string check_keybind_type(int pin) {

  return mouse_keyboard_keys.getElement(pin); // 0 = mouse, 1 = keyboard

}



// CONFIG READ / RETURN
// loads configuration into the firmware
void read_config() {
  if (Serial.available() > 0) {
    std::string config = Serial.readString(); // data recieved from the companion program

    // check to verify serial string recieved is from the correct program
    if (config == 'Good Morning') {
      Serial.write('Gamer'); // send signal back to application

      std::string config_delimiter = ";";
      std::string button_delimiter = ":";

      config = Serial.readString(); // read again for actual mouse configuration
      std::vector<std::string> config_read = split(config, config_delimiter);

      for (int i = 0; i < config_read.size(); i++) {

        std::vector<std::string> button_keybind = split(config_read[i], button_delimiter);

        std::vector<char> button_value = string_to_mk_char(button_keybind[1]); // contains the macro/button mapping

        // switch case updates the keybinds in the Hashtable
        switch(button_keybind[0]) {
          
          case "PROFILE": PROFILE = button_keybind[1]; // whatever the profile's name is
          case "DPI": current_dpi = button_keybind[1];
          case "DPI_SET": dpi_values = button_value;
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
          // case "FINGERPRINT_SENSOR": assign_button_config(FINGERPRINT_SENSOR, button_value);
          case "DEBOUNCE": DEBOUNCE = button_keybind[1];

        }
      }
    }
  }
}

void config_test() {

  if (Serial.available() > 0) {
    std::string current_config = "";
    
    // PROFILE
    current_config += "PROFILE:" + PROFILE + ";";

    // DPI
    current_config += "DPI:" + std::to_string(current_dpi) + ";";

    // DPI_SET
    current_config += "DPI_SET:"; 
    for(int i = 0; i < dpi_values.size(); i++) {
      current_config += std::to_string(dpi_values[i]);
      if (i != dpi_values.size()-1) {
        current_config += "+"
      }
    }
    current_config += ";";

    current_config += "DEBOUNCE:" + std::tostring(DEBOUNCE) + ";";

    // LEFT_CLICK
    current_config += "LEFT_CLICK:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(LEFT_CLICK));
    }
    current_config += ";";

    current_config += "RIGHT_CLICK:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(RIGHT_CLICK));
    }
    current_config += ";";

    current_config += "MIDDLE_CLICK:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MIDDLE_CLICK));
    }
    current_config += ";";

    current_config += "MOUSE_4:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_4));
    }
    current_config += ";";

    current_config += "MOUSE_5:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_5));
    }
    current_config += ";";    

    current_config += "MOUSE_6:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_6));
    }
    current_config += ";";

    current_config += "MOUSE_7:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_7));
    }
    current_config += ";";
  
    current_config += "MOUSE_8:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_8));
    }
    current_config += ";";    
  
    current_config += "MOUSE_9:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_9));
    }
    current_config += ";";

    current_config += "MOUSE_10:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_10));
    }
    current_config += ";";

    current_config += "MOUSE_11:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_11));
    }
    current_config += ";";   

    current_config += "MOUSE_12:";
    for(int i = 0; i < button_binds[0].size(); i++) {
      current_config += keybind_to_string(button_map.getElement(MOUSE_12));
    }
    current_config += ";";

  }

}





// BUTTON HANDLERS

void mouse_press(int pin) {



}

void keyboard_press(int pin) {



}

void macro_press(int type, int pin) {



}

void button_press(int pin) {

  if (pin == is_pressed) {

    std::vector<char> keybind = get_button_keybind(pin);
    int keybind_length = keybind.size();
    std::string keybind_type = check_keybind_type(pin);

    if (keybind_length == 1) {
      switch(keybind_type) {
        case "MOUSE": mouse_press(pin);
        case "KEYBOARD": keyboard_press(pin);
      }
    }
    
    

  }

}







// MAIN LOOP
void loop()
{
  read_config(); // keep checking for updates
      
  // callibrate mouse
  PMW3360_DATA data = sensor.readBurst();
  if (data.isOnSurface && data.isMotion) {
    int mdx = constrain(data.dx, -127, 127);
    int mdy = constrain(data.dy, -127, 127);
    Mouse.move(mdx, mdy, 0);
  }
  
}
