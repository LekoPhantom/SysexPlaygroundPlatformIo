#Controlled Sysex Playground

##ControlledSysexPlayground.ino
```C++
// ControlledSysexPlayground.ino
#include "Screen.hpp"
#include "Pixies.hpp"
#include "ControlSurfaceSingleton.h"

void setup() {
  initPixies();
  Serial.begin(115200);
  delay(1000);
  Serial.println("Strarting control surface");
    // Initialize the Control Surface through the singleton
  ControlSurfaceSingleton::getInstance().begin();
  Serial.println("Hi");
  initDisplays();
}

void loop() {
  updatePixies();
  ControlSurfaceSingleton::getInstance().loop();
  ControlSurfaceSingleton::getInstance().getStatus();
  updateDisplays();
  //Serial.println("Hi");
}
```

```C++
#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
//#include <Control_Surface.h> // Include the Control Surface library
#include "ControlSurfaceSingleton.h"
#include <Display/DisplayInterfaces/DisplayInterfaceSSD1306.hpp>


// ----------------------------- MIDI Interface ----------------------------- //
//BluetoothMIDI_Interface midi;

// ----------------------------- Display setup ------------------------------ //
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr int8_t OLED_RESET = -1; // Use the external RC circuit for reset

// ✅ Instantiate the displays correctly with their respective I2C buses
Adafruit_SSD1306 ssd1306Display_L(128, 64, &Wire, -1);
Adafruit_SSD1306 ssd1306Display_R(128, 64, &Wire1, -1);

// ----------------------------- Display Interface ----------------------------- //
#if defined(ADAFRUIT_SSD1306_HAS_SETBUFFER) && ADAFRUIT_SSD1306_HAS_SETBUFFER
static uint8_t buffer[(SCREEN_WIDTH * SCREEN_HEIGHT + 7) / 8];
#endif

class MySSD1306_DisplayInterface : public SSD1306_DisplayInterface {
public:
    MySSD1306_DisplayInterface(Adafruit_SSD1306 &display)
        : SSD1306_DisplayInterface(display) {}

    void begin() override {
#if defined(ADAFRUIT_SSD1306_HAS_SETBUFFER) && ADAFRUIT_SSD1306_HAS_SETBUFFER
        disp.setBuffer(buffer);
#endif
        if (!disp.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 initialization failed."));
            while (true);
        }
        SSD1306_DisplayInterface::begin();
        disp.setTextColor(WHITE);
        disp.setTextSize(1);
    }

    void display() override {
        SSD1306_DisplayInterface::display();
    }

    void drawBackground() override {}

} display_L(ssd1306Display_L), display_R(ssd1306Display_R);

Bank<2> bank(4); // Create a new bank with four tracks per bank

// Create a new bank selector to control the bank using a push button
IncrementSelector<2> bankselectorUP {bank, 18};


// -------------------------- MIDI Input Elements --------------------------- //
// ========================================================================== //

/*
 * Define all elements that listen for MIDI messages.
 */

// Main MCU LCD screen, used to get track names
MCU::LCD<> lcd {};

// Time display_L keeps track of the bar counter
MCU::TimeDisplay timedisplay {};

// Play / Record
NoteValue play {MCU::PLAY};
NoteValue record {MCU::RECORD};

// Mute
Bankable::NoteValue<2> mute[] {
  {bank, MCU::MUTE_1},
  {bank, MCU::MUTE_2},
  {bank, MCU::MUTE_3},
  {bank, MCU::MUTE_4},
};

// Solo
Bankable::NoteValue<2> solo[] {
  {bank, MCU::SOLO_1},
  {bank, MCU::SOLO_2},
  {bank, MCU::SOLO_3},
  {bank, MCU::SOLO_4},
};

NoteValue rudeSolo {MCU::RUDE_SOLO};

// Record arm / ready
Bankable::NoteValue<2> recrdy[] {
  {bank, MCU::REC_RDY_1},
  {bank, MCU::REC_RDY_2},
  {bank, MCU::REC_RDY_3},
  {bank, MCU::REC_RDY_4},
};

// VU meters
MCU::Bankable::VU<2> vu[] {
  {bank, 1, MCU::VUDecay::Hold},
  {bank, 2, MCU::VUDecay::Hold},
  {bank, 3, MCU::VUDecay::Hold},
  {bank, 4, MCU::VUDecay::Hold},
};

// VPot rings
MCU::Bankable::VPotRing<2> vpot[] {
  {bank, 1},
  {bank, 2},
  {bank, 3},
  {bank, 4},
};

// ---------------------------- Display Elements ---------------------------- //
// ========================================================================== //

/*
 * Define all display_L elements that display_L the state of the input elements.
 */

// Track names
/*
MCU::LCDDisplay lcddisps[] {
  // track (1), position (0, 40), font size (1)
  {display_L, lcd, bank, 1,1, {0, 40}, 1, WHITE},
  {display_L, lcd, bank, 2, 1, {64, 40}, 1, WHITE},
  {display_R, lcd, bank, 3, 1, {0, 40}, 1, WHITE},
  {display_R, lcd, bank, 4, 1, {64, 40}, 1, WHITE},
};
*/
// Track names (two lines per track)
MCU::LCDDisplay lcddisps[] {
  // Line 1
  {display_L, lcd, bank, 1, 1, {0, 40}, 1, WHITE},
  {display_L, lcd, bank, 2, 1, {64, 40}, 1, WHITE},
  {display_R, lcd, bank, 3, 1, {0, 40}, 1, WHITE},
  {display_R, lcd, bank, 4, 1, {64, 40}, 1, WHITE},

  // Line 2
  {display_L, lcd, bank, 1, 2, {0, 50}, 1, WHITE},
  {display_L, lcd, bank, 2, 2, {64, 50}, 1, WHITE},
  {display_R, lcd, bank, 3, 2, {0, 50}, 1, WHITE},
  {display_R, lcd, bank, 4, 2, {64, 50}, 1, WHITE},
};

// Time display
MCU::TimeDisplayDisplay timedisplaydisplay {
  // position (0, 0), font size (1)
  display_L, timedisplay, {0, 0}, 1, WHITE,
};

// Play / Record
BitmapDisplay<> playDisp {
  display_L, play, XBM::play_7, {16 + 64, 0}, WHITE,
};
BitmapDisplay<> recordDisp {
  display_L, record, XBM::record_7, {26 + 64, 0}, WHITE,
};

// Mute
BitmapDisplay<> muteDisp[] {
  {display_L, mute[0], XBM::mute_10B, {14, 50}, WHITE},
  {display_L, mute[1], XBM::mute_10B, {14 + 64, 50}, WHITE},
  {display_R, mute[2], XBM::mute_10B, {14, 50}, WHITE},
  {display_R, mute[3], XBM::mute_10B, {14 + 64, 50}, WHITE},
};

// Solo
BitmapDisplay<> soloDisp[] {
  {display_L, solo[0], XBM::solo_10B, {14, 50}, WHITE},
  {display_L, solo[1], XBM::solo_10B, {14 + 64, 50}, WHITE},
  {display_R, solo[2], XBM::solo_10B, {14, 50}, WHITE},
  {display_R, solo[3], XBM::solo_10B, {14 + 64, 50}, WHITE},
};

BitmapDisplay<> rudeSoloDisp {
  display_L, rudeSolo, XBM::solo_7, {36 + 64, 0}, WHITE};

// Record arm / ready
BitmapDisplay<> recrdyDisp[] {
  {display_L, recrdy[0], XBM::rec_rdy_10B, {14 + 14, 50}, WHITE},
  {display_L, recrdy[1], XBM::rec_rdy_10B, {14 + 14 + 64, 50}, WHITE},
  {display_R, recrdy[2], XBM::rec_rdy_10B, {14 + 14, 50}, WHITE},
  {display_R, recrdy[3], XBM::rec_rdy_10B, {14 + 14 + 64, 50}, WHITE},
};

// VU meters
MCU::VUDisplay<> vuDisp[] {
  // position (32+11, 60), width (16), bar height (3) px, bar spacing (1) px
  {display_L, vu[0], {32 + 11, 60}, 16, 3, 1, WHITE},
  {display_L, vu[1], {32 + 11 + 64, 60}, 16, 3, 1, WHITE},
  {display_R, vu[2], {32 + 11, 60}, 16, 3, 1, WHITE},
  {display_R, vu[3], {32 + 11 + 64, 60}, 16, 3, 1, WHITE},
};

// VPot rings
MCU::VPotDisplay<> vpotDisp[] {
  // position (0, 10), outer radius (14) px, inner radius (12) px
  {display_L, vpot[0], {0, 10}, 14, 12, WHITE},
  {display_L, vpot[1], {64, 10}, 14, 12, WHITE},
  {display_R, vpot[2], {0, 10}, 14, 12, WHITE},
  {display_R, vpot[3], {64, 10}, 14, 12, WHITE},
};

// Bank seting <----- These are the track number, but I am not sure if it is the real track numbers, or just the bank?

BankDisplay bankDisp[] {
  // first track of the bank (1), position (0, 50), font size (2)
  {display_L, bank, 1, {0, 50}, 1, WHITE},
  {display_L, bank, 2, {64, 50}, 1, WHITE},
  {display_R, bank, 3, {0, 50}, 1, WHITE},
  {display_R, bank, 4, {64, 50}, 1, WHITE},
};

#include <map>

std::map<uint8_t, String> trackNames; // Store track names for display



void updateLCDDisplay() {
    Serial.println("Updating LCD Display...");

        // ✅ Add a custom message without interfering
    // display_L.setCursor(20, 20); // Adjust position
    // display_L.print("Hello, Leko!");

    lcd.clearDirty();
}


// ------------------------------- Setup ---------------------------------- //
void initDisplays() {
    // ✅ Initialize I2C buses before display instantiation
    Wire.begin(21, 22);
    Wire1.begin(25, 26);
    delay(10);  // Allow buses to stabilize

    // ✅ Properly initialize displays after I2C buses
    if (!ssd1306Display_L.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("Left OLED failed to initialize."));
        while (true);
    }

    if (!ssd1306Display_R.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("Right OLED failed to initialize."));
        while (true);
    }

    //Control_Surface.begin(); // ✅ Initialize Control Surface AFTER display setup
}

void updateDisplays() {
    uint8_t currentBank = bank.getSelection();
    if (currentBank >= 4) {
        Serial.println("Invalid Bank Access! Resetting...");
        //bank.setSelection(0);
    }
    
    //Control_Surface.loop();  
    updateLCDDisplay();
}
#endif
```
```
#ifndef PIXIES_HPP
#define PIXIES_HPP

#include <Arduino.h>
#define Selectable FastLED_Selectable
#include <FastLED.h>
#include <Control_Surface.h>

#define LED_PIN 5  
#define NUM_LEDS 2 

CRGB leds[NUM_LEDS];

void initPixies() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(20);
}

// ✅ Explicitly use FastLED namespace
CRGB getFastLEDColor(uint8_t colorByte) {
    switch (colorByte) {
        case 0x01: return CRGB::Red;
        case 0x02: return CRGB::Green;
        case 0x03: return CRGB::Yellow;
        case 0x04: return CRGB::Blue;
        case 0x05: return CRGB::Purple;
        case 0x06: return CRGB::Teal;
        default: return CRGB::White;
    }
}

void updateLEDColor(uint8_t colorByte) {
    CRGB trackColor = getFastLEDColor(colorByte);
    leds[0] = trackColor;
    leds[1] = trackColor;
    FastLED.show();
}


uint8_t ledCounter = 0;
void  updatePixies() {
  static uint32_t lastUpdateTime = 0;
    if (millis() - lastUpdateTime > 5000) {  // ✅ Print every 5 seconds
        //trackManager.printTrackNames();
        //debugPrint(trackManager.getTrackColor(1));
        updateLEDColor(ledCounter);
        lastUpdateTime = millis();
        if (ledCounter > 5){
          ledCounter = 0;
        }
        ledCounter++;
    }
}
#endif
```
```C++
#ifndef CONTROLSURFACE_SINGLETON_H
#define CONTROLSURFACE_SINGLETON_H

#include <Control_Surface.h> // Include the Control Surface library
//#include <MIDI_Interfaces/BLEMIDI/ArduinoBLEBackend.hpp>
//#include <ArduinoBLE.h>

class ControlSurfaceSingleton {
    BluetoothMIDI_Interface midi;
    
public:
    static ControlSurfaceSingleton& getInstance() {
        static ControlSurfaceSingleton instance;
        return instance;
    }

    void begin() {
        Control_Surface.begin();
    }

    void loop() {
        Control_Surface.loop();
    }

    void getStatus() {
        if (midi.isConnected()) {
            Serial.println("Bluetooth MIDI is connected.");
        } else {
            Serial.println("Bluetooth MIDI is not connected.");
        }
    }

    // ✅ Template method for adding components dynamically
    /*
    template <typename T>
    void addComponent(T &component) {
        Control_Surface.add(component);
    }
*/
private:
    ControlSurfaceSingleton() {}
    ControlSurfaceSingleton(const ControlSurfaceSingleton&) = delete;
    ControlSurfaceSingleton& operator=(const ControlSurfaceSingleton&) = delete;
};


#endif // CONTROLSURFACE_SINGLETON_H
```

