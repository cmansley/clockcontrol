
# Install

1. Add DS3231.zip library via Sketch -> Include Library -> Add .zip Library
1. Add Adafruit Neopixel via Sketch -> Include Library -> Manage Libraries
  1. Used Adafruit NeoPixel by Adafruit (1.1.8)
1. In order to program, the bluetooth receiver must be disconnected.

# Bluetooth Serial Protocol

- `m` switches mode 
- `+` and `-` increment and decrement clock (when in the hour or minute set mode)
- `A` starts animation mode
- `H`, `M`, `S` starts the color selection for hour, minute or second
- `r`, `g`, `b` indicates the RGB channel followed by an integer to indicate value

- Examples:
  - m+m+--
  - Hr255g0b0Mr255g255b255
  - A