#ifndef CONTROLSURFACE_SINGLETON_H
#define CONTROLSURFACE_SINGLETON_H

#include <Control_Surface.h> // Include the Control Surface library
//#include <MIDI_Interfaces/BLEMIDI/ArduinoBLEBackend.hpp>
//#include <ArduinoBLE.h>
#include <Display/DisplayInterfaces/DisplayInterfaceSSD1306.hpp>

class ControlSurfaceSingleton {
  // Instantiate a MIDI over USB interface.
BluetoothMIDI_Interface midi;
  public:
    // Get the singleton instance
    static ControlSurfaceSingleton& getInstance() {
        static ControlSurfaceSingleton instance;
        return instance;
    }

    // Public methods to initialize and update the control surface
    void begin() {
        Control_Surface.begin(); // Initialize Control Surface
    }

    void loop() {
        Control_Surface.loop(); // Update Control Surface
    }
 
    // Add custom methods for interacting with the control surface if needed
    void getStatus() {
    if (midi.isConnected()) {
        Serial.println("Bluetooth MIDI is connected.");
    } else {
        Serial.println("Bluetooth MIDI is not connected.");
    }
}

  private:
    // Private constructor to enforce singleton pattern
    ControlSurfaceSingleton() {}

    // Disallow copy and assignment
    ControlSurfaceSingleton(const ControlSurfaceSingleton&) = delete;
    ControlSurfaceSingleton& operator=(const ControlSurfaceSingleton&) = delete;

    // Control_Surface instance can be managed here (if additional customization is needed)
};

#endif // CONTROLSURFACE_SINGLETON_H