#ifndef SCREEN_HPP
#define SCREEN_HPP

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Control_Surface.h> // Include the Control Surface library
//#include "ControlSurfaceSingleton.h"
#include <Display/DisplayInterfaces/DisplayInterfaceSSD1306.hpp>


// ----------------------------- MIDI Interface ----------------------------- //
BluetoothMIDI_Interface midi;

// ----------------------------- Display setup ------------------------------ //
constexpr uint8_t SCREEN_WIDTH = 128;
constexpr uint8_t SCREEN_HEIGHT = 64;
constexpr int8_t OLED_RESET = -1; // Use the external RC circuit for reset

// ✅ Instantiate the displays correctly with their respective I2C buses
Adafruit_SSD1306 ssd1306Display_L(128, 64, &Wire, -1);
Adafruit_SSD1306 ssd1306Display_R(128, 64, &Wire1, -1);

// ----------------------------- Display Interface ----------------------------- //
#if defined(ADAFRUIT_SSD1306_HAS_SETBUFFER) && ADAFRUIT_SSD1306_HAS_SETBUFFER
static uint8_t buffer[(SCREEN_WIDTH * SCREEN_HEIGHT + 7) / 8];
#endif

class MySSD1306_DisplayInterface : public SSD1306_DisplayInterface {
public:
    MySSD1306_DisplayInterface(Adafruit_SSD1306 &display)
        : SSD1306_DisplayInterface(display) {}

