
# RPI PICO  ssd1306 driver / lib with examples

this is hopefully a better graphics library for the common oled's ported from rpi and adafruit to rpi pico. for games gameplay and UIs. using basically the same code underneath but some higher level abstractions for convenience.

![](IMG_0089.jpg)

## instructions

1. plug your oled into pins 6 and 7 on your rpi (pins are configurable)
2. compile the example using the cmake file provided
3. drag it over to the usb device

## roadmap

- [x] refactored example oled code provided by the rpi people
- [ ] basic font code from the adafruit library
- [ ] graphics primitives from the adafruit library
  - [ ] line
  - [ ] triangle
  - [ ] rect
  - [ ] poly, circle, etc

- [ ] 2d game graphics
  - [ ] sprites
  - [ ] animations

- [ ] 2d image masking & compositing like cairo graphics
  - [ ] multiple draw surfaces
  - [ ] write/copy from one surface to another 
  - [ ] other logical operation (AND OR XOR NOT etc) from one surface to another

- [ ] grid 9 images for frames etc

- [ ] raycasting engine
- [ ] proper 3d engine

