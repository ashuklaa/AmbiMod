#include <Keyboard.h>
#include <Mouse.h>
#include <PMW3360.h>
#include <EEPROM.h> // https://docs.arduino.cc/learn/built-in-libraries/eeprom/

// native constants
#define is_pressed LOW
#define MAX_MACRO 3
#define MAX_DPI_SET 5
#define MAX_CONFIG_SENT 16
#define KEYBOARD_PRESS_MOD "0x"
#define KEYBOARD_PRESS '_'
#define CONFIG_DELIMITER ';'
#define BUTTON_DELIMITER ':'
#define MACRO_DELIMITER '+'


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

// INCOMING STRING -- NUMKEY:BIND;

// INCOMING STRINGS
const String RECEIVE_CONNECT_IDENT = "Hello";
const String RECEIVE_SCONFIG_IDENT = "Good Night";
const String RECEIVE_UCONFIG_IDENT = "Good Morning";


// OUTGOING STRINGS
const String SEND_CONNECT_IDENT = "World";
const String SEND_SCONFIG_IDENT = "Chat";
const String SEND_UCONFIG_IDENT = "Gamer";

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
const uint8_t LEFT_CLICK = byte(00010000);     // 00010000 
const uint8_t RIGHT_CLICK = byte(10000000);    // 10000000
const uint8_t MIDDLE_CLICK = byte(00000010);   // 00000010
    // scrolling https://forum.arduino.cc/t/mouse-scroll-wheel-encoder-to-arduino/68935
const uint8_t SCROLL_UP = byte(00000100);      // 00000100
const uint8_t SCROLL_DOWN = byte(00001000);    // 00001000
    // side panel - MOBA panel
const uint8_t MOUSE_6 = byte(00000001);        // 00000001
const uint8_t MOUSE_7 = byte(10000000);        // 10000000
const uint8_t MOUSE_8 = byte(00010000);        // 00010000
const uint8_t MOUSE_9 = byte(00000010);        // 00000010
const uint8_t MOUSE_10 = byte(00100000);       // 00100000
const uint8_t MOUSE_11 = byte(00001000);       // 00001000
const uint8_t MOUSE_12 = byte(00000100);       // 00000100
const uint8_t DPI = byte(00000001);            // 00000001

// PMW3360 Sensor :: https://github.com/SunjunKim/PMW3360/tree/master
PMW3360 sensor;

// CONFIG DATA - defaults
    // DPI
int DPI_VALUES[] = {800, 1200, 1600, 2400, 5000}; // default values
int CURRENT_DPI = 800; // default dpi
int DPI_INDEX = 0;

    // PROFILE
String PROFILE = "DEFAULT";

    // STORED KEYBINDS
String L_CLICK_BIND[MAX_MACRO] = {"1"};
String R_CLICK_BIND[MAX_MACRO] = {"2"};
String MID_CLICK_BIND[MAX_MACRO] = {"3"};
String M4_CLICK_BIND[MAX_MACRO] = {"0x82", "0xD8"};
String M5_CLICK_BIND[MAX_MACRO] = {"0x82", "0xD7"};
String M6_CLICK_BIND[MAX_MACRO] = {"_I"};
String M7_CLICK_BIND[MAX_MACRO] = {"_n"};
String M8_CLICK_BIND[MAX_MACRO] = {"_a"};
String M9_CLICK_BIND[MAX_MACRO] = {"_'"};
String M10_CLICK_BIND[MAX_MACRO] = {"_n"};
String M11_CLICK_BIND[MAX_MACRO] = {"_i"};
String M12_CLICK_BIND[MAX_MACRO] = {"_s"};

// CONFIG READ HELPERS
    // split string by a given delimiter, excludes the delimiter
String[] split(String str, char delimiter, int num_elements) {
  
  String config[num_elements];
  String temp = "";

  // iterate through each character in the string
  for (int i = 0; i < str.length(); i++) {
    // when delimiter is found, reset temp string and add previous temp value to config vector
    if (str[i] == delimiter) {
      config[i] = temp;
      temp = "";
    } 
    else { // add character to temp string
      temp += str[i];
    }
  }

  return config;

}

