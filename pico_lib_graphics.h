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

    class Vector2D
    {
        static const uint8_t width = 0;
        static const uint8_t x = 0;
        static const uint8_t height = 1;
        static const uint8_t y = 1;

        uint8_t v[2];
    };


    class Surface
    {
    public:
        uint8_t w_;
        uint8_t h_;
        uint8_t *buffer;

        Surface(uint8_t w, uint8_t h)
            : w_(w)
            , h_(h)
        {
            
        }


        virtual ~Surface()
        {
            if (buffer)
            {
                free(buffer);
            }
        }


        virtual void SetPixel(uint8_t x, uint8_t y)=0;
        virtual void ClearPixel(uint8_t x, uint8_t y)=0;
        
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
            : Surface(w, h)
        {
            // NOTE width and height must be multiples of 8
            size_ = ((w_ / 8) * h_);
            buffer = (uint8_t*) malloc( size_);
            for(uint16_t n=0;n<size_; n++)
            {
                buffer[n]=0x00;
            }
        }

        void SetPixel(uint8_t x, uint8_t y)
        {
            if (OutOfBounds(x, y))
                assert("Coords Out of Bounds of Buffer");

            uint8_t *addr = GetByte(x, y);
            *addr |= 1 << (y & 7);
        }

        void ClearPixel(uint8_t x, uint8_t y)
        {
            if (OutOfBounds(x, y))
                assert("Coords Out of Bounds of Buffer");

            uint8_t *addr = GetByte(x, y);
            *addr &= ~(1 << (y & 7));
        }

    protected:
        inline uint8_t *GetByte(uint8_t x, uint8_t y)
        {
            return & buffer [ x + ( ( y / 8 ) * w_ )  ];
        }
    };
}

// ----------------------------------------------------------------------
