#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// ------------------------------------------------------------

#include "pico_lib_SSD1306.h"

#include "pico_lib_busio.h"    
#include "pico_lib_renderer.h" 
#include "pico_lib_graphics.h"
#include "pico_lib_testimages.h"


// ------------------------------------------------------------

//
// this DEVICE sets up the gpio and i2c to talk nicely to an ssd1306 oled
//

BusIO::Device::SSD1306_OLED device(
    400 * 1000,
    PICO_DEFAULT_I2C_SDA_PIN,
    PICO_DEFAULT_I2C_SCL_PIN,
    GPIO_FUNC_I2C,
    (OLED_ADDR & OLED_WRITE_MODE),
    0x80);

//
// a SURFACE is where we do our actual drawing commands
// 

Rendering::Surface_1bit surface(128, 64);

//
// a RENDERER will when asked 'render' the surface to the device
//

Rendering::GraphicsSurface_GenericBusDevice_Renderer renderer(
    surface,
    device);

// ------------------------------------------------------------

// Rendering::Surface_8bit surface2(128, 64);

// Rendering::GraphicsSurface_GenericBusDevice_Renderer renderer2(
//     surface2.buffer,
//     &device);

// ------------------------------------------------------------

// int demo_gradient()
// {
//     for (int i = 0; i < 128; i++)
//     {
//         surface2.SetColour(i*2);

//         for (int j = 0; i < 64; j++)
//         {
//             surface2.SetPixel(i, j);
//         }
//     }

//     renderer2.Render();

//     return 0;
// }

// ------------------------------------------------------------


int demo_flash()
{
    // flash whatever is already on the screen
    // this directly accesses the device and sends it an invert code

    for (int i = 0; i < 50; i++)
    {
        device.SendCommand(SSD1306_SET_NORMAL_DISPLAY);
        sleep_ms(10);

        device.SendCommand(SSD1306_SET_INVERSE_DISPLAY);
        sleep_ms(10);
    }

    device.SendCommand(SSD1306_SET_NORMAL_DISPLAY);

    return 0;
}

// ------------------------------------------------------------

int demo_plot()
{
    surface.DrawLine(63, 0, 127, 32);   // right down
    surface.DrawLine(127, 32, 63, 63);  // left down
    surface.DrawLine(63, 63, 0, 32);    // left up
    surface.DrawLine(0, 32, 63, 0);     // right up
 
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

    return 0;
}

// ------------------------------------------------------------

int demo_animate()
{
    for (int n = 20; n > 0; n--)
    {
        for (int n = 0; n < 128 * 64 / 8; n++)
        {
            surface.buffer[n] = testpattern_128x64x1[n];
        }

        renderer.Render();

        for (int n = 0; n < 128 * 64 / 8; n++)
        {
            surface.buffer[n] = testimage_128x64x1[n];
        }

        renderer.Render();
    }

    return 0;
}

// ------------------------------------------------------------

int demo_font()
{
    for (int n = 0; n < 128 * 64 / 8; n++)
    {
        surface.buffer[n] = charmap_futuristic_black[n];
    }

    renderer.Render();
    return 0;
}

// ------------------------------------------------------------

// 'person', 16x16px
const unsigned char person_sprite [32] = {
	0x00, 0x00, 0x00, 0x80, 0xc0, 0xe6, 0x6f, 0xff, 0xfd, 0xef, 0x62, 0xc0, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0xc3, 0xe3, 0x70, 0x3f, 0x1f, 0x0f, 0xff, 0xf8, 0x83, 0x07, 0x00, 0x00, 0x00
};

int demo_sprite()
{
    Rendering::Surface_1bit sprite(16, 16);

    memcpy(sprite.buffer, person_sprite, 32);

    for (int n=0;n<4;n++)
    {
        surface.DrawBitmap(&sprite, &surface, (n*16)+32, 0);
        surface.DrawBitmap(&sprite, &surface, (n*16)+32, 16);
        surface.DrawBitmap(&sprite, &surface, (n*16)+32, 32);
        surface.DrawBitmap(&sprite, &surface, (n*16)+32, 48);
    }

    renderer.Render();

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
    // simple 1bit demo junk
    //

    surface.RawFill(0x00); 
    demo_animate();
    sleep_ms(1000);

    surface.RawFill(0x00); 
    demo_plot();
    sleep_ms(1000);

    surface.RawFill(0x00);
    demo_font();
    sleep_ms(1000);

    surface.RawFill(0x00);
    demo_sprite();
    sleep_ms(1000);

    surface.RawFill(0x00);
    demo_flash();
    sleep_ms(1000);


    // 8 bit demo junk

    // surface2.RawFill(0xFF);
    // demo_gradient();
    // sleep_ms(1000);


    return 0;
}

// ------------------------------------------------------------
