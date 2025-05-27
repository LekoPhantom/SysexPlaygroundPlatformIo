// ControlledSysexPlayground.cpp
#include <Arduino.h>
#include "Screen.hpp"
#include "Pixies.hpp"
#include "ControlSurfaceSingleton.h"

void setup() {
  initPixies();
  Serial.begin(115200);
  delay(1000);
  Serial.println("Strarting control surface");
  initDisplays();
    // Initialize the Control Surface through the singleton
  ControlSurfaceSingleton::getInstance().begin();
  Serial.println("Hi");
  delay(1000);
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  delay(1000);

}

void loop() {

  updatePixies();
  ControlSurfaceSingleton::getInstance().loop();
  //ControlSurfaceSingleton::getInstance().getStatus();
  updateDisplays();
  //Serial.println("Greetings from the loop!");
}
