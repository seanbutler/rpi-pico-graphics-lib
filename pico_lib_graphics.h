//
// pico_renderer.h
//

// Provide an interface between data output from Graphcs Code and Devices

// ----------------------------------------------------------------------

#pragma once

#include <initializer_list>
#include <array>
#include <utility>

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "swap.hpp"

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
        const uint8_t bit_depth_;

        Surface(uint8_t w, uint8_t h, uint8_t d)
            : w_(w), h_(h)
            , bit_depth_(d)
            , size_((w * h) / (8/d))
        {
            buffer = (uint8_t *)malloc(size_);
        }

        virtual ~Surface()
        {
            if (buffer)
            {
                free(buffer);
            }
        }

        void RawFill(uint8_t fill)
        {
            for (uint16_t i = 0; i < size_; i++)
            {
                buffer[i] = fill;
            }
        }

        uint16_t GetSize() const 
        { 
            return size_;
        }

        uint8_t* GetBuffer() const 
        { 
            return buffer;
        }

        virtual void SetPixel(uint8_t x, uint8_t y) = 0;
        virtual void ClearPixel(uint8_t x, uint8_t y) = 0;
        virtual bool GetPixel(uint8_t x, uint8_t y) = 0;


        uint8_t x_, y_;
        inline void To( uint8_t x, uint8_t y) { x_ = x; y_ = y; }
        
        void LineTo( uint8_t x, uint8_t y) {
            Line(x_, y_, x, y);
            To(x, y);
        }

        void LineRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
        {
            // TODO put in code here to handle special fast cases

            Line(x0, y0, x1, y0);
            Line(x1, y0, x1, y1);
            Line(x1, y1, x0, y1);
            Line(x0, y1, x0, y0);
        }

        void FillRect(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
        {
            // TODO put in code here to handle special fast cases

            Line(x0, y0, x1, y0);
            Line(x1, y0, x1, y1);
            Line(x1, y1, x0, y1);
            Line(x0, y1, x0, y0);
        }



        void LineTriangle(uint8_t x0, uint8_t y0,
                          uint8_t x1, uint8_t y1,
                          uint8_t x2, uint8_t y2)
        {
            To(x0, y0);
            LineTo(x1, y1);
            LineTo(x2, y2);
            LineTo(x0, y0);
        }



        void LineSequence( uint8_t* buffer, uint8_t size)
        {

            To(buffer[0], buffer[1]);

            for(uint16_t n=2;n<size; n+=2) {
                LineTo(buffer[n], buffer[n+1]);
            }

            LineTo(buffer[0], buffer[1]);
        }

        inline void LinePolygon( uint8_t* buffer, uint8_t size)
        {
            LineSequence(buffer, size);
            LineTo(buffer[0], buffer[1]);
        }


        void DrawBitmap(Surface *src,
                        Surface *dest,
                        uint8_t dest_x, uint8_t dest_y)
        {
            // TODO optimise this
            for (uint8_t current_src_y = 0; current_src_y < src->h_; current_src_y++)
            {
                for (uint8_t current_src_x = 0; current_src_x < src->w_; current_src_x++)
                {
                    uint8_t current_dest_x = dest_x + current_src_x;
                    uint8_t current_dest_y = dest_y + current_src_y;

                    if (src->GetPixel(current_src_x, current_src_y))
                    {
                        dest->SetPixel(current_dest_x, current_dest_y);
                    }
                }
            }
        }




        // Bresenhams from here https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

        void Line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
        {
            // TODO put in code here to handle special fast cases

            if (OutOfBounds(x0, y0) || OutOfBounds(x1, y1))
                assert("Coords Out of Bounds of Buffer");

            int16_t steep = abs(y1 - y0) > abs(x1 - x0);
            if (steep)
            {
                sb::swap(x0, y0);
                sb::swap(x1, y1);
            }

            if (x0 > x1)
            {
                sb::swap(x0, x1);
                sb::swap(y0, y1);
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

        inline bool OutOfBounds(uint8_t x, uint8_t y)
        {
            return ((x < 0) || (y < 0) || (x >= w_) || (y >= h_));
        }

        inline uint8_t *GetByte(uint8_t x, uint8_t y)
        {
            return &buffer[x + ((y / (8/bit_depth_)) * w_)];
        }

    };

    // ----------------------------------------------------------------------

    class Surface_1bit : public Surface
    {
        // NOTE this is specifically a 1bit surface, its more complex than an 8 bit one
        // NOTE for the 1bit width and height must be multiples of 8

    public:

        Surface_1bit(uint8_t w, uint8_t h)
            : Surface(w, h, 1)
        {
        }

        void SetPixel(uint8_t x, uint8_t y)
        {
            if (OutOfBounds(x, y))
                assert("Coords Out of Bounds of Buffer");

            uint8_t *addr = GetByte(x, y);
            *addr |= 1 << (y & 7);
        }

        virtual bool GetPixel(uint8_t x, uint8_t y)
        {
            uint8_t *addr = GetByte(x, y);
            return 0x01 & (*addr >> (y & 7));
        }

        void ClearPixel(uint8_t x, uint8_t y)
        {
            if (OutOfBounds(x, y))
                assert("Coords Out of Bounds of Buffer");

            uint8_t *addr = GetByte(x, y);
            *addr &= ~(1 << (y & 7));
        }
    };

    // ----------------------------------------------------------------------

    class Surface_8bit : public Surface
    {
        // NOTE this is an 8bit surface, one byte per pixel

    public:

        Surface_8bit(uint8_t w, uint8_t h)
            : Surface(w, h, 8)
        {
            // this isnt really optimal to include here, should be optional
            for (uint16_t n = 0; n < size_; n++)
            {
                buffer[n] = (uint8_t)0x00;
            }
        }

        void Fill(uint8_t fill)
        {
            for (int i = 0; i < size_; i++)
            {
                buffer[i] = fill;
            }
        }

        void SetPixel(uint8_t x, uint8_t y)
        {
            if (OutOfBounds(x, y)) assert("Coords Out of Bounds of Buffer");

            buffer[(y*w_)+x]=colour_;
        }

        virtual bool GetPixel(uint8_t x, uint8_t y)
        {
            if (OutOfBounds(x, y)) assert("Coords Out of Bounds of Buffer");

            return buffer[(y*w_)+x];
        }

        void ClearPixel(uint8_t x, uint8_t y)
        {
            SetPixel(x, y);
        }

        void SetColour(uint8_t c=0xFF) {
            colour_ = c;
        }


    protected:

        inline uint8_t *GetByte(uint8_t x, uint8_t y)
        {
            return &buffer[x + ((y) * w_)];
        }

        u_int8_t colour_;
    };

}

// ----------------------------------------------------------------------
