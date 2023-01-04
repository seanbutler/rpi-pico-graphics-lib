#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ------------------------------------------------------------------------------------------------------------------------

#include "pico_lib_SSD13XX.h"

// ------------------------------------------------------------

// #define OLED_WIDTH        128
// #define OLED_HEIGHT       64
// #define OLED_PAGE_HEIGHT  8
// #define OLED_NUM_PAGES (OLED_HEIGHT/OLED_PAGE_HEIGHT)

// ------------------------------------------------------------

// initialize render area for entire frame (128 pixels by N(8) pages)
struct render_area frame_area =
{
    start_col : 0,
    end_col : OLED_WIDTH - 1,
    start_page : 0,
    end_page : OLED_NUM_PAGES - 1
};


struct render_area smaller_area =
{
    start_col : 32,
    end_col : 96,
    start_page : 2,
    end_page : 6
};


int demo_stripes()
{
    const unsigned int incre = 0x08;
    const unsigned int max = 0xFE;

    for (__uint8_t pattern = 0; pattern < max - incre; pattern += incre)
    {
        uint8_t buf[OLED_BUF_LEN];
        fill(buf, pattern);
        render(buf, &frame_area);
    }

    return 0;
}

int demo_flash()
{
    uint8_t buf[OLED_BUF_LEN];
    fill(buf, 0x00);
    render(buf, &frame_area);

    // intro sequence: flash the screen
    for (int i = 0; i < 5; i++)
    {
        oled_send_cmd(0xA5); // ignore RAM, all pixels on
        sleep_ms(100);
        oled_send_cmd(0xA4); // go back to following RAM
        sleep_ms(100);
    }

    return 0;
}

// ------------------------------------------------------------

int demo_bitmap()
{
    render(testpattern_128x64x1, &frame_area);
    return 0;
}

// ------------------------------------------------------------

int demo_animate()
{
    for (int n = 60; n > 0; n--)
    {
        render(testpattern_128x64x1, &frame_area);
        render(ssd1306_seanglasses, &frame_area);
    }

    return 0;
}

// ------------------------------------------------------------

int main()
{
    stdio_init_all();

    // useful information for picotool
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN,
                               PICO_DEFAULT_I2C_SCL_PIN,
                               GPIO_FUNC_I2C));

    bi_decl(bi_program_description("OLED I2C example for the Raspberry Pi Pico"));

    //
    // setup the gpio to do i2c for the oled
    //

    oled_i2c_gpio_setup(400 * 1000,                 // baud rate
                        PICO_DEFAULT_I2C_SDA_PIN,
                        PICO_DEFAULT_I2C_SCL_PIN,
                        GPIO_FUNC_I2C); 

    //
    // oled module needs some initialisation functions
    //
    
    oled_init();

    //
    // simple demo junk
    //

    calc_render_area_buflen(&frame_area);
    calc_render_area_buflen(&smaller_area);

    // demo_stripes();
    // demo_flash();
    demo_bitmap();
    demo_animate();

    return 0;
}

// ------------------------------------------------------------
