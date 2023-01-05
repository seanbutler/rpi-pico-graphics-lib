
/*
 * Low Level Interface to SSD13XX
 * 
 * Based on (Inspired/Copied) Code... 
 * From RPi Foundation Pico Examples
 * Also from Adafruit Arduino SSD1306 Driver Code
 *
 * Also External documents and datasheets... 
 *
 * https://datasheethub.com/ssd1306-128x64-mono-0-96-inch-i2c-oled-display/ 
 * https://datasheethub.com/wp-content/uploads/2022/08/SSD1306.pdf
 * https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf  
 * 
 */

// ---------------------------------------------------------------------------

#ifndef SSD1306_LIB

#define SSD1306_LIB 

// ---------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ---------------------------------------------------------------------------

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// ---------------------------------------------------------------------------

#include <stdbool.h>

// ---------------------------------------------------------------------------

//
// FUNDAMENTAL COMMANDS 
//

#define SSD1306_SET_CONTRAST_CONTROL    _u(0x81)

#define SSD1306_SET_ENTIRE_DISPLAY_RAM  _u(0xA4)
#define SSD1306_SET_ENTIRE_DISPLAY_ON   _u(0xA5)

#define SSD1306_SET_NORMAL_DISPLAY      _u(0xA6)
#define SSD1306_SET_INVERSE_DISPLAY     _u(0xA7)

#define SSD1306_SET_DISPLAY_OFF         _u(0xAE)
#define SSD1306_SET_DISPLAY_ON          _u(0xAF)

//
// SCROLLING COMMANDS 
//

#define SSD1306_CONTINUOUS_SCROLL_HORIZONTAL_RIGHT  _u(0x26)
#define SSD1306_CONTINUOUS_SCROLL_HORIZONTAL_LEFT   _u(0x27)

#define SSD1306_CONTINUOUS_SCROLL_VERTICAL_RIGHT    _u(0x29)
#define SSD1306_CONTINUOUS_SCROLL_VERTICAL_LEFT     _u(0x2A)

#define SSD1306_SET_CONTINUOUS_SCROLL_DEACTIVATE    _u(0x2E)
#define SSD1306_SET_CONTINUOUS_SCROLL_ACTIVATE      _u(0x2F)

#define SSD1306_SET_VERTICAL_SCROLL_AREA            _u(0xA3)


//
// ADDRESSING SETTING COMMANDS 
//

#define SSD1306_SET_MEMORY_ADDRESSING_MODE        _u(0x20)

#define SSD1306_MEMORY_ADDRESSING_MODE_HORIZONTAL _u(0x00)
#define SSD1306_MEMORY_ADDRESSING_MODE_VERTICAL   _u(0x01)
#define SSD1306_MEMORY_ADDRESSING_MODE_PAGED      _u(0x02)
#define SSD1306_MEMORY_ADDRESSING_MODE_INVALID    _u(0x03)

#define SSD1306_SET_COLUMN_ADDRESS                _u(0x21)
#define SSD1306_SET_PAGE_ADDRESS                  _u(0x22)
#define SSD1306_SET_PAGE_START_ADDRESS            _u(0xB0)


//
// HARDWARE CONFIGURATION PANEL RESOLUTION AND LAYOUT
//

#define SSD1306_SET_DISPLAY_START_LINE            _u(0x40)
#define SSD1306_SET_SEGMENT_REMAP_ZERO            _u(0xA0)
#define SSD1306_SET_SEGMENT_REMAP_127             _u(0xA1)

#define SSD1306_SET_MULTIPLEX_RATIO               _u(0xA8)

#define SSD1306_SET_COM_OUTPUT_SCAN_DIR           _u(0xC0)
#define SSD1306_SET_COM_OUTPUT_SCAN_DIR_RIGHT     _u(0xC0)
#define SSD1306_SET_COM_OUTPUT_SCAN_DIR_LEFT      _u(0xC8)

#define SSD1306_SET_DISPLAY_OFFSET                _u(0xD3)

#define SSD1306_SET_COM_PIN_HARDWARE_CONFIG    _u(0xDA)
#define SSD1306_COM_PIN_HARDWARE_CONFIG_SEQ    _u(0b00000010) // TODO
#define SSD1306_COM_PIN_HARDWARE_CONFIG_ALT    _u(0b00010010) // TODO
#define SSD1306_COM_PIN_HARDWARE_DISABLE       _u(0b00000010) // TODO
#define SSD1306_COM_PIN_HARDWARE_ENABLE        _u(0b00100010) // TODO


//
// TIMING AND DRIVING SCHEME SETTING
// 

#define SSD1306_SET_DISPLAY_CLOCK_DIVIDE      _u(0xD5)
#define SSD1306_SET_PRECHARGE_PERIOD          _u(0xD9)

#define SSD1306_SET_VCOM_DESELECT_LEVEL       _u(0xDB)

#define SSD1306_VCOM_DESELECT_065_TIMES       _u(0b00000000)
#define SSD1306_VCOM_DESELECT_077_TIMES       _u(0b00100000)
#define SSD1306_VCOM_DESELECT_083_TIMES       _u(0b00110000)

#define SSD1306_NOP                           _u(0xE3)

// ---------------------------------------------------------------------------

//
// CHARGE PUMP COMMANDS
//

#define SSD1306_SET_CHARGE_PUMP             _u(0x8D)
#define SSD1306_CHARGE_PUMP_DISABLE         _u(0b00010000)
#define SSD1306_CHARGE_PUMP_ENABLE          _u(0b00010100)


// ---------------------------------------------------------------------------

#define SSD1306_WRITE_MODE             _u(0xFE)
#define SSD1306_READ_MODE              _u(0xFF)

#define OLED_WRITE_MODE             SSD1306_WRITE_MODE
#define OLED_READ_MODE              SSD1306_READ_MODE

// ---------------------------------------------------------------------------

// KEEP -- REFACTOR TO NEW Naming Scheme, 

// #define OLED_ADDR                   _u(0x3C)
#define OLED_ADDR                   _u(0x3D)            // parameterise this as can be detected by other lib

// #define OLED_HEIGHT                 _u(32)
#define OLED_HEIGHT                 _u(64)              // TODO parameterise for client code setting

#define OLED_WIDTH                  _u(128)             // TODO parameterise for client code setting

#define OLED_PAGE_HEIGHT            _u(8)               // TODO parameterise for client code setting

#define OLED_NUM_PAGES              (OLED_HEIGHT / OLED_PAGE_HEIGHT)
#define OLED_BUF_LEN                (OLED_NUM_PAGES * OLED_WIDTH)

// ---------------------------------------------------------------------------

#include "./pico_lib_SSD13XX_images.h"
#define SSD1306_TESTIMAGE testpattern_128x64x1

// ---------------------------------------------------------------------------

struct render_area {
    uint8_t start_col;
    uint8_t end_col;
    uint8_t start_page;
    uint8_t end_page;

    int buflen;
};

// ---------------------------------------------------------------------------

void fill(uint8_t buf[], uint8_t fill);
void fill_page(uint8_t *buf, uint8_t fill, uint8_t page);
void calc_render_area_buflen(struct render_area *area);
void oled_send_cmd(uint8_t cmd);
void oled_send_buf(uint8_t buf[], int buflen);

// ---------------------------------------------------------------------------

void oled_i2c_gpio_setup(unsigned long baud_rate, uint8_t sda_pin, uint8_t scl_pin, enum gpio_function gpio_func);

void oled_init();

void render(uint8_t *buf, struct render_area *area);

// ---------------------------------------------------------------------------

#endif /* SSD1306_LIB */