    void begin() override {
#if defined(ADAFRUIT_SSD1306_HAS_SETBUFFER) && ADAFRUIT_SSD1306_HAS_SETBUFFER
        disp.setBuffer(buffer);
#endif
        if (!disp.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
            Serial.println(F("SSD1306 initialization failed."));
            while (true);
        }
        SSD1306_DisplayInterface::begin();
        disp.setTextColor(WHITE);
        disp.setTextSize(1);
    }

    void display() override {
        SSD1306_DisplayInterface::display();
    }

    void drawBackground() override {}

} display_L(ssd1306Display_L), display_R(ssd1306Display_R);

Bank<2> bank(4); // Create a new bank with four tracks per bank

// Create a new bank selector to control the bank using a push button
IncrementSelector<2> bankselectorUP {bank, 18};


// -------------------------- MIDI Input Elements --------------------------- //
// ========================================================================== //

/*
 * Define all elements that listen for MIDI messages.
 */

// Main MCU LCD screen, used to get track names
MCU::LCD<> lcd {};

// Time display_L keeps track of the bar counter
MCU::TimeDisplay timedisplay {};

// Play / Record
NoteValue play {MCU::PLAY};
NoteValue record {MCU::RECORD};

// Mute
Bankable::NoteValue<2> mute[] {
  {bank, MCU::MUTE_1},
  {bank, MCU::MUTE_2},
  {bank, MCU::MUTE_3},
  {bank, MCU::MUTE_4},
};

