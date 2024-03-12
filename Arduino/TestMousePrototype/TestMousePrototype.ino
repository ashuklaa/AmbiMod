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
#include <PMW3360.h>
#include <Adafruit_Fingerprint.h>
#include <Hashtable.h>
#include <EEPROM.h> // https://docs.arduino.cc/learn/built-in-libraries/eeprom/

#define is_pressed LOW

// ARDUINO PINS
#define FINGERPRINT_TXD 0
#define FINGERPRINT_RXD 1
#define MOUSE_4 2
#define MOUSE_5 3
#define TOP_LOAD 5
#define TOP_CLK 6
#define TOP_DATAIN 7
#define TOP_CLK_ENABLE 8
#define SENSOR_SS 10
#define MOBA_CLK_ENABLE 18
#define MOBA_DATAIN 19
#define MOBA_CLK 20
#define MOBA_LOAD 21

// CONFIG INDEX VALUES
#define M1 0
#define M2 1
#define M3 2
#define M4 3
#define M5 4
#define M6 5
#define M7 6
#define M8 7
#define M9 8
#define M10 9
#define M11 10
#define M12 11

#define EE_PROF_ADDR 0 // address of the profile in EEPROM


int mouse_keys[] = {MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE};

int keyboard_modifiers[] = {KEY_LEFT_CTRL, KEY_LEFT_SHIFT, KEY_LEFT_ALT, KEY_LEFT_GUI, KEY_RIGHT_CTRL, KEY_RIGHT_SHIFT, 
                      KEY_RIGHT_ALT, KEY_RIGHT_GUI, KEY_TAB, KEY_CAPS_LOCK, KEY_BACKSPACE, KEY_RETURN, KEY_MENU,
                      KEY_INSERT, KEY_DELETE, KEY_HOME, KEY_END, KEY_PAGE_UP, KEY_PAGE_DOWN, KEY_UP_ARROW, 
                      KEY_DOWN_ARROW, KEY_LEFT_ARROW, KEY_RIGHT_ARROW, KEY_NUM_LOCK, KEY_KP_SLASH, KEY_KP_ASTERISK,
                      KEY_KP_MINUS, KEY_KP_PLUS, KEY_KP_ENTER, KEY_KP_1, KEY_KP_2, KEY_KP_3, KEY_KP_4, KEY_KP_5,
                      KEY_KP_6, KEY_KP_7, KEY_KP_8, KEY_KP_9, KEY_KP_0, KEY_KP_DOT, KEY_ESC, KEY_F1, KEY_F2,
                      KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12, KEY_F13,
                      KEY_F14, KEY_F15, KEY_F16, KEY_F17, KEY_F18, KEY_F19, KEY_F20, KEY_F21, KEY_F22, KEY_F23,
                      KEY_F24, KEY_PRINT_SCREEN, KEY_SCROLL_LOCK, KEY_PAUSE};
