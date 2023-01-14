//
// pico_renderer.h
//

// Provide an interface between data output from Graphcs Code and Devices

// ----------------------------------------------------------------------

#pragma once

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"


#include "pico_lib_graphics.h"

// ----------------------------------------------------------------------

namespace Rendering {

    class AbstractRenderer {
        public:
        AbstractRenderer(const Surface & surface, const BusIO::Device::Generic & device)
                : surface_(surface)
                , device_(device)
        {

        }

        virtual void Render() const = 0;

        protected:
            const Surface & surface_;
            const BusIO::Device::Generic & device_;

    };


    class GraphicsSurface_GenericBusDevice_Renderer : public AbstractRenderer
    {

    public:
        GraphicsSurface_GenericBusDevice_Renderer(const Surface & surface, const BusIO::Device::Generic & device)
            : AbstractRenderer(surface, device)
        {

        }


        void Render() const
        {    
            device_.SendData(surface_.GetSize(), surface_.GetBuffer(), 0x40);
        }

    };

}


// ----------------------------------------------------------------------