// Solo
Bankable::NoteValue<2> solo[] {
  {bank, MCU::SOLO_1},
  {bank, MCU::SOLO_2},
  {bank, MCU::SOLO_3},
  {bank, MCU::SOLO_4},
};

NoteValue rudeSolo {MCU::RUDE_SOLO};

// Record arm / ready
Bankable::NoteValue<2> recrdy[] {
  {bank, MCU::REC_RDY_1},
  {bank, MCU::REC_RDY_2},
  {bank, MCU::REC_RDY_3},
  {bank, MCU::REC_RDY_4},
};

// VU meters
MCU::Bankable::VU<2> vu[] {
  {bank, 1, MCU::VUDecay::Hold},
  {bank, 2, MCU::VUDecay::Hold},
  {bank, 3, MCU::VUDecay::Hold},
  {bank, 4, MCU::VUDecay::Hold},
};

// VPot rings
MCU::Bankable::VPotRing<2> vpot[] {
  {bank, 1},
  {bank, 2},
  {bank, 3},
  {bank, 4},
};

// ---------------------------- Display Elements ---------------------------- //
// ========================================================================== //

/*
 * Define all display_L elements that display_L the state of the input elements.
 */

// Track names
/*
MCU::LCDDisplay lcddisps[] {
  // track (1), position (0, 40), font size (1)
  {display_L, lcd, bank, 1,1, {0, 40}, 1, WHITE},
  {display_L, lcd, bank, 2, 1, {64, 40}, 1, WHITE},
  {display_R, lcd, bank, 3, 1, {0, 40}, 1, WHITE},
  {display_R, lcd, bank, 4, 1, {64, 40}, 1, WHITE},
};
*/
// Track names (two lines per track)
MCU::LCDDisplay lcddisps[] {
  // Line 1
  {display_L, lcd, bank, 1, 1, {0, 40}, 1, WHITE},
  {display_L, lcd, bank, 2, 1, {64, 40}, 1, WHITE},
  {display_R, lcd, bank, 3, 1, {0, 40}, 1, WHITE},
  {display_R, lcd, bank, 4, 1, {64, 40}, 1, WHITE},

  // Line 2
  {display_L, lcd, bank, 1, 2, {0, 50}, 1, WHITE},
  {display_L, lcd, bank, 2, 2, {64, 50}, 1, WHITE},
  {display_R, lcd, bank, 3, 2, {0, 50}, 1, WHITE},
  {display_R, lcd, bank, 4, 2, {64, 50}, 1, WHITE},
};