int keyboard_alpha[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
                        'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
int keyboard_num[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
int keyboard_symbols_ns[] = {'`', '-', '=', '[', ']', '\\', ';', '\'', ',', '.', '/'};
int keyboard_symbols_s[] = {'~', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '{', '}', '|', ':', '\"', '<', '>', '?'};

Hashtable<int, String> mouse_keyboard_keys;

// NOTE: ALL BUTTONS WILL BECOME uint8_t DATA TYPE 

// MOUSE BUTTON VALUES -- byte values in order HGFEDCBA
  //      TOP BOARD     ||     MOBA
      // A : DPI          : M6
      // B : M3           : M9
      // C : SCROLL_A     : M12
      // D : SCROLL_B     : M11
      // E : M1           : M8
      // F :              : M10
      // G :              : 
      // H : M2           : M7
    // primary -- TOP
const int LEFT_CLICK = byte(00010000);     // 00010000 
const int RIGHT_CLICK = byte(10000000);    // 10000000
const int MIDDLE_CLICK = byte(00000010);   // 00000010
    // scrolling https://forum.arduino.cc/t/mouse-scroll-wheel-encoder-to-arduino/68935
const int SCROLL_UP = byte(00000100);      // 00000100
const int SCROLL_DOWN = byte(00001000);    // 00001000
    // side panel - MOBA panel
const int MOUSE_6 = byte(00000001);        // 00000001
const int MOUSE_7 = byte(10000000);        // 10000000
const int MOUSE_8 = byte(00010000);        // 00010000
const int MOUSE_9 = byte(00000010);        // 00000010
const int MOUSE_10 = byte(00100000);       // 00100000
const int MOUSE_11 = byte(00001000);       // 00001000
const int MOUSE_12 = byte(00000100);       // 00000100
const int DPI = byte(00000001);            // 00000001
    // side panel - fingerprint
// const int FINGERPRINT_SENSOR = 15;

// PMW3360 Sensor :: https://github.com/SunjunKim/PMW3360/tree/master
PMW3360 sensor;
//Adafruit_Fingerprint finger = Adafruit_Fingerprint();

// stores the current profile in the mouse
String PROFILE = "DEFAULT";

// CONFIG DATA
int dpi_values[] = {800, 1200, 1600, 2400, 5000}; // default values
int current_dpi = 800; // default dpi
  // default button values
    // int = pin number
    // char = const global
    // acii table = https://www.commfront.com/pages/ascii-chart#:~:text=ASCII%20stands%20for%20American%20Standard,codes%20and%20Extended%20ASCII%20codes.

int button_set[] = {LEFT_CLICK, RIGHT_CLICK, MIDDLE_CLICK, MOUSE_4, MOUSE_5, MOUSE_6, MOUSE_7, MOUSE_8, MOUSE_9, MOUSE_10, MOUSE_11, MOUSE_12};

SimpleVector<int> L_CLICK_BIND;
SimpleVector<int> R_CLICK_BIND;
SimpleVector<int> MID_CLICK_BIND;
SimpleVector<int> M4_CLICK_BIND;
SimpleVector<int> M5_CLICK_BIND;
SimpleVector<int> M6_CLICK_BIND;
SimpleVector<int> M7_CLICK_BIND;
SimpleVector<int> M8_CLICK_BIND;
SimpleVector<int> M9_CLICK_BIND;
SimpleVector<int> M10_CLICK_BIND;
SimpleVector<int> M11_CLICK_BIND;
SimpleVector<int> M12_CLICK_BIND;

SimpleVector<SimpleVector<int>> button_binds;

// FOR STORED PROFILE OF EACH BUTTON
// first int is the int of the pin, second int is the index for the vector of the in button_binds
Hashtable<int, int> button_map;

// FOR CONFIG CONVERSIONS
Hashtable<String, int> config_string_to_ascii;
Hashtable<int, String> config_ascii_to_string;
Hashtable<String, int> button_str_to_pin;

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

// NOTE: NEED TO UPDATE
void setup()
{
  //  PIN SET UP
  pinMode(LEFT_CLICK, INPUT_PULLUP);
  pinMode(RIGHT_CLICK, INPUT_PULLUP);
  pinMode(SCROLL_UP, INPUT_PULLUP);
  pinMode(SCROLL_DOWN, INPUT_PULLUP);
  pinMode(MIDDLE_CLICK, INPUT_PULLUP);
  // pinMode(FINGERPRINT_SENSOR, INPUT_PULLUP);
  pinMode(MOUSE_4, INPUT_PULLUP);
  pinMode(MOUSE_5, INPUT_PULLUP);
  pinMode(MOUSE_6, INPUT_PULLUP);
  pinMode(MOUSE_7, INPUT_PULLUP);
  pinMode(MOUSE_8, INPUT_PULLUP);
  pinMode(MOUSE_9, INPUT_PULLUP);
  pinMode(MOUSE_10, INPUT_PULLUP);
  pinMode(MOUSE_11, INPUT_PULLUP);
  pinMode(MOUSE_12, INPUT_PULLUP);

  // begin mouse processes
  Serial.begin(9600);
  sensor.setCPI(current_dpi);
  //fingerprint.begin(57600);
  Keyboard.begin();
  Mouse.begin();

  // INITIALIZE BUTTON BIND VECTORS W/ DEFAULTS
  L_CLICK_BIND.put(MOUSE_LEFT);
  R_CLICK_BIND.put(MOUSE_RIGHT);
  MID_CLICK_BIND.put(MOUSE_MIDDLE);

  M4_CLICK_BIND.put(KEY_LEFT_ALT);
  M4_CLICK_BIND.put(KEY_RIGHT_ARROW);

  M5_CLICK_BIND.put(KEY_LEFT_ALT);
  M5_CLICK_BIND.put(KEY_LEFT_ARROW);

  M6_CLICK_BIND.put('I');
  M7_CLICK_BIND.put('n');
  M8_CLICK_BIND.put('a');
  M9_CLICK_BIND.put('\'');
  M10_CLICK_BIND.put('n');
  M11_CLICK_BIND.put('i');
  M12_CLICK_BIND.put('s');

  // put vectors into button binds
  button_binds.put(L_CLICK_BIND);
  button_binds.put(R_CLICK_BIND);
  button_binds.put(MID_CLICK_BIND);
  button_binds.put(M4_CLICK_BIND);
  button_binds.put(M5_CLICK_BIND);
  button_binds.put(M6_CLICK_BIND);
  button_binds.put(M7_CLICK_BIND);
  button_binds.put(M8_CLICK_BIND);
  button_binds.put(M9_CLICK_BIND);
  button_binds.put(M10_CLICK_BIND);
  button_binds.put(M11_CLICK_BIND);
  button_binds.put(M12_CLICK_BIND);


  // INITIALIZE HASHTABLE FOR GENERAL BUTTON CHARS
    // add mouse keys to hashtable
  for(int i = 0; i < sizeof(mouse_keys); i++){
    mouse_keyboard_keys.put(mouse_keys[i], "MOUSE");
  }
    // add keyboard alphas // upper & lower case
  for(int i = 0; i < sizeof(keyboard_alpha); i++){
    mouse_keyboard_keys.put(keyboard_alpha[i], "KEYBOARD");
    mouse_keyboard_keys.put(tolower(keyboard_alpha[i]), "KEYBOARD");
  }
    // add keyboard numbers
  for(int i = 0; i < sizeof(keyboard_num); i++) {
    mouse_keyboard_keys.put(keyboard_num[i], "KEYBOARD");
  }
    // add keyboard symbols (no shift)
  for(int i = 0; i < sizeof(keyboard_symbols_ns); i++) {
    mouse_keyboard_keys.put(keyboard_symbols_ns[i], "KEYBOARD");
  }
    // add keyboard modifiers specific to Keyboard.h
  for (int i = 0; i < sizeof(keyboard_modifiers); i++) {
    mouse_keyboard_keys.put(keyboard_modifiers[i], "KEYBOARD");  
  }
    // add keyboard symbols (shift)
  for(int i = 0; i < sizeof(keyboard_symbols_s); i++) {
    mouse_keyboard_keys.put(keyboard_symbols_s[i], "KEYBOARD");
  }    

  // INITIALIZE HASHTABLE FOR STRING TO CHAR CONVERSIONS
  config_string_to_ascii.put("MOUSE_LEFT", MOUSE_LEFT);
  config_string_to_ascii.put("MOUSE_RIGHT", MOUSE_RIGHT);
  config_string_to_ascii.put("MOUSE_MIDDLE", MOUSE_MIDDLE);
    // KEYBOARD MODIFIERS
  config_string_to_ascii.put("KEY_LEFT_CTRL", KEY_LEFT_CTRL);
  config_string_to_ascii.put("KEY_LEFT_SHIFT", KEY_LEFT_SHIFT);
  config_string_to_ascii.put("KEY_LEFT_ALT", KEY_LEFT_ALT);
  config_string_to_ascii.put("KEY_LEFT_GUI", KEY_LEFT_GUI);
  config_string_to_ascii.put("KEY_RIGHT_CTRL", KEY_RIGHT_CTRL);
  config_string_to_ascii.put("KEY_RIGHT_SHIFT", KEY_RIGHT_SHIFT);
  config_string_to_ascii.put("KEY_RIGHT_ALT", KEY_RIGHT_ALT);
  config_string_to_ascii.put("KEY_RIGHT_GUI", KEY_RIGHT_GUI);
    // KEYBOARD - SPECIAL KEYS
  config_string_to_ascii.put("KEY_TAB", KEY_TAB);
  config_string_to_ascii.put("KEY_CAPS_LOCK", KEY_CAPS_LOCK);
  config_string_to_ascii.put("KEY_BACKSPACE", KEY_BACKSPACE);
  config_string_to_ascii.put("KEY_RETURN", KEY_RETURN);
  config_string_to_ascii.put("KEY_MENU", KEY_MENU);
    // KEYBOARD - NAVIGATION CLUSTER
  config_string_to_ascii.put("KEY_INSERT", KEY_INSERT);
  config_string_to_ascii.put("KEY_DELETE", KEY_DELETE);
  config_string_to_ascii.put("KEY_HOME", KEY_HOME);
  config_string_to_ascii.put("KEY_END", KEY_END);
  config_string_to_ascii.put("KEY_PAGE_UP", KEY_PAGE_UP);
  config_string_to_ascii.put("KEY_PAGE_DOWN", KEY_PAGE_DOWN);
  config_string_to_ascii.put("KEY_UP_ARROW", KEY_UP_ARROW);
  config_string_to_ascii.put("KEY_DOWN_ARROW", KEY_DOWN_ARROW);
  config_string_to_ascii.put("KEY_LEFT_ARROW", KEY_LEFT_ARROW);
  config_string_to_ascii.put("KEY_RIGHT_ARROW", KEY_RIGHT_ARROW);
    // KEYBOARD - NUMERIC KEYPAD
  config_string_to_ascii.put("KEY_NUM_LOCK", KEY_NUM_LOCK);
  config_string_to_ascii.put("KEY_KP_SLASH", KEY_KP_SLASH);
  config_string_to_ascii.put("KEY_KP_ASTERISK", KEY_KP_ASTERISK);
  config_string_to_ascii.put("KEY_KP_MINUS", KEY_KP_MINUS);
  config_string_to_ascii.put("KEY_KP_PLUS", KEY_KP_PLUS);
  config_string_to_ascii.put("KEY_KP_ENTER", KEY_KP_ENTER);
  config_string_to_ascii.put("KEY_KP_1", KEY_KP_1);
  config_string_to_ascii.put("KEY_KP_2", KEY_KP_2);
  config_string_to_ascii.put("KEY_KP_3", KEY_KP_3);
  config_string_to_ascii.put("KEY_KP_4", KEY_KP_4);
  config_string_to_ascii.put("KEY_KP_5", KEY_KP_5);
  config_string_to_ascii.put("KEY_KP_6", KEY_KP_6);
  config_string_to_ascii.put("KEY_KP_7", KEY_KP_7);
  config_string_to_ascii.put("KEY_KP_8", KEY_KP_8);
  config_string_to_ascii.put("KEY_KP_9", KEY_KP_9);
  config_string_to_ascii.put("KEY_KP_0", KEY_KP_0);
  config_string_to_ascii.put("KEY_KP_DOT", KEY_KP_DOT);
  config_string_to_ascii.put("KEY_ESC", KEY_ESC);
  config_string_to_ascii.put("KEY_F1", KEY_F1);
  config_string_to_ascii.put("KEY_F2", KEY_F2);
  config_string_to_ascii.put("KEY_F3", KEY_F3);
  config_string_to_ascii.put("KEY_F4", KEY_F4);
  config_string_to_ascii.put("KEY_F5", KEY_F5);
  config_string_to_ascii.put("KEY_F6", KEY_F6);
  config_string_to_ascii.put("KEY_F7", KEY_F7);
  config_string_to_ascii.put("KEY_F8", KEY_F8);
  config_string_to_ascii.put("KEY_F9", KEY_F9);
  config_string_to_ascii.put("KEY_F10", KEY_F10);
  config_string_to_ascii.put("KEY_F11", KEY_F11);
  config_string_to_ascii.put("KEY_F12", KEY_F12);
  config_string_to_ascii.put("KEY_F13", KEY_F13);
  config_string_to_ascii.put("KEY_F14", KEY_F14);
  config_string_to_ascii.put("KEY_F15", KEY_F15);
  config_string_to_ascii.put("KEY_F16", KEY_F16);
  config_string_to_ascii.put("KEY_F17", KEY_F17);
  config_string_to_ascii.put("KEY_F18", KEY_F18);
  config_string_to_ascii.put("KEY_F19", KEY_F19);
  config_string_to_ascii.put("KEY_F20", KEY_F20);
  config_string_to_ascii.put("KEY_F21", KEY_F21);
  config_string_to_ascii.put("KEY_F22", KEY_F22);
  config_string_to_ascii.put("KEY_F23", KEY_F23);
  config_string_to_ascii.put("KEY_F24", KEY_F24);
  config_string_to_ascii.put("KEY_F24", KEY_F24);
    // KEYBOARD - FUNCTION CONTROL KEYS
  config_string_to_ascii.put("KEY_PRINT_SCREEN", KEY_PRINT_SCREEN);
  config_string_to_ascii.put("KEY_SCROLL_LOCK", KEY_SCROLL_LOCK);
  config_string_to_ascii.put("KEY_PAUSE", KEY_PAUSE);
  


  // INITIALIZE HASHTABLE FOR CHAR TO STRING CONVERSIONS
  config_ascii_to_string.put(MOUSE_LEFT, "MOUSE_LEFT");
  config_ascii_to_string.put(MOUSE_RIGHT, "MOUSE_RIGHT");
  config_ascii_to_string.put(MOUSE_MIDDLE, "MOUSE_MIDDLE");
    // KEYBOARD MODIFIERS
  config_ascii_to_string.put(KEY_LEFT_CTRL, "KEY_LEFT_CTRL");
  config_ascii_to_string.put(KEY_LEFT_SHIFT, "KEY_LEFT_SHIFT");
  config_ascii_to_string.put(KEY_LEFT_ALT, "KEY_LEFT_ALT");
  config_ascii_to_string.put(KEY_LEFT_GUI, "KEY_LEFT_GUI");
  config_ascii_to_string.put(KEY_RIGHT_CTRL, "KEY_RIGHT_CTRL");
  config_ascii_to_string.put(KEY_RIGHT_SHIFT, "KEY_RIGHT_SHIFT");
  config_ascii_to_string.put(KEY_RIGHT_ALT, "KEY_RIGHT_ALT");
  config_ascii_to_string.put(KEY_RIGHT_GUI, "KEY_RIGHT_GUI");
    // KEYBOARD - SPECIAL KEYS
  config_ascii_to_string.put(KEY_TAB, "KEY_TAB");
  config_ascii_to_string.put(KEY_CAPS_LOCK, "KEY_CAPS_LOCK");
  config_ascii_to_string.put(KEY_BACKSPACE, "KEY_BACKSPACE");
  config_ascii_to_string.put(KEY_RETURN, "KEY_RETURN");
  config_ascii_to_string.put(KEY_MENU, "KEY_MENU");
    // KEYBOARD - NAVIGATION CLUSTER
  config_ascii_to_string.put(KEY_INSERT, "KEY_INSERT");
  config_ascii_to_string.put(KEY_DELETE, "KEY_DELETE");
  config_ascii_to_string.put(KEY_HOME, "KEY_HOME");
  config_ascii_to_string.put(KEY_END, "KEY_END");
  config_ascii_to_string.put(KEY_PAGE_UP, "KEY_PAGE_UP");
  config_ascii_to_string.put(KEY_PAGE_DOWN, "KEY_PAGE_DOWN");
  config_ascii_to_string.put(KEY_UP_ARROW, "KEY_UP_ARROW");
  config_ascii_to_string.put(KEY_DOWN_ARROW, "KEY_DOWN_ARROW");
  config_ascii_to_string.put(KEY_LEFT_ARROW, "KEY_LEFT_ARROW");
  config_ascii_to_string.put(KEY_RIGHT_ARROW, "KEY_RIGHT_ARROW");
    // KEYBOARD - NUMERIC KEYPAD
  config_ascii_to_string.put(KEY_NUM_LOCK, "KEY_NUM_LOCK");
  config_ascii_to_string.put(KEY_KP_SLASH, "KEY_KP_SLASH");
  config_ascii_to_string.put(KEY_KP_ASTERISK, "KEY_KP_ASTERISK");
  config_ascii_to_string.put(KEY_KP_MINUS, "KEY_KP_MINUS");
  config_ascii_to_string.put(KEY_KP_PLUS, "KEY_KP_PLUS");
  config_ascii_to_string.put(KEY_KP_ENTER, "KEY_KP_ENTER");
  config_ascii_to_string.put(KEY_KP_1, "KEY_KP_1");
  config_ascii_to_string.put(KEY_KP_2, "KEY_KP_2");
  config_ascii_to_string.put(KEY_KP_3, "KEY_KP_3");
  config_ascii_to_string.put(KEY_KP_4, "KEY_KP_4");
  config_ascii_to_string.put(KEY_KP_5, "KEY_KP_5");
  config_ascii_to_string.put(KEY_KP_6, "KEY_KP_6");
  config_ascii_to_string.put(KEY_KP_7, "KEY_KP_7");
  config_ascii_to_string.put(KEY_KP_8, "KEY_KP_8");
  config_ascii_to_string.put(KEY_KP_9, "KEY_KP_9");
  config_ascii_to_string.put(KEY_KP_0, "KEY_KP_0");
  config_ascii_to_string.put(KEY_KP_DOT, "KEY_KP_DOT");
    // FUNCTION KEYS
  config_ascii_to_string.put(KEY_ESC, "KEY_ESC");
  config_ascii_to_string.put(KEY_F1, "KEY_F1");
  config_ascii_to_string.put(KEY_F2, "KEY_F2");
  config_ascii_to_string.put(KEY_F3, "KEY_F3");
  config_ascii_to_string.put(KEY_F4, "KEY_F4");
  config_ascii_to_string.put(KEY_F5, "KEY_F5");
  config_ascii_to_string.put(KEY_F6, "KEY_F6");
  config_ascii_to_string.put(KEY_F7, "KEY_F7");


  // INITIALIZE HASHTABLE FOR STRING BUTTON NAMES TO PINS
  button_str_to_pin.put("LEFT_CLICK", LEFT_CLICK);
  button_str_to_pin.put("RIGHT_CLICK", RIGHT_CLICK);
  button_str_to_pin.put("MIDDLE_CLICK", MIDDLE_CLICK);
  button_str_to_pin.put("MOUSE_4", MOUSE_4);
  button_str_to_pin.put("MOUSE_5", MOUSE_5);
  button_str_to_pin.put("MOUSE_6", MOUSE_6);
  button_str_to_pin.put("MOUSE_7", MOUSE_7);
  button_str_to_pin.put("MOUSE_8", MOUSE_8);
  button_str_to_pin.put("MOUSE_9", MOUSE_9);
  button_str_to_pin.put("MOUSE_10", MOUSE_10);
  button_str_to_pin.put("MOUSE_11", MOUSE_11);
  button_str_to_pin.put("MOUSE_12", MOUSE_12);



  // INITIALIZE HASHTABLE FOR KEYBIND INDEX
  for (int i = 0; i < sizeof(button_set); i++) {
    button_map.put(button_set[i], i);
  }

}

// split string by a given delimiter, excludes the delimiter
SimpleVector<String> split(String str, char delimiter) {
  
  SimpleVector<String> config;

  String temp = "";

  // iterate through each character in the string
  for (int i = 0; i < str.length(); i++) {
    // when delimiter is found, reset temp string and add previous temp value to config vector
    if (str[i] == delimiter) {
      config.put(temp);
      temp = "";
    } 
    else { // add character to temp string
      temp += str[i];
    }
  }

  return config;
}

// CONFIG RETURN HELPERS
  // FOR KEYBINDS/CONFIGS CONTAINING MORE THAN ONE ELEMENT
String keybind_to_string(int button, SimpleVector<int> keybinds) {

  String str_keybind = "";

  SimpleVector<int> keybind = button_binds[button_map.getElement(button)]; // get the value using the button as a key

  for (int i = 0; i < keybind.size(); i++) {
    str_keybind += config_ascii_to_string.getElement(keybind[i]);
    if (i != keybind.size()-1) {
      str_keybind += "+";
    }
  }
  
  return str_keybind;
}

// BUTTON READ HELPERS
// helper function for button reads
SimpleVector<int> get_button_keybind(int pin) {

  return button_binds[button_map.getElement(pin)];

}

// returns if the keybind is a mouse or keyboard operation
String check_keybind_type(int pin) {

  return mouse_keyboard_keys.getElement(pin); // 0 = mouse, 1 = keyboard

}

void update_profile(const String name) {

  byte str_len = name.length();
  EEPROM.update(EE_PROF_ADDR, str_len);

  for (int i = 0; i < str_len; i++) {
    EEPROM.update(EE_PROF_ADDR + 1 + i, name[i]);
  }

}

String get_profile() {

  int str_len = EEPROM.read(EE_PROF_ADDR);
  char data[str_len+1];

  for (int i = 0; i < str_len; i++) {
    data[i] = EEPROM.read(EE_PROF_ADDR + 1 + i);
  }

  data[str_len] = '\0';

  return String(data);

}

// CONFIG READ / RETURN
// loads configuration into the firmware
void read_config() {
  if (Serial.available() > 0) {
    String config = Serial.readString(); // data recieved from the companion program

    // check to verify serial string recieved is from the correct program
    if (config == 'Good Morning') {
      Serial.println('Gamer'); // send signal back to application

      char config_delimiter = ';';
      char button_delimiter = ':';

      config = Serial.readString(); // read again for actual mouse configuration
      SimpleVector<String> config_read = split(config, config_delimiter); // stores each string for each individual button/config into one element in a vector

      for (int i = 0; i < config_read.size(); i++) {

        SimpleVector<String> button_keybind = split(config_read[i], button_delimiter);

        // Update Hashtable
        if (button_keybind[0] == "PROFILE") {

          PROFILE = button_keybind[1]; // second value stored in vector should be the name of the profile
          update_profile(PROFILE);

        } else if (button_keybind[0] == "DPI") {
          
          current_dpi = button_keybind[1].toInt(); // converts the string in the vector to an int

        } else if (button_keybind[0] == "DPI_SET") {

          SimpleVector<String> temp_dpi = split(button_keybind[1], "+"); // split dpi string into vector

          for (int i = 0; i < sizeof(dpi_values); i++){  // overwrite values into stored config vector
            dpi_values[i] = temp_dpi[i].toInt();
          }

        } else if (button_keybind[0] == "DEBOUNCE") {

          DEBOUNCE = button_keybind[1].toInt();

        } else { // for all the buttons

          int designated_button = button_str_to_pin.getElement(button_keybind[0]);
          SimpleVector<String> temp_button_str = split(button_keybind[1], "+");
          SimpleVector<char> temp_keybinds;

          for (int i = 0; i < temp_button_str.size(); i++) { // iterates through each vector element adding it to a temp keybind set
            if (config_string_to_ascii.exists(temp_button_str[i])) {
            temp_keybinds.put(config_string_to_ascii.getElement(temp_button_str[i]));
            } else {
              temp_keybinds.put(&temp_button_str[i]);
            }
          }

          button_binds[button_map.getElement(designated_button)] = temp_keybinds;

        }
        
      }
    }
  }
}

void send_config() {

  Serial.println("Chat");

  String current_config = "";
  
  // PROFILE
  current_config += "PROFILE:" + update_profile() + ";";

  // DPI
  current_config += "DPI:" + String(current_dpi) + ";";

  // DPI_SET
  current_config += "DPI_SET:"; 
  for(int i = 0; i < sizeof(dpi_values); i++) {
    current_config += String(dpi_values[i]);
    if (i != sizeof(dpi_values)-1) {
      current_config += "+";
    }
  }
  current_config += ";";

  current_config += "DEBOUNCE:" + String(DEBOUNCE) + ";";

  // LEFT_CLICK
  current_config += "LEFT_CLICK:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(LEFT_CLICK, button_binds[button_map.getElement(LEFT_CLICK)]);
  }
  current_config += ";";

  current_config += "RIGHT_CLICK:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(RIGHT_CLICK, button_binds[button_map.getElement(RIGHT_CLICK)]);
  }
  current_config += ";";

  current_config += "MIDDLE_CLICK:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MIDDLE_CLICK, button_binds[button_map.getElement(MIDDLE_CLICK)]);
  }
  current_config += ";";

  current_config += "MOUSE_4:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_4, button_binds[button_map.getElement(MOUSE_4)]);
  }
  current_config += ";";

  current_config += "MOUSE_5:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_5, button_binds[button_map.getElement(MOUSE_5)]);
  }
  current_config += ";";    

  current_config += "MOUSE_6:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_6, button_binds[button_map.getElement(MOUSE_6)]);
  }
  current_config += ";";

  current_config += "MOUSE_7:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_7, button_binds[button_map.getElement(MOUSE_7)]);
  }
  current_config += ";";

  current_config += "MOUSE_8:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_8, button_binds[button_map.getElement(MOUSE_8)]);
  }
  current_config += ";";    

  current_config += "MOUSE_9:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_9, button_binds[button_map.getElement(MOUSE_9)]);
  }
  current_config += ";";

  current_config += "MOUSE_10:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_10, button_binds[button_map.getElement(MOUSE_10)]);
  }
  current_config += ";";

  current_config += "MOUSE_11:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_11, button_binds[button_map.getElement(MOUSE_11)]);
  }
  current_config += ";";   

  current_config += "MOUSE_12:";
  for(int i = 0; i < button_binds[0].size(); i++) {
    current_config += keybind_to_string(MOUSE_12, button_binds[button_map.getElement(MOUSE_12)]);
  }
  current_config += ";";

  Serial.println(current_config);

}

