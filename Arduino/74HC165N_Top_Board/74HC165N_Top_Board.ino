/*
Project: 74HC165N Shift Register Top Board
Programmer: Kevin Nguyen
Datasheet: https://www.ti.com/lit/ds/symlink/sn74hc165.pdf
Description:
Allows data from top board to be sent to main board using four connections instead of six.
The data sent will be the states of M1, M2, M3, A, B, DPI.
*/

// Define globals (change as necessary)
byte lastData = 0;
byte data = 0;
const int mouseDelay = 5;

// Define pin connections (change as necessary)
const int load = 8; // PL pin 1: When low, the parallel inputs are loaded. The clock is also ignored at this time. When high, the clock is able to be read.
const int clock = 5;  // CP pin 2: When pulsed, the parallel inputs are push to the shift registers.
const int clockEnable = 6;  // CE pin 15: When low, the clock is able to be pulsed. When high, the clock cannot pulse.
const int dataIn = 7;  // Q7 pin 9: Serial output of shift register.

void setup()
{
  // serial monitor
  Serial.begin(9600);

  // 74HC165N
  pinMode(load, OUTPUT);
  pinMode(clock, OUTPUT);
  pinMode(clockEnable, OUTPUT);
  pinMode(dataIn, INPUT);
}

void loop()
{
  // PL pulse
  digitalWrite(load, LOW);  // turn PL low to load inputs
  delayMicroseconds(5);
  digitalWrite(load, HIGH); // turn PL high so the CP can pulse
  delayMicroseconds(5);
  
  // push data to shift registers
  digitalWrite(clock, HIGH);  // pulse CP with CE low to push inputs
  digitalWrite(clockEnable, LOW);
  lastData = data;
  data = shiftIn(dataIn, clock, LSBFIRST); // shift in data
  digitalWrite(clockEnable, HIGH);  // end of CP pulse

  // decode byte data
  if (lastData != data)
  {
    Serial.print("States: ");

    for (int i = 0; i < 8; i++) // expected output: HGFEDCBA because LSB first
    {
      // output to serial monitor (or uC)
      Serial.print(bitRead(data, i)); // shift register inverts bits
    }

    Serial.print("\n");
  }

  delayMicroseconds(mouseDelay);
}
