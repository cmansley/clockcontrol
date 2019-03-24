
#include <DS3231.h>
#include <Adafruit_NeoPixel.h>
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

// Current hour color.
int h_red_deger = 255;
int h_green_deger = 0;
int h_blue_deger = 0;

// Current minute color.
int m_red_deger = 0;
int m_green_deger = 255;
int m_blue_deger = 0;

// Current second color.
int s_red_deger = 255;
int s_green_deger = 255;
int s_blue_deger = 0;

// All other hours color.
int bk_red = 0;
int bk_green = 255;
int bk_blue = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(12, PIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(12, PIN2, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  rtc.begin();

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
          break;
        case 'g':
          bk_green = Serial.parseInt();
          break;
        case 'b':
          bk_blue = Serial.parseInt();
          break;
      }
    }

    // If the color selection mode is hour, check the data for r, g, b.
    if (color_select == 'H') {
      switch (data) {
        case 'r':
          h_red_deger = Serial.parseInt();
          break;
        case 'g':
          h_green_deger = Serial.parseInt();
          break;
        case 'b':
          h_blue_deger = Serial.parseInt();
          break;
      }
    }

    // If the color selection mode is minute, check the data for r, g, b.
    if (color_select == 'M') {
      switch (data) {
        case 'r':
          m_red_deger = Serial.parseInt();
          break;
        case 'g':
          m_green_deger = Serial.parseInt();
          break;
        case 'b':
          m_blue_deger = Serial.parseInt();
          break;
      }
    }

    // If the color selection mode is minute, check the data for r, g, b.
    if (color_select == 'S') {
      switch (data) {
        case 'r':
          s_red_deger = Serial.parseInt();
          break;
        case 'g':
          s_green_deger = Serial.parseInt();
          break;
        case 'b':
          s_blue_deger = Serial.parseInt();
          break;
      }
    }

    // Update display with colors.
    updateOutsidePixels(Adafruit_NeoPixel::Color(m_red_deger, m_green_deger, m_blue_deger),
                        Adafruit_NeoPixel::Color(s_red_deger, s_green_deger, s_blue_deger));
    updateHourPixels(Adafruit_NeoPixel::Color(h_red_deger, h_green_deger, h_blue_deger),
                     Adafruit_NeoPixel::Color(bk_red, bk_green, bk_blue));
  } // End of serial loop.

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