void send_profile() {

  Serial.println('World');
  Serial.println(EEPROM.read(0));
  
}




// BUTTON HANDLERS

void mouse_press(int pin) {



}

void keyboard_press(int pin) {



}

void macro_press(int type, int pin) {



}

// void button_press(int pin) {

//   if (pin == is_pressed) {

//     SimpleVector<char> keybind = get_button_keybind(pin);
//     int keybind_length = keybind.size();
//     String keybind_type = check_keybind_type(pin);

//     if (keybind_length == 1) {
//       switch(keybind_type) {
//         case "MOUSE": mouse_press(pin);
//         case "KEYBOARD": keyboard_press(pin);
//       }
//     }
    
    

//   }

// }







// MAIN LOOP
void loop()
{
  // check for communication with companion app
  if (Serial.available() > 0) {

    String companion_read = Serial.readString();

    if (companion_read == "Hello") { // sends "World"
      send_profile();
    }
    else if (companion_read == "Good Morning") { // sends "Gamer"
      read_config();
    }
    else if (companion_read == "Good Night") { // sends "Chat"
      send_config();
    }

  }
      
  // callibrate mouse
  PMW3360_DATA data = sensor.readBurst();
  if (data.isOnSurface && data.isMotion) {
    int mdx = constrain(data.dx, -127, 127);
    int mdy = constrain(data.dy, -127, 127);
    Mouse.move(mdx, mdy, 0);
  }
  
}
