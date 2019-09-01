
#include <DS3231.h>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN            6
#define PIN2           7

#define NUMPIXELS      60
DS3231  rtc(SDA, SCL);
Time  t;
int hr;
int mnt;
int sc;
char data;
char color_select;
bool dirty = false;

// Current hour color.
uint8_t h_red_deger = 255;
uint8_t h_green_deger = 0;
uint8_t h_blue_deger = 0;

// Current minute color.
uint8_t m_red_deger = 0;
uint8_t m_green_deger = 255;
uint8_t m_blue_deger = 0;

// Current second color.
uint8_t s_red_deger = 255;
uint8_t s_green_deger = 255;
uint8_t s_blue_deger = 0;

// All other hours color.
uint8_t bk_red = 0;
uint8_t bk_green = 255;
uint8_t bk_blue = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(12, PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(12, PIN2, NEO_GRB + NEO_KHZ800);

void loadColors() {
  h_red_deger = EEPROM.read(0);
  h_green_deger = EEPROM.read(1);
  h_blue_deger = EEPROM.read(2);
  m_red_deger = EEPROM.read(3);
  m_green_deger = EEPROM.read(4);
  m_blue_deger = EEPROM.read(5);
  s_red_deger = EEPROM.read(6);
  s_green_deger = EEPROM.read(7);
  s_blue_deger = EEPROM.read(8);
  bk_red = EEPROM.read(9);
  bk_green = EEPROM.read(10);
  bk_blue = EEPROM.read(11);
}

void updateColors() {
  EEPROM.update(0, h_red_deger);
  EEPROM.update(1, h_green_deger);
  EEPROM.update(2, h_blue_deger);
  EEPROM.update(3, m_red_deger);
  EEPROM.update(4, m_green_deger);
  EEPROM.update(5, m_blue_deger);
  EEPROM.update(6, s_red_deger);
  EEPROM.update(7, s_green_deger);
  EEPROM.update(8, s_blue_deger);
  EEPROM.update(9, bk_red);
  EEPROM.update(10, bk_green);
  EEPROM.update(11, bk_blue);
}

void setup() {
  Serial.begin(9600);
  rtc.begin();

  // Load colors from eeprom.
  loadColors();

#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  // End of trinket special code

  pixels.begin(); // This initializes the NeoPixel library.
  pixels2.begin();
  strip.begin();
  strip.show();
  strip2.begin();
  strip2.show();
}

// This function loops on serial input. When the mode switch `m` arrives, it leaves the function.
// It returns the character that broke it out of the loop. The real time clock is set by this function.
char hourSelectMode() {
  t = rtc.getTime();
  hr = t.hour;
  mnt = t.min;
  sc = t.sec;
  char data = ' ';
  while (data != 'm')
  {
    data = Serial.read();
    // Update hour.
    if (data == '+')
    {
      hr++;
    }
    if (data == '-')
    {
      hr--;
    }
    // Convert time to 0-11.
    hr = positive_modulo(hr, 12);

    // Flash display to indicate hour being set.
    updateHourPixels(Adafruit_NeoPixel::Color(h_red_deger, h_green_deger, h_blue_deger),
                     Adafruit_NeoPixel::Color(bk_red, bk_green, bk_blue));
    delay(100);
    updateHourPixels(Adafruit_NeoPixel::Color(0, 0, 0),
                     Adafruit_NeoPixel::Color(bk_red, bk_green, bk_blue));
    delay(100);
  }
  rtc.setTime(hr, mnt, sc);
  return data;
}

// This function loops on serial input. When the mode switch `m` arrives, it leaves the function.
// It returns the character that broke it out of the loop. The real time clock is set by this function.
char minuteSelectMode() {
  t = rtc.getTime();
  hr = t.hour;
  mnt = t.min;
  sc = t.sec;

  char data = ' ';
  while (data != 'm')
  {
    data = Serial.read();
    if (data == '+')
    {
      mnt++;
    }
    if (data == '-')
    {
      mnt--;
    }
    // Convert time to 0-59.
    mnt = positive_modulo(mnt, 60);

    // Flash display to indicate minute being set.
    updateOutsidePixels(Adafruit_NeoPixel::Color(m_red_deger, m_green_deger, m_blue_deger),
                        Adafruit_NeoPixel::Color(s_red_deger, s_green_deger, s_blue_deger));
    delay(100);
    updateOutsidePixels(Adafruit_NeoPixel::Color(0, 0, 0),
                        Adafruit_NeoPixel::Color(s_red_deger, s_green_deger, s_blue_deger));
    delay(100);
  }

  rtc.setTime(hr, mnt, sc);

  return data;
}

// This function reads serial input. The real time clock is set by this function.
char timeSync() {
  hr = positive_modulo(Serial.parseInt(), 12);
  data = Serial.read();
  mnt = positive_modulo(Serial.parseInt(), 60);
  data = Serial.read();
  sc = positive_modulo(Serial.parseInt(), 60);
  rtc.setTime(hr, mnt, sc);
  return data;
}

inline int positive_modulo(int i, int n) {
  return (i % n + n) % n;
}

void loop() {

  while (Serial.available() > 0)
  {
    delay(10);
    data = Serial.read();
    if (data == 'm')
    {
      data = hourSelectMode();
    }

    if (data == 'm')
    {
      data = minuteSelectMode();
    }

    // data must be `m` at this point indicating new mode,
    // but there may be no more data. No execution path below
    // should execute because data is m.

    if (data == 'T') 
    {
      data = timeSync();
    }

    // Indicate which time unit color is changing.
    if (data == 'H' || data == 'M' || data == 'S' || data == 'A')
    {
      color_select = data;
    }

    // If the color selection mode is 'A', change background colors.
    if (color_select == 'A') {
      switch (data) {
        case 'r':
          bk_red = Serial.parseInt();
          dirty = true;
          break;
        case 'g':
          bk_green = Serial.parseInt();
          dirty = true;
          break;
        case 'b':
          bk_blue = Serial.parseInt();
          dirty = true;
          break;
      }
    }

    // If the color selection mode is hour, check the data for r, g, b.
    if (color_select == 'H') {
      switch (data) {
        case 'r':
          h_red_deger = Serial.parseInt();
          dirty = true;
          break;
        case 'g':
          h_green_deger = Serial.parseInt();
          dirty = true;
          break;
        case 'b':
          h_blue_deger = Serial.parseInt();
          dirty = true;
          break;
      }
    }

    // If the color selection mode is minute, check the data for r, g, b.
    if (color_select == 'M') {
      switch (data) {
        case 'r':
          m_red_deger = Serial.parseInt();
          dirty = true;
          break;
        case 'g':
          m_green_deger = Serial.parseInt();
          dirty = true;
          break;
        case 'b':
          m_blue_deger = Serial.parseInt();
          dirty = true;
          break;
      }
    }

    // If the color selection mode is minute, check the data for r, g, b.
    if (color_select == 'S') {
      switch (data) {
        case 'r':
          s_red_deger = Serial.parseInt();
          dirty = true;
          break;
        case 'g':
          s_green_deger = Serial.parseInt();
          dirty = true;
          break;
        case 'b':
          s_blue_deger = Serial.parseInt();
          dirty = true;
          break;
      }
    }


  } // End of serial loop.

  // If the colors have changed, update them in the eeprom.
  if (dirty) {
    updateColors();
    dirty = false;
  }

  // Get time from real time clock.
  t = rtc.getTime();
  hr = t.hour;
  mnt = t.min;
  sc = t.sec;

  // Convert time to 0-11.
  hr = positive_modulo(hr, 12);

  // Update display with colors.
  updateOutsidePixels(Adafruit_NeoPixel::Color(m_red_deger, m_green_deger, m_blue_deger),
                      Adafruit_NeoPixel::Color(s_red_deger, s_green_deger, s_blue_deger));
  updateHourPixels(Adafruit_NeoPixel::Color(h_red_deger, h_green_deger, h_blue_deger),
                   Adafruit_NeoPixel::Color(bk_red, bk_green, bk_blue));
}


void updateOutsidePixels(uint32_t minute_color, uint32_t second_color) {
  // Turn off all pixels.
  for (int k = 0; k < 60; k++) {
    pixels.setPixelColor(k, pixels.Color(0, 0, 0));
  }

  // Turn on every 4th mark around outside.
  for (int k = 0; k < 60; k++) {
    pixels.setPixelColor(k, pixels.Color(25, 25, 255));
    k = k + 4;
  }

  // Set second and minute pixels to their color.
  pixels.setPixelColor(sc, second_color);
  pixels.setPixelColor(mnt, minute_color);
  pixels.show();
}

void updateHourPixels(uint32_t current, uint32_t other) {
  // Set all hour pixels to color
  for (int i = 0; i < 12; i++) {
    pixels2.setPixelColor(i, other);
  }

  // Set the current hour to a different color.
  pixels2.setPixelColor(hr, current);
  pixels2.show();
}

void colorWipe(uint32_t c, uint8_t wait) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();

    strip2.setPixelColor(i, c);
    strip2.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256; j++) {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i + j) & 255));
      strip2.setPixelColor(i, Wheel((i + j) & 255));
    }
    strip2.show();

    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j = 0; j < 256 * 5; j++) { // 5 cycles of all colors on wheel
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
      strip2.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    strip2.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
        strip2.setPixelColor(i + q, c);
      }
      strip2.show();
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
        strip2.setPixelColor(i + q, 0);
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j = 0; j < 256; j++) {   // cycle all 256 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 255)); //turn every third pixel on
        strip2.setPixelColor(i + q, Wheel( (i + j) % 255));
      }
      strip2.show();
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
        strip2.setPixelColor(i + q, 0);
      }
    }
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
