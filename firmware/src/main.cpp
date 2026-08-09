#include <Arduino.h>

// Pins
const int PowerSwitch = 2;
const int BinSwitch = 3;
const int TankFull = 5;
const int TankEmpty = 6;
const int IrBlaster = 7;
const int Compressor = 8;
const int Auger = 9;
const int Fan = 10;
const int UvLed = 11;
const int Pump = 12;
const int IrReceiver = 13;
const int MotorAmmeter = A0;

void setup() {
  // Grounded inputs need to be pulled up
  pinMode(PowerSwitch, INPUT_PULLUP);
  pinMode(BinSwitch, INPUT_PULLUP);
  pinMode(TankFull, INPUT_PULLUP);
  pinMode(TankEmpty, INPUT_PULLUP);

  // Standard inputs
  pinMode(IrReceiver, INPUT);
  pinMode(MotorAmmeter, INPUT);

  // Outputs
  pinMode(IrBlaster, OUTPUT);
  pinMode(Compressor, OUTPUT);
  pinMode(Auger, OUTPUT);
  pinMode(Fan, OUTPUT);
  pinMode(UvLed, OUTPUT);
  pinMode(Pump, OUTPUT);
};

// Persistent Values
bool pumping = false;
bool waitingForReset = false;
const unsigned long pumpTimeout = 30000; // 30s
unsigned long pumpStartedTime = 0;

void loop() {
  // DIGITAL READS ARE INVERTED BECAUSE WE PULL UP!!!
  const bool isPowered = digitalRead(PowerSwitch) == LOW;
  const bool isTankFull = digitalRead(TankFull) == LOW;
  const bool isTankEmpty = digitalRead(TankEmpty) == LOW;
  const bool isBinInserted = digitalRead(BinSwitch) == LOW;

  if (!isPowered || waitingForReset) {
      digitalWrite(Pump, LOW);
      digitalWrite(Compressor, LOW);
      digitalWrite(Fan, LOW);
      digitalWrite(UvLed, LOW);
      digitalWrite(IrBlaster, LOW);
      digitalWrite(Auger, LOW);
      pumping = false;
    if (!isBinInserted) {
        waitingForReset = false;
    }
    return;
  }

  if (pumping && (millis() - pumpStartedTime  > pumpTimeout)) {
    // We are probably out of water
    waitingForReset = true;
    return;
  }

  if (isTankEmpty && !pumping) {
    digitalWrite(UvLed, HIGH);
    digitalWrite(Pump, HIGH);
    pumping = true;
    pumpStartedTime = millis();
  }

  if (isTankFull && pumping) {
    digitalWrite(UvLed, LOW);
    digitalWrite(Pump, LOW);
    pumping = false;
  }
}
