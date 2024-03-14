String MouseIDENT = "Hello";
String ResponseIDENT = "World";
bool testMode = false;

void setup() {
  Serial.begin(115200);
  while(!Serial);
}

void loop() {
  if(Serial.available() > 0){
    String response = Serial.readStringUntil('\n');

    response.trim();

    if (response == MouseIDENT){
      Serial.println(ResponseIDENT);
      Serial.println("Profile6");
    } else if(response == "Testing"){
      testMode = !testMode;
      Serial.println("123");
      delay(3000);
      Serial.println("LMB");
      delay(2000);
      Serial.println("RMB");
      delay(2000);
      Serial.println("KEY_LEFT_CTRL+KEY_LEFT_ARROW");
      delay(3000);
      Serial.println("Test Sequence Complete");
      // if (testMode == true){
      //   Serial.println("Test Mode Enabled");
      // } else if(testMode == false){
      //   Serial.println("Test Mode Disabled");
      // }
    } else if (response.startsWith("ProfileName:")){
      Serial.println("Profile Received from App: " + response);
    } else{
      Serial.println(response);
    }
  }
}