// SERIAL READ/SEND CONFIG
void update_config() {
    
    Serial.println(SEND_UCONFIG_IDENT);

    config = Serial.readString();

    String[] config_read = split(config, CONFIG_DELIMITER, MAX_CONFIG_SENT);

    for (int i = 0; i < sizeof(config_read); i++) {

        String[] button_keybind = split(config_read[i], BUTTON_DELIMITER);

        if (button_keybind[0] == "PROFILE") {
            
            PROFILE = button_keybind[1];
            update_profile(PROFILE);

        } else if (button_keybind[0] == "DPI") {

            CURRENT_DPI = button_keybind[1].toInt();

        } else if (button_keybind[0] == "DPI_SET") {

            String[] temp_dpi = split(button_keybind[0], MACRO_DELIMITER, MAX_DPI_SET);

            for (int i = 0; i < sizeof(DPI_VALUES); i++) {
                dpi_values[i] = temp_dpi[i].toInt();
            }

        } else if (button_keybind[0] == "DEBOUNCE") {

            DEBOUNCE = button_keybind[1].toInt();

        } else {

            int designated_button = button_keybind[0].toInt();
            String[] temp_button_str = split(button_keybind[1], MACRO_DELIMITER, MAX_MACRO);

            switch(designated_button) {
                case M1: L_CLICK_BIND = temp_button_str;
                case M2: R_CLICK_BIND = temp_button_str;
                case M3: MID_CLICK_BIND = temp_button_str;
                case M4: M4_CLICK_BIND = temp_button_str;
                case M5: M5_CLICK_BIND = temp_button_str;
                case M6: M6_CLICK_BIND = temp_button_str;
                case M7: M7_CLICK_BIND = temp_button_str;
                case M8: M8_CLICK_BIND = temp_button_str;
                case M9: M9_CLICK_BIND = temp_button_str;
                case M10: M10_CLICK_BIND = temp_button_str;
                case M11: M11_CLICK_BIND = temp_button_str;
                case M12: M12_CLICK_BIND = temp_button_str;
            }

        }

    }

}


void send_config() {
    
    Serial.println(SEND_SCONFIG_IDENT);

    String current_config = "";
  
    // PROFILE
    current_config += "PROFILE:" + read_profile() + ";";

    // DPI
    current_config += "DPI:" + String(CURRENT_DPI) + ";";

    // LEFT
    current_config += "LEFT_CLICK:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += L_CLICK_BIND[i];
    }
    current_config += ";";

    // RIGHT
    current_config += "RIGHT_CLICK:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += R_CLICK_BIND[i];
    }
    current_config += ";";

    // MIDDLE
    current_config += "MIDDLE_CLICK:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += MID_CLICK_BIND[i];
    }
    current_config += ";";

    // M4
    current_config += "MOUSE_4:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M4_CLICK_BIND[i];
    }
    current_config += ";";

    // M5
    current_config += "MOUSE_5:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M5_CLICK_BIND[i];
    }
    current_config += ";";    

    // M6
    current_config += "MOUSE_6:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M6_CLICK_BIND[i];
    }
    current_config += ";";

    // M7
    current_config += "MOUSE_7:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M7_CLICK_BIND[i];
    }
    current_config += ";";

    // M8
    current_config += "MOUSE_8:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M8_CLICK_BIND[i];
    }
    current_config += ";";    

    // M9
    current_config += "MOUSE_9:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M9_CLICK_BIND[i];
    }
    current_config += ";";

    // M10
    current_config += "MOUSE_10:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M10_CLICK_BIND[i];
    }
    current_config += ";";

    // M11
    current_config += "MOUSE_11:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M11_CLICK_BIND[i];
    }
    current_config += ";";   

    // M12
    current_config += "MOUSE_12:";
    for(int i = 0; i < MAX_MACRO; i++) {
        current_config += M12_CLICK_BIND[i];
    }
    current_config += ";";

    Serial.println(current_config);

}


String* str_to_bindArr(String button) {

    switch(button.toInt()) {
        case M1: return L_CLICK_BIND;
        case M2: return R_CLICK_BIND;
        case M3: return MID_CLICK_BIND;
        case M4: return M4_CLICK_BIND;
        case M5: return M5_CLICK_BIND;
        case M6: return M6_CLICK_BIND;
        case M7: return M7_CLICK_BIND;
        case M8: return M8_CLICK_BIND;
        case M9: return M9_CLICK_BIND;
        case M10: return M10_CLICK_BIND;
        case M11: return M11_CLICK_BIND;
        case M12: return M12_CLICK_BIND;
        default: return nullptr;
    }

}

// PROFILE HANDLERS
void send_profile() {

    Serial.println('World');
    Serial.println(EEPROM.read(0));

}

    // EEPROM
