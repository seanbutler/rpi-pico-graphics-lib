//
// pico_busio.h
//
// To abstract away various ways of doing Bus IO Comms (i2c etc) on the rpi pico behind single interface
//

// ----------------------------------------------------------------------

#pragma once

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// ----------------------------------------------------------------------

namespace BusIO
{
    class Interface
    {
        // TODO put a generic interface to one or more devices here
    };

    namespace Device
    {
        class Generic
        {
        public:
            virtual void Initialise() = 0;
            virtual void SendCommand(uint8_t cmd) = 0;
            virtual void SendData(uint16_t len, uint8_t *data) = 0;
            virtual int8_t *ReadData(uint16_t len) = 0;
        };

        class SSD1306_OLED : public Generic
        {
        public:
            unsigned long baud_rate_;
            uint8_t sda_pin_;
            uint8_t scl_pin_;
            enum gpio_function gpio_func_;
            uint8_t write_flags_; // ssd1306 oled needs (OLED_ADDR & OLED_WRITE_MODE)

            SSD1306_OLED(unsigned long baud_rate,
                         uint8_t sda_pin,
                         uint8_t scl_pin,
                         enum gpio_function gpio_func,
                         uint8_t write_flags,
                         uint8_t cmd_pfix = 0x40)
                : baud_rate_(baud_rate), sda_pin_(sda_pin), scl_pin_(scl_pin), gpio_func_(gpio_func), write_flags_(write_flags), cmd_pfix_(cmd_pfix)
            {

                //
                // Device Specific Initialisation Code Here
                //
                i2c_init(i2c_default, baud_rate_);
                gpio_set_function(sda_pin, gpio_func);
                gpio_set_function(scl_pin, gpio_func);
                gpio_pull_up(sda_pin);
                gpio_pull_up(scl_pin);

                Initialise();
            }

            void Initialise()
            {
                //
                // Device Specific Initialisation Code Here
                //

                // some of these commands are not strictly necessary as the reset
                // process defaults to some of these but they are shown here
                // to demonstrate what the initialization sequence looks like

                // some configuration values are recommended by the board manufacturer

                SendCommand(SSD1306_SET_DISPLAY_OFF); // set display off

                /* memory mapping */
                SendCommand(SSD1306_SET_MEMORY_ADDRESSING_MODE);        // set memory address mode
                SendCommand(SSD1306_MEMORY_ADDRESSING_MODE_HORIZONTAL); // horizontal addressing mode

                /* resolution and layout */
                SendCommand(SSD1306_SET_DISPLAY_START_LINE);       // set display start line to 0
                SendCommand(SSD1306_SET_SEGMENT_REMAP_127);        // set segment re-map column address 127 is mapped to SEG0
                SendCommand(SSD1306_SET_MULTIPLEX_RATIO);          // set multiplex ratio
                SendCommand(OLED_HEIGHT - 1);                      // display height - 1
                SendCommand(SSD1306_SET_COM_OUTPUT_SCAN_DIR_LEFT); // set COM (common) output scan direction
                SendCommand(SSD1306_SET_DISPLAY_OFFSET);           // set display offset
                SendCommand(0x00);                                 // no offset

                SendCommand(SSD1306_SET_COM_PIN_HARDWARE_CONFIG); // set COM (common) pins hardware configuration

                if (OLED_HEIGHT <= _u(32))
                {
                    SendCommand(0x02); // TODO what is this?
                }
                else
                {
                    SendCommand(0x12);
                }

                /* timing and driving scheme */
                SendCommand(SSD1306_SET_DISPLAY_CLOCK_DIVIDE); // set display clock divide ratio
                SendCommand(0x80);                             // div ratio of 1, standard freq
                SendCommand(SSD1306_SET_PRECHARGE_PERIOD);     // set pre-charge period
                SendCommand(0xF1);                             // Vcc internally generated on our board
                SendCommand(SSD1306_SET_VCOM_DESELECT_LEVEL);  // set VCOMH deselect level
                SendCommand(SSD1306_VCOM_DESELECT_083_TIMES);  // 0.83xVcc

                /* display */
                SendCommand(SSD1306_SET_CONTRAST_CONTROL);     // set contrast control
                SendCommand(0xFF);
                SendCommand(SSD1306_SET_ENTIRE_DISPLAY_RAM);   // set entire display on to follow RAM content
                SendCommand(SSD1306_SET_NORMAL_DISPLAY);       // set normal (not inverse, B=W & W=B) display
                SendCommand(SSD1306_SET_CHARGE_PUMP);          // set charge pump
                SendCommand(0x14);                             // Vcc internally generated on our board

                /* scrolling safety */
                SendCommand(SSD1306_SET_CONTINUOUS_SCROLL_DEACTIVATE); // deactivate horizontal scrolling if set
                                                                        // this is necessary as memory writes will corrupt if scrolling was enabled

                SendCommand(SSD1306_SET_DISPLAY_ON); // turn display on ( surely this should be client option)
            }

            void SendCommand(uint8_t cmd)
            {
                cmd_buf_[1] = cmd;

                i2c_write_blocking(i2c_default,
                                   write_flags_,
                                   cmd_buf_,
                                   2,
                                   false);
            }

            void SendData(uint16_t len, uint8_t *data)
            {
                temp_buf_ = (uint8_t *)malloc(len + 1);

                // Co = 0, D/C = 1 => the driver expects data to be written to RAM
                temp_buf_[0] = 0x40; // TODO parameterise this 0x40 magic number

                for (int i = 1; i < len + 1; i++)
                {
                    temp_buf_[i] = data[i - 1];
                }

                i2c_write_blocking(i2c_default,
                                   write_flags_,
                                   temp_buf_,
                                   len + 1,
                                   false);

                free(temp_buf_);
            }

            int8_t *ReadData(uint16_t len)
            {
                static_assert("Write Only Device");
                return 0;
            };

        private:
            uint8_t cmd_pfix_;
            uint8_t cmd_buf_[2];
            uint8_t *temp_buf_;
        };
    }
}

// ----------------------------------------------------------------------
