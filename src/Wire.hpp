#ifndef WIRE_HPP
#define WIRE_HPP

#include <Control_Surface.h>
#include <Wire.h>
#include "Screen.hpp"

void initWire() {
    // Initialize the I2C buses for the displays
    Wire.begin(21, 22);  // SDA, SCL for left display
    Wire1.begin(25, 26); // SDA, SCL for right display
    delay(10);  
            // Allow buses to stabilize
}


#endif