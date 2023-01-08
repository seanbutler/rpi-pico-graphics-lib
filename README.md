
# RPI PICO Graphics Library 

 - header only 
 - lines, sprites, surfaces/masks
 - easy to include in your project, use cmake or just copy the header files
 - works with 1bit depth displays

![](IMG_0089.jpg)

## Motivation 

The existing rpi example code for oleds doesnt handle screens with sizes other than 128x32 without altering the code.
This code handles 128x62x1 screens more nicely. Could do with some parameterisation so can work on more hardware and also testing, but i dont have a other kinds of OLEDs to test on.

This is hopefully a better graphics library for the common oled's with ideas stolen from from rpi-sdk and adafruit-gfx. for games gameplay and UIs. using basically the others code underneath but some higher level abstractions for my convenience.


## Design/Use/Architecture

1. Create a Device


~~~c++
  BusIO::Device::SSD1306_OLED device(
    400 * 1000,                     //baud rate
    PICO_DEFAULT_I2C_SDA_PIN,       // pin
    PICO_DEFAULT_I2C_SCL_PIN,       // pin
    GPIO_FUNC_I2C,                  // sdk enum
    (OLED_ADDR & OLED_WRITE_MODE),  // sdk enum
    0x80);                          // manufacturer magic number
~~~

3. Create a Surface

~~~c++
  Rendering::Surface_1bit surface(128, 64);
~~~

4. Issue Drawing Commands to Surfaces

~~~c++
  surface.DrawLine(63, 0, 127, 32);   // right down
  surface.DrawLine(127, 32, 63, 63);  // left down
  surface.DrawLine(63, 63, 0, 32);    // left up
  surface.DrawLine(0, 32, 63, 0);     // right up
~~~


5. Use Renderer Class to Draw Surface to Device

~~~c++
  Rendering::GraphicsSurface_GenericBusDevice_Renderer renderer(
    surface.buffer, 
    &device);
~~~


## External documents and datasheets... 

https://datasheethub.com/ssd1306-128x64-mono-0-96-inch-i2c-oled-display/ 

https://datasheethub.com/wp-content/uploads/2022/08/SSD1306.pdf

https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf  

## Adafruit and Arduino

Lots of excellent work here on their Arudino libraries. These libs have grown and hold some interesting useful ideas. 

Hide the various hardware IO methods behind this... 
 - [Adafruit_BusIO](https://github.com/adafruit/Adafruit_BusIO) 

Nice big family of graphics methods here... 
 - [Adafruit-GFX-Library](https://github.com/adafruit/Adafruit-GFX-Library)

 They are specifically for Arduino and contain some Arduino-isms which i need to work around. 

__Possibly I should just fork the Adafruit libraries to make ports for RPI PICO? Would learn more by reading them and re-writing where necessary though.__

## instructions

1. plug your oled into pins 6 and 7 on your rpi (pins are configurable)
2. compile the example using the cmake file provided
3. drag it over to the usb device

## roadmap

**basic refactoring**

- [x] refactored example oled code provided by the rpi people

**architectural separation, devices, interface, renderers and graphics**

- [x] encapsulation of target device 
  - [x] hide bus io device code 
  - [x] common interface for extension

- [x] surface 
  - [x] surface as a destination for 2D ops & bit twiddling 

- [x] renderer 
  - [x] tell device to render whats at the memory  
  - [x] association between surface and devices 

- [x] graphics primitives (from the adafruit library?)
  - [x] pixel plot and unplot 
  - [x] line
  - [x] line rect
  - [x] line triangle
  - [ ] line poly/circle
  - [ ] fill triangle
  - [ ] fill rect
  - [ ] fill poly/circle

- [x] basic texture display

- [ ] 2d image masking & compositing like cairo graphics
  - [x] write/copy from one surface to another 
  - [ ] multiple draw surfaces
  - [ ] other logical operation (AND OR XOR NOT etc) from one surface to another
  - [ ] scaling between surfaces with logical operations

- [ ] Sprites
  - [x] sprites
  - [ ] animations
  - [ ] sprite sheet

- [ ] Constrained Optimised Sprites
  - [ ] byte aligned data reads in 1bit surface
  - [ ] linear reads, skipping writes

- [ ] 2d game graphics
  - [ ] sprites (see quad texture copy above)
  - [ ] animations
  - [ ] grid 9 images for frames etc

- [ ] more graphics primitives (from the adafruit library?)
  - [ ] fast ortho lines
  - [ ] line poly, circle, etc
  - [ ] fill poly, circle, etc

- [ ] raycasting engine
- [ ] proper 3d engine

- [ ] works with 1 bit depth devices
- [ ] works with 8 bit depth devices
- [ ] works with 16 bit depth devices