void update_profile() {

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

// BUTTON HANDLERS

uint8_t Read_Shift_Register(int load, int clock, int clockEnable, int dataIn) {

  uint8_t data = 0;

  // PL pulse
  digitalWrite(load, LOW);  // turn PL low to load inputs
  delayMicroseconds(5);
  digitalWrite(load, HIGH); // turn PL high so the CP can pulse
  delayMicroseconds(5);
  
  // push data to shift registers
  digitalWrite(clock, HIGH);  // pulse CP with CE low to push inputs
  digitalWrite(clockEnable, LOW);
  data = shiftIn(dataIn, clock, LSBFIRST); // shift in data
  digitalWrite(clockEnable, HIGH);  // end of CP pulse

  return data;

}

void Handle_Pins() {

  // 255 means "no button pressed"
  uint8_t returnByte = 255;

  // TOP
  returnByte = Read_Shift_Register(TOP_LOAD, TOP_CLK, TOP_CLK_ENABLE, TOP_DATAIN);

  // M4
  if (returnByte == 0 && digitalRead(MOUSE_4) == is_pressed) {
    returnByte = String(2);
  }

  // M5
  if (returnByte == 0 && digitalRead(MOUSE_5) == is_pressed) {
    returnByte = String(3);
  }

  // MOBA
  if (returnByte == 0) {
    returnByte = ~(Read_Shift_Register(MOBA_LOAD, MOBA_CLK, MOBA_CLK_ENABLE, MOBA_DATAIN));
  }

  // convert to binary notation
  String returnString = "";

  for (int i = 7; i >= 0; i--) {
    returnString += String(bitRead(returnByte, i));
  }

  // call advait
  byte_btnPress(returnString);

}

void byte_btnPress(String recvdBtn){

  //call ellie's function to receive correct array of binds for key pressed
  String* bindArray = button_in_to_arr(recvdBtn);

  for(int i = 0; i <= sizeof(bindArray); i++){
    Serial.println(bindArray[i]);

    //if value is null, release all keys and break for loop to stop reading array
    if (bindArray[i] == nullptr){
      //Serial.println("I'M ABOUT TO SHIT MYSELF");
      Mouse.release(MOUSE_LEFT); 
      Mouse.release(MOUSE_RIGHT); 
      Mouse.release(MOUSE_MIDDLE);
      Keyboard.releaseAll();
      delay(debounceConstant);
      break;
    }


    if (bindArray[i].startsWith("0x")){ //keyboard press identified
      int key = strtol(bindArray[i].c_str(), NULL, 16);
      Keyboard.press(key);
      
    } else if (bindArray[i].startsWith("_")){
      int key = bindArray[i][1];
      Keyboard.press(key);
    } else{ //mouse press identified
      int mousepress = atoi(bindArray[i].c_str());
      Mouse.press(mousepress);
    }

  }

}

void dpi_press() {

    DPI_INDEX += 1;

    if (DPI_INDEX == 5) {
        DPI_INDEX = 0;
    }

    sensor.setCPI(DPI_VALUES[DPI_INDEX%5]);

}

void scroll_input() {



}

String* button_in_to_arr(String id) {

    String *arr;

    if (id.length() == 8) {

        uint8_t reg_btn = atoi(id.c_str());

        switch(reg_btn) {

            case LEFT_CLICK: {arr = L_CLICK_BIND; break;}
            case RIGHT_CLICK: {arr = R_CLICK_BIND; break;}
            case MIDDLE_CLICK: {arr = MID_CLICK_BIND; break;}
            case MOUSE_6: {arr = M6_CLICK_BIND; break;}
            case MOUSE_7: {arr = M7_CLICK_BIND; break;}
            case MOUSE_8: {arr = M8_CLICK_BIND; break;}
            case MOUSE_9: {arr = M9_CLICK_BIND; break;}
            case MOUSE_10: {arr = M10_CLICK_BIND; break;}
            case MOUSE_11: {arr = M11_CLICK_BIND; break;}
            case MOUSE_12: {arr = M12_CLICK_BIND; break;}
            default: {arr = nullptr; break;}

        }

    } else {

        int btn = int(id);

        switch(btn) {
            case MOUSE_4: {arr = M4_CLICK_BIND; break;}
            case MOUSE_5: {arr = M5_CLICK_BIND; break;}
            default: {arr = nullptr; break;}
        }

    }

    return arr;

}


void setup() {

    // MOUSE 4 & 5
    pinMode(MOUSE_4, INPUT_PULLUP);
    pinMode(MOUSE_5, INPUT_PULLUP);
    // TOP BOARD
    pinMode(TOP_LOAD, OUTPUT);
    pinMode(TOP_CLK, OUTPUT);
    pinMode(TOP_CLK_ENABLE, OUTPUT);
    pinMode(TOP_DATAIN, INPUT);
    // MOBA BOARD
    pinMode(MOBA_LOAD, OUTPUT);
    pinMode(MOBA_CLK, OUTPUT);
    pinMode(MOBA_CLK_ENABLE, OUTPUT);
    pinMode(MOBA_DATAIN, INPUT);

    

    // START UP PROCESSES
    Serial.begin(9600);
    sensor.setCPI(CURRENT_DPI);
    Keyboard.begin();
    Mouse.begin();

}

// MAIN LOOP
void loop() {

  // COMPANION APP COMMUNICATION
    // check for communication with companion app
  if (Serial.available() > 0) {

    String companion_read = Serial.readString();

    if (companion_read == RECEIVE_CONNECT_IDENT) { // sends "World"
      send_profile();
    }
    else if (companion_read == RECEIVE_UCONFIG_IDENT) { // sends "Gamer"
      update_config();
    }
    else if (companion_read == RECEIVE_SCONFIG_IDENT) { // sends "Chat"
      send_config();
    }

  }
  
  Handle_Pins();


  // MAIN MOUSE FUNCTIONALITY  
    // callibrate mouse
  PMW3360_DATA data = sensor.readBurst();
  if (data.isOnSurface && data.isMotion) {
    int mdx = constrain(data.dx, -127, 127);
    int mdy = constrain(data.dy, -127, 127);
    Mouse.move(mdx, mdy, 0);
  }
  

}
