String MouseIDENT = "Hello";
String ResponseIDENT = "World";
boolean receiving = true;

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
    } else if (response.startsWith("ProfileName:")){
      Serial.println("Profile Received from App: " + response);
    } else{
      Serial.println(response);
    }
  }
}
