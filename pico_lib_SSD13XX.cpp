
// --------------------------------------------------

#include "pico_lib_SSD13XX.h"


// TODO fix these so they work on surfaces and move to andother file then delete this file?

// --------------------------------------------------


void fill(uint8_t buf[], uint8_t fill) 
{
    for (int i = 0; i < OLED_BUF_LEN; i++) {
        buf[i] = fill;
    }
}

// --------------------------------------------------


void fill_page(uint8_t *buf, 
                uint8_t fill, 
                uint8_t page) 
{
    // fill entire page with the same byte
    memset(buf + (page * OLED_WIDTH), fill, OLED_WIDTH);
}

// --------------------------------------------------


void calc_render_area_buflen(struct render_area *area) {
    // calculate how long the flattened buffer will be for a render area
    area->buflen = (area->end_col - area->start_col + 1) * (area->end_page - area->start_page + 1);
}


// --------------------------------------------------
