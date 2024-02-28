#include <Mouse.h>

// Define the pins for the encoder
const int encoderPinA = 2;
const int encoderPinB = 3;

// Define variables to track encoder state
volatile int encoderPos = 0;
volatile int lastEncoderA = LOW;
volatile int lastEncoderB = LOW;

void setup()
{
  Serial.begin(9600);

  Mouse.begin();

  // Initialize pins
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);

  // Attach interrupts to encoder pins
  attachInterrupt(digitalPinToInterrupt(encoderPinA), updateEncoder, CHANGE);
}

void loop()
{
  updateEncoder();

  if (encoderPos != 0)
  {
    Serial.println(encoderPos);
    Mouse.move(0, 0, encoderPos);
    encoderPos = 0; // reset scroll
  }

  delay(10);
}

void updateEncoder()
{
  int newEncoderA = digitalRead(encoderPinA);
  int newEncoderB = digitalRead(encoderPinB);

  // if A leads B
  if (newEncoderA != lastEncoderA)
  {
    if (newEncoderA == HIGH && newEncoderB == LOW || newEncoderA == LOW && newEncoderB == HIGH)
    {
      encoderPos++; // scroll up
    }
  }

  // if B leads A
  if (newEncoderB != lastEncoderB)
  {
    if (newEncoderA == HIGH && newEncoderB == LOW || newEncoderA == LOW && newEncoderB == HIGH)
    {
      encoderPos--; // scroll down
    }
  }

  lastEncoderA = newEncoderA;
  lastEncoderB = newEncoderB;
}
