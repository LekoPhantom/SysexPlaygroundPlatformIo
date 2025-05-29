#ifndef CONTROLSURFACE_SINGLETON_H
#define CONTROLSURFACE_SINGLETON_H

#include <Control_Surface.h> // Include the Control Surface library
//#include <MIDI_Interfaces/BLEMIDI/ArduinoBLEBackend.hpp>
//#include <ArduinoBLE.h>
#include <Display/DisplayInterfaces/DisplayInterfaceSSD1306.hpp>
#include "MCU.hpp"// Include the MCU header for track colors and other elements
#include "Pixies.hpp"
class ControlSurfaceSingleton {
  // Instantiate a MIDI over USB interface.
BluetoothMIDI_Interface midi;
  public:
    // Get the singleton instance
    static ControlSurfaceSingleton& getInstance() {
        static ControlSurfaceSingleton instance;
        return instance;
    }

    // Custom MIDI callback that prints incoming SysEx messages.
#include "MCU.hpp" // Make sure this is included

struct MyMIDI_Callbacks : MIDI_Callbacks {
  void onSysExMessage(MIDI_Interface &, SysExMessage sysex) override {
    const uint8_t colorStart = 6; // Adjust if your header is longer/shorter
    uint8_t numTracks = NUM_BANKS * TRACKS_PER_BANK;
    if (sysex.length >= colorStart + numTracks + 1 && sysex.data[5] == 0x72) {
      for (uint8_t i = 0; i < numTracks; ++i) {
        uint8_t bankIndex = i / TRACKS_PER_BANK;
        uint8_t track = i % TRACKS_PER_BANK;
        uint8_t color = sysex.data[colorStart + i];
        trackColors[bankIndex][track] = color;
        Serial << F("Init: Set color for bank ") << bankIndex
               << F(", track ") << track
               << F(" to ") << color << endl;
      }
    }
    // ... (you can keep your existing per-bank logic for later updates)
  }
} callback;

// Add this public method to your singleton:
void requestAllTrackColors() {
    uint8_t bankIndex = bank.getSelection();
    for (uint8_t track = 0; track < TRACKS_PER_BANK; ++track) {
        // Construct your SysEx request message for this track
        uint8_t sysexRequest[] = {0xF0, 0x7D, 0x01, 0x10, bankIndex, track, 0xF7}; // Example format
        midi.sendSysEx(sysexRequest, sizeof(sysexRequest));
    }
}

    // Public methods to initialize and update the control surface
    void begin() {
      //midi.begin(); 
        Control_Surface.begin(); // Initialize Control Surface
        midi.setCallbacks(callback);
    }

    void loop() {
        Control_Surface.loop(); // Update Control Surface
        //midi.update(); 
    }
 
    // Add custom methods for interacting with the control surface if needed
    boolean getStatus() {
    /*if (midi.isConnected()) {
        Serial.println("Bluetooth MIDI is connected.");
    } else {
        Serial.println("Bluetooth MIDI is not connected.");
    }
*/
    return midi.isConnected();  
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
