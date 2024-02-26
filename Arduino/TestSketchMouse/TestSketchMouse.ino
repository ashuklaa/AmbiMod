String incomingData = "";
String idPrompt = "Hello";
String idResponse = "World";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){
    ;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0){

    incomingData = Serial.readStringUntil('\n');
    incomingData.trim();


    if (incomingData.equals(idPrompt)){

      Serial.println(idResponse);
    }
  }
}
