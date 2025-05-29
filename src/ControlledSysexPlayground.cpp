// ControlledSysexPlayground.cpp
#include <Arduino.h>
#include "Screen.hpp"
#include "Pixies.hpp"
#include "ControlSurfaceSingleton.h"
#include "Wire.hpp"
#include "MCU.hpp"




void setup() {
  Serial.begin(115200);
  initPixies();
  delay(1000);
  Serial.println("Strarting control surface");
  
  initWire(); // Initialize I2C buses for displays
  initDisplays();
  
  
    // Initialize the Control Surface through the singleton
  ControlSurfaceSingleton::getInstance().begin();
  //Control_Surface.onSysEx(handleSysEx);
  //Serial.println("Hi");
  ControlSurfaceSingleton::getInstance().requestAllTrackColors();
  delay(1000);
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  //delay(1000);
  
  

}
uint8_t lastBankIndex = 0; 
void loop() {
  ControlSurfaceSingleton::getInstance().loop();
  if (ControlSurfaceSingleton::getInstance().getStatus() == false) {
    lightItUp();
  } else {
    uint8_t bankIndex = bank.getSelection();
    if (bankIndex != lastBankIndex) {
      Serial.print("Bank changed! Requesting colors for bank: ");
      Serial.println(bankIndex);
      ControlSurfaceSingleton::getInstance().requestAllTrackColors();
      lastBankIndex = bankIndex;
    }
    // Always update LEDs to reflect current trackColors for the active bank
    for (uint8_t track = 0; track < TRACKS_PER_BANK; ++track) {
      Serial.print("Track ");
      Serial.print(track);
      Serial.print(" color: ");
      Serial.println(trackColors[bankIndex][track]);
      setLEDColor(track, trackColors[bankIndex][track]);
    }
  }
  updateDisplays();
}
