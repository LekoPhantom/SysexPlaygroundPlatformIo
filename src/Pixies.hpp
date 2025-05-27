#ifndef PIXIES_HPP
#define PIXIES_HPP

#include <Arduino.h>
#define Selectable FastLED_Selectable
#include <FastLED.h>
#include <Control_Surface.h>

#define LED_PIN 5  
#define NUM_LEDS 7

CRGB leds[NUM_LEDS];

void initPixies() {
    FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(5);
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

void setAllLEDsColor(uint8_t colorByte) {
    CRGB trackColor = getFastLEDColor(colorByte);
    leds[0] = trackColor;
    leds[1] = trackColor;
    leds[2] = trackColor;
    leds[3] = trackColor;   
    leds[4] = trackColor;
    leds[5] = trackColor;
    leds[6] = trackColor; // Set all LEDs to the same color for simplicity
    FastLED.show();
}

void updateLEDColor(uint8_t trackIndex) {
    // Assuming trackIndex is 0-based and corresponds to the track number
    uint8_t colorByte = trackIndex + 1; // Convert to 1-based color byte
    setAllLEDsColor(colorByte);
}
void setLEDColor(uint8_t trackIndex, uint8_t colorByte) {
    // Assuming trackIndex is 0-based and corresponds to the track number
    if (trackIndex < NUM_LEDS) {
        leds[trackIndex] = getFastLEDColor(colorByte);
        FastLED.show();
    }
}
void rainbow() {
    // Fill the LEDs with a rainbow effect
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CHSV((i * 256 / NUM_LEDS), 255, 255);
    }
    FastLED.show();
}


uint8_t ledCounter = 0;
void  updatePixies() {
  static uint32_t lastUpdateTime = 0;
    if (millis() - lastUpdateTime > 5000) {  // ✅ Print every 5 seconds
        //trackManager.printTrackNames();
        //debugPrint(trackManager.getTrackColor(1));
        //updateLEDColor(ledCounter);
        rainbow();
        lastUpdateTime = millis();
        if (ledCounter > 5){
          ledCounter = 0;
        }
        ledCounter++;
    }
}
#endif