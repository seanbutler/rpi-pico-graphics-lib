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

namespace Rendering {

    class AbstractRenderer {
        virtual void Render() = 0;
    };

    class GraphicsSurface_GenericBusDevice_Renderer : public AbstractRenderer
    {

    public:
        GraphicsSurface_GenericBusDevice_Renderer(uint8_t* surface,
                BusIO::Device::Generic *device)
                : surface_(surface)
                , device_(device)
        {

        }

        void SetSurface(uint8_t* surface){
            surface_ = surface;
        }

        void Render()
        {    
            device_->SendData( 128 * 64 / 8, surface_, 0x40);
            // device_->SendData(surface->GetSize(), surface->GetDataPtr());
        }

    private:
        uint8_t *                   surface_;
        BusIO::Device::Generic *    device_;
    };

}


// ----------------------------------------------------------------------
