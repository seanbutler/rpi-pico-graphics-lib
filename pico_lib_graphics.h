//
// pico_renderer.h
//

// Provide an interface between data output from Graphcs Code and Devices

// ----------------------------------------------------------------------

#pragma once

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

// ----------------------------------------------------------------------

namespace Rendering
{
    class Surface
    {
    public:
        uint8_t w_;
        uint8_t h_;
        uint16_t size_;
        uint8_t *buffer;

        Surface(uint8_t w, uint8_t h)
            : w_(w), h_(h)
        {
        }

        virtual ~Surface()
        {
            if (buffer)
            {
                free(buffer);
            }
        }

        virtual void SetPixel(uint8_t x, uint8_t y) = 0;
        virtual void ClearPixel(uint8_t x, uint8_t y) = 0;
        virtual bool GetPixel(uint8_t x, uint8_t y) = 0;

        inline bool OutOfBounds(uint8_t x, uint8_t y)
        {
            return ((x < 0) || (y < 0) || (x >= w_) || (y >= h_));
        }
    };

    class Surface_1bit : public Surface
    {
        // NOTE this is specifically a 1bit surface, its more complex than a color one
        // TODO must rename it and create a abstract base class

    public:
        uint16_t size_;

        Surface_1bit(uint8_t w, uint8_t h)
            : Surface(w, h), size_((w * h) / 8)

        {
            // NOTE for the 1bit width and height must be multiples of 8
            // TODO perhaps this size and malloc should be in the base class?
            buffer = (uint8_t *)malloc(size_);

            for (uint16_t n = 0; n < size_; n++)
            {
                buffer[n] = (uint8_t)0x00;
            }
        }


        void Fill(uint8_t fill) 
        {
            for (int i = 0; i < size_; i++) {
                buffer[i] = fill;
            }
        }

        void SetPixel(uint8_t x, uint8_t y)
        {
            if (OutOfBounds(x, y))
                assert("Coords Out of Bounds of Buffer");

            uint8_t *addr = GetByte(x, y);
            *addr |= 1 << (y & 7);
        }

        virtual bool GetPixel(uint8_t x, uint8_t y) {
            uint8_t *addr = GetByte(x, y);
            return 0x01 & ( *addr >> (y & 7) );
        }

        void ClearPixel(uint8_t x, uint8_t y)
        {
            if (OutOfBounds(x, y))
                assert("Coords Out of Bounds of Buffer");

            uint8_t *addr = GetByte(x, y);
            *addr &= ~(1 << (y & 7));
        }

        void DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
        {
            // TODO put in code here to handle special fast cases
            _DrawLine(x0, y0, x1, y1);
        }

        void DrawRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
        {
            // TODO put in code here to handle special fast cases
            _DrawLine(x0, y0, x1, y0);
            _DrawLine(x1, y0, x1, y1);
            _DrawLine(x1, y1, x0, y1);
            _DrawLine(x0, y1, x0, y0);
        }

        void DrawTriangle(uint8_t x0, uint8_t y0, 
                            uint8_t x1, uint8_t y1,
                            uint8_t x2, uint8_t y2)
        {
            _DrawLine(x0, y0, x1, y1);
            _DrawLine(x1, y1, x2, y2);
            _DrawLine(x2, y2, x0, y0);
        }



        void DrawBitmap(Surface * src,
                        Surface * dest,
                        uint8_t dest_x, uint8_t dest_y )
        {
            // TODO optimise this
            for ( uint8_t current_src_y = 0; current_src_y < src->h_; current_src_y++ ) {
                for ( uint8_t current_src_x = 0; current_src_x < src->w_; current_src_x++ ) {

                    uint8_t current_dest_x = dest_x + current_src_x;
                    uint8_t current_dest_y = dest_y + current_src_y;

                    if ( src->GetPixel(current_src_x, current_src_y) ) {
                        dest->SetPixel(current_dest_x, current_dest_y);
                    }

                }
            }
        }

    protected:

        // Bresenhams from here https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

#define _swap_int8_t(x0, y0) \
    {                        \
        uint8_t t = x0;      \
        x0 = y0;             \
        y0 = t;              \
    }

        void _DrawLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
        {
            if (OutOfBounds(x0, y0) || OutOfBounds(x1, y1))
                assert("Coords Out of Bounds of Buffer");

            int16_t steep = abs(y1 - y0) > abs(x1 - x0);
            if (steep)
            {
                _swap_int8_t(x0, y0);
                _swap_int8_t(x1, y1);
            }

            if (x0 > x1)
            {
                _swap_int8_t(x0, x1);
                _swap_int8_t(y0, y1);
            }

            int8_t delta_x = x1 - x0;
            int8_t delta_y = abs(y1 - y0);

            int8_t err = delta_x / 2;

            int8_t ystep;
            if (y0 < y1)
            {
                ystep = 1;
            }
            else
            {
                ystep = -1;
            }

            for (; x0 < x1; x0++)
            {
                if (steep)
                {
                    SetPixel(y0, x0);
                }
                else
                {
                    SetPixel(x0, y0);
                }
                err -= delta_y;
                if (err < 0)
                {
                    y0 += ystep;
                    err += delta_x;
                }
            }
        }

        inline uint8_t *GetByte(uint8_t x, uint8_t y)
        {   
            const uint8_t pixels_per_byte = 8;
            return &buffer[x + ((y / pixels_per_byte) * w_)];
        }
    };

}

// ----------------------------------------------------------------------
