#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ------------------------------------------------------------

#include "pico_lib_SSD13XX.h"
#include "pico_lib_busio.h"
#include "pico_lib_renderer.h"

// ------------------------------------------------------------

//
// this 'device' sets up the gpio and i2c to talk nicely to an ssd1306 oled
//

BusIO::Device::SSD1306_OLED oled_device(
        400*1000,
        PICO_DEFAULT_I2C_SDA_PIN,
        PICO_DEFAULT_I2C_SCL_PIN,
        GPIO_FUNC_I2C,
        (OLED_ADDR & OLED_WRITE_MODE), 
        0x80 );

uint8_t buf[OLED_BUF_LEN];

Rendering::GraphicsSurface_GenericBusDevice_Renderer renderer(
        buf, 
        &oled_device);

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
        fill(buf, pattern);
        renderer.Render();
    }

    return 0;
}

int demo_flash()
{
    // uint8_t buf[OLED_BUF_LEN];
    fill(buf, 0x00);
    // render(buf, &frame_area);
    renderer.Render();
    // oled_device.WriteData(OLED_BUF_LEN, buf);

    // intro sequence: flash the screen
    for (int i = 0; i < 5; i++)
    {
        oled_device.WriteCommand(0xA5);
        sleep_ms(200);
        oled_device.WriteCommand(0xA4);
        sleep_ms(200);
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
    // simple demo junk
    //

    calc_render_area_buflen(&frame_area);
    calc_render_area_buflen(&smaller_area);

    demo_stripes();
    demo_flash();
    demo_bitmap();
    demo_animate();

    return 0;
}

// ------------------------------------------------------------
