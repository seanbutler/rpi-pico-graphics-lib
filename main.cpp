#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ------------------------------------------------------------

#include "pico_lib_SSD13XX.h"
#include "pico_lib_busio.h"    // devices
#include "pico_lib_renderer.h" //
#include "pico_lib_graphics.h"
#include "pico_lib_testimages.h"


// ------------------------------------------------------------

//
// this 'device' sets up the gpio and i2c to talk nicely to an ssd1306 oled
//

BusIO::Device::SSD1306_OLED oled_device(
    400 * 1000,
    PICO_DEFAULT_I2C_SDA_PIN,
    PICO_DEFAULT_I2C_SCL_PIN,
    GPIO_FUNC_I2C,
    (OLED_ADDR & OLED_WRITE_MODE),
    0x80);

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


int demo_flash()
{
    fill(buf, 0x00);
    renderer.Render();

    // intro sequence: flash the screen
    for (int i = 0; i < 5; i++)
    {
        oled_device.WriteCommand(0xA5);
        sleep_ms(100);

        oled_device.WriteCommand(0xA4);
        sleep_ms(100);
    }

    return 0;
}

// ------------------------------------------------------------

int demo_bitmap()
{
    for (int n = 0; n < 128 * 64 / 8; n++)
    {
        // buf[n] = charmap_cellphone_white[n];
        buf[n] = charmap_futuristic_black[n];
    }

    renderer.Render();
    return 0;
}


// ------------------------------------------------------------

int demo_plot()
{
    Rendering::Surface_1bit surface(128, 64);

    Rendering::GraphicsSurface_GenericBusDevice_Renderer renderer(
        surface.buffer,
        &oled_device);

    surface.DrawLine(63, 0, 127, 32);  // right down
    surface.DrawLine(127, 32, 63, 63); // left down

    surface.DrawLine(63, 63, 0, 32); // left up
    surface.DrawLine(0, 32, 63, 0);  // right up

    for (int n = 0; n < 3; n++)
    {
        surface.DrawRect(0 + n * 8, 0 + n * 8, 127 - n * 8, 63 - +n * 8);
    }

    for (int n = 32; n <= 96; n += 32)
    {
        surface.DrawTriangle(n, 24,
                             n + 8, 40,
                             n - 8, 40);
    }

    renderer.Render();
    sleep_ms(200);

    return 0;
}

// ------------------------------------------------------------

int demo_animate()
{
    for (int n = 60; n > 0; n--)
    {
        for (int n = 0; n < 128 * 64 / 8; n++)
        {
            buf[n] = testpattern_128x64x1[n];
        }

        renderer.Render();

        for (int n = 0; n < 128 * 64 / 8; n++)
        {
            buf[n] = testimage_128x64x1[n];
        }

        renderer.Render();
    }

    return 0;
}

// ------------------------------------------------------------


// 'man', 16x16px
const unsigned char man_sprite [32] = {
	0x00, 0x00, 0x00, 0x80, 0xc0, 0xe6, 0x6f, 0xff, 0xfd, 0xef, 0x62, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc3, 0xe3, 0x70, 0x3f, 0x1f, 0x0f, 0xff, 0xf8, 0x83, 0x07, 0x00, 0x00, 0x00
};

int demo_sprite()
{
    Rendering::Surface_1bit surface(128, 64);
    Rendering::Surface_1bit sprite(16, 16);

    memcpy(sprite.buffer, man_sprite, 32);

    for (int n=0;n<4;n++)
    {
        surface.DrawBitmap(&sprite, &surface, (n*16+n)+0, 0);
        surface.DrawBitmap(&sprite, &surface, (n*16+n)+16, 16);
        surface.DrawBitmap(&sprite, &surface, (n*16+n)+32, 32);
        surface.DrawBitmap(&sprite, &surface, (n*16+n)+48, 48);
    }

    Rendering::GraphicsSurface_GenericBusDevice_Renderer renderer(
        surface.buffer,
        &oled_device);

    renderer.Render();
    sleep_ms(200);

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

    // calc_render_area_buflen(&frame_area);
    // calc_render_area_buflen(&smaller_area);

    // demo_flash();

    demo_plot();

    // demo_animate();

    // demo_bitmap();

    demo_sprite();


    return 0;
}

// ------------------------------------------------------------
