// Copyright (C) 2026 SomeAspy (Aiden B. amb@aspy.dev)
// SPDX-License-Identifier: GPL-3.0-only
// https://github.com/someaspy/GE-Opal-2-fixes

#include <Arduino.h>
#include <EmonLib.h>

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

// Run the compressor
// Run the auger
// Monitor the Ammeter
// Compressor needs cooldown

EnergyMonitor augerMeter;

void setup() {
  Serial.begin(115200);

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
  augerMeter.current(
      MotorAmmeter,
      5.76); // Calibration factor by gemini, because I hate complex math
};

// Persistent Values
bool pumping = false;
bool waitingForReset = false;
const unsigned long pumpTimeout =
    120000; // 2m - may be a bit much but when the filter is installed waterflow
            // can drop significantly.
unsigned long pumpStartedTime = 0;
unsigned long compressorStopTime = 300001; // If the machine is off, assume its
                                           // been off for a safe amount of time
const unsigned long compressorTimeout = 300000; // 5m
bool isCompressorRunning = false;
unsigned long compressorStartTime = 0;

// From testing the machine usually settles around 0.45A to 0.48A.
// 0.05 means the compressor is off.
// Note currentDrawLimit is ignored when the compressor first starts to
// accomodate inrush current. Tweak as needed.
const float currentDrawLimit = 0.50;
const unsigned long compressorGracePeriod = 15000; // 15s for inrush to settle

bool defrostCycle = false;
const unsigned long defrostCycleLength = 600000; // 10m
unsigned long defrostCycleStartTime = 0;

void loop() {
  // DIGITAL READS ARE INVERTED BECAUSE WE PULL UP!!!
  const bool isPowered = digitalRead(PowerSwitch) == LOW;
  const bool isTankFull = digitalRead(TankFull) == LOW;
  const bool isTankEmpty = digitalRead(TankEmpty) == LOW;
  const bool isBinInserted = digitalRead(BinSwitch) == LOW;
  const double currentDraw = augerMeter.calcIrms(
      1480); // IRM sample count by Gemini, because I can't be bothered.

  Serial.println(currentDraw);

  if (!isPowered || waitingForReset || defrostCycle) {
    Serial.println("Halted");
    digitalWrite(Pump, LOW);
    digitalWrite(Compressor, LOW);
    digitalWrite(Fan, LOW);
    digitalWrite(UvLed, LOW);
    digitalWrite(IrBlaster, LOW);
    digitalWrite(Auger, LOW);
    if (isCompressorRunning) {
      compressorStopTime = millis();
      isCompressorRunning = false;
    }
    pumping = false;
    if (!isBinInserted) {
      waitingForReset = false;
    }
    if (millis() - defrostCycleStartTime > defrostCycleLength) {
      defrostCycle = false;
    }
    return;
  }

  if (pumping && (millis() - pumpStartedTime > pumpTimeout)) {
    // We are probably out of water
    Serial.println("no water?");
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

  if (!isCompressorRunning) {
    if (millis() - compressorStopTime < compressorTimeout) {
      return;
    }
    digitalWrite(Compressor, HIGH);
    digitalWrite(Fan, HIGH);
    digitalWrite(Auger, HIGH);
    compressorStartTime = millis();
    isCompressorRunning = true;
  }

  if (currentDraw >= currentDrawLimit &&
      millis() - compressorStartTime > compressorGracePeriod) {
    defrostCycle = true;
    defrostCycleStartTime = millis();
  }
}