// Time display
MCU::TimeDisplayDisplay timedisplaydisplay {
  // position (0, 0), font size (1)
  display_L, timedisplay, {0, 0}, 1, WHITE,
};

// Play / Record
BitmapDisplay<> playDisp {
  display_L, play, XBM::play_7, {16 + 64, 0}, WHITE,
};
BitmapDisplay<> recordDisp {
  display_L, record, XBM::record_7, {26 + 64, 0}, WHITE,
};

// Mute
BitmapDisplay<> muteDisp[] {
  {display_L, mute[0], XBM::mute_10B, {14, 50}, WHITE},
  {display_L, mute[1], XBM::mute_10B, {14 + 64, 50}, WHITE},
  {display_R, mute[2], XBM::mute_10B, {14, 50}, WHITE},
  {display_R, mute[3], XBM::mute_10B, {14 + 64, 50}, WHITE},
};

// Solo
BitmapDisplay<> soloDisp[] {
  {display_L, solo[0], XBM::solo_10B, {14, 50}, WHITE},
  {display_L, solo[1], XBM::solo_10B, {14 + 64, 50}, WHITE},
  {display_R, solo[2], XBM::solo_10B, {14, 50}, WHITE},
  {display_R, solo[3], XBM::solo_10B, {14 + 64, 50}, WHITE},
};

