String completeMessage = "";
String MouseIDENT = "Hello";
String ResponseIDENT = "World";
boolean receiving = true;

void setup() {
  Serial.begin(9600);
}

void loop() {
  while (Serial.available() > 0 && receiving) {
    String pkt = Serial.readString();
    String trimmedPkt = pkt;
    trimmedPkt.trim();
    if (trimmedPkt == MouseIDENT){
      receiving = false;
      completeMessage = trimmedPkt;
      break;
    }

    if (pkt == "<FIN>") {
      receiving = false;
    } else {
      completeMessage += trimmedPkt;
    }
  }

  if (!receiving) {
    // The complete message is assembled
    // You can now parse and use the data
    if (completeMessage == MouseIDENT){
      Serial.println(ResponseIDENT);
    }else if (completeMessage.startsWith("ProfileName:")){
      Serial.println(completeMessage);
    } else{
      Serial.println(completeMessage);
    }

    // Reset for the next message
    completeMessage = "";
    receiving = true;
  }
}
