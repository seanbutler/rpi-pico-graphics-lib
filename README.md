
# RPI PICO ssd1306 oled lib (with examples)

## Motivation 

The existing rpi example code for oleds doesnt handle screens with sizes other than 128x32 without altering the code.
This code handles 128x62x1 screens more nicely. Could do with some parameterisation so can work on more hardware and also testing, but i dont have a other kinds of OLEDs to test on.

This is hopefully a better graphics library for the common oled's with ideas stolen from from rpi-sdk and adafruit-gfx. for games gameplay and UIs. using basically the others code underneath but some higher level abstractions for my convenience.

![](IMG_0089.jpg)
 
 
## External documents and datasheets... 

https://datasheethub.com/ssd1306-128x64-mono-0-96-inch-i2c-oled-display/ 

https://datasheethub.com/wp-content/uploads/2022/08/SSD1306.pdf

https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf  


## Adafruit and Arduino

Lots of excellent work here: Abstracting away the various IO methods available on the Arduino. Providing a basic but usable generic graphics library. 

Hide the various hardware IO methods behind this... 
 - [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO) 

Nice big family of graphics methods here... 

 - [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)

These libs have grown and hold some interesting usaful ideas.  They are specifically for Arduino and contain many Arduino-isms which i need to work around. Must consider how to do that. 

**Possibly I need to FORK these two libraries to make ports for RPI PICO?**

## instructions

1. plug your oled into pins 6 and 7 on your rpi (pins are configurable)
2. compile the example using the cmake file provided
3. drag it over to the usb device

## roadmap

**basic refactoring**

- [x] refactored example oled code provided by the rpi people


**architectural separation, devices, interface, renderers and graphics**

- [x] encapsulation of target device 
  - [x] hide bus io code 
  - [x] common interface for extension

- [x] surface 
  - [x] surface as a destination for 2D bit twiddling 

- [x] renderer 
  - [x] tell device to render whats at the memory  
  - [x] association between surface and devices 

- [ ] graphics primitives (from the adafruit library?)
  - [x] pixel plot and unplot 
  - [x] line
  - [ ] fast ortho line
  - [ ] triangle
  - [ ] rect
  - [ ] poly, circle, etc

- [x] basic texture display the adafruit library
- [ ] basic font code from the adafruit library

- [ ] More GFX lib
  - [ ] sprites
  - [ ] animations

- [ ] 2d game graphics
  - [ ] sprites
  - [ ] animations

- [ ] 2d image masking & compositing like cairo graphics
  - [ ] multiple draw surfaces
  - [ ] write/copy from one surface to another 
  - [ ] other logical operation (AND OR XOR NOT etc) from one surface to another
  - [ ] scaling between surfaces when operations

- [ ] grid 9 images for frames etc

- [ ] raycasting engine
- [ ] proper 3d engine