BitmapDisplay<> rudeSoloDisp {
  display_L, rudeSolo, XBM::solo_7, {36 + 64, 0}, WHITE};

// Record arm / ready
BitmapDisplay<> recrdyDisp[] {
  {display_L, recrdy[0], XBM::rec_rdy_10B, {14 + 14, 50}, WHITE},
  {display_L, recrdy[1], XBM::rec_rdy_10B, {14 + 14 + 64, 50}, WHITE},
  {display_R, recrdy[2], XBM::rec_rdy_10B, {14 + 14, 50}, WHITE},
  {display_R, recrdy[3], XBM::rec_rdy_10B, {14 + 14 + 64, 50}, WHITE},
};

// VU meters
MCU::VUDisplay<> vuDisp[] {
  // position (32+11, 60), width (16), bar height (3) px, bar spacing (1) px
  {display_L, vu[0], {32 + 11, 60}, 16, 3, 1, WHITE},
  {display_L, vu[1], {32 + 11 + 64, 60}, 16, 3, 1, WHITE},
  {display_R, vu[2], {32 + 11, 60}, 16, 3, 1, WHITE},
  {display_R, vu[3], {32 + 11 + 64, 60}, 16, 3, 1, WHITE},
};

// VPot rings
MCU::VPotDisplay<> vpotDisp[] {
  // position (0, 10), outer radius (14) px, inner radius (12) px
  {display_L, vpot[0], {0, 10}, 14, 12, WHITE},
  {display_L, vpot[1], {64, 10}, 14, 12, WHITE},
  {display_R, vpot[2], {0, 10}, 14, 12, WHITE},
  {display_R, vpot[3], {64, 10}, 14, 12, WHITE},
};

// Bank seting <----- These are the track number, but I am not sure if it is the real track numbers, or just the bank?

BankDisplay bankDisp[] {
  // first track of the bank (1), position (0, 50), font size (2)
  {display_L, bank, 1, {0, 50}, 1, WHITE},
  {display_L, bank, 2, {64, 50}, 1, WHITE},
  {display_R, bank, 3, {0, 50}, 1, WHITE},
  {display_R, bank, 4, {64, 50}, 1, WHITE},
};

#include <map>

std::map<uint8_t, String> trackNames; // Store track names for display



void updateLCDDisplay() {
    //Serial.println("Updating LCD Display...");

        // ✅ Add a custom message without interfering
    // display_L.setCursor(20, 20); // Adjust position
    // display_L.print("Hello, Leko!");

    lcd.clearDirty();
}


// ------------------------------- Setup ---------------------------------- //
void initDisplays() {
    // ✅ Initialize I2C buses before display instantiation
    Wire.begin(21, 22);
    Wire1.begin(25, 26);
    delay(10);  // Allow buses to stabilize

    // ✅ Properly initialize displays after I2C buses
    if (!ssd1306Display_L.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("Left OLED failed to initialize."));
        while (true);
    }

    if (!ssd1306Display_R.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        Serial.println(F("Right OLED failed to initialize."));
        while (true);
    }

    Control_Surface.begin(); // ✅ Initialize Control Surface AFTER display setup
}

void updateDisplays() {
    uint8_t currentBank = bank.getSelection();
    if (currentBank >= 2) {
        Serial.println("Invalid Bank Access! Resetting...");
        //bank.setSelection(0);
    }
    
    Control_Surface.loop();  
    updateLCDDisplay();
}
#endif