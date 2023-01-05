
// --------------------------------------------------

#include "pico_lib_SSD13XX.h"

// --------------------------------------------------

void fill(uint8_t buf[], uint8_t fill) 
{
    for (int i = 0; i < OLED_BUF_LEN; i++) {
        buf[i] = fill;
    }
}

void fill_page(uint8_t *buf, 
                uint8_t fill, 
                uint8_t page) 
{
    // fill entire page with the same byte
    memset(buf + (page * OLED_WIDTH), fill, OLED_WIDTH);
}

void calc_render_area_buflen(struct render_area *area) {
    // calculate how long the flattened buffer will be for a render area
    area->buflen = (area->end_col - area->start_col + 1) * (area->end_page - area->start_page + 1);
}

// --------------------------------------------------

void oled_send_cmd(uint8_t cmd) {
    // I2C write process expects a control byte followed by data
    // this "data" can be a command or data to follow up a command

    // Co = 1, D/C = 0 => the driver expects a command
    uint8_t buf[2] = {0x80, cmd};
    i2c_write_blocking(i2c_default, 
        (OLED_ADDR & OLED_WRITE_MODE), 
        buf, 
        2, 
        false);
}

// --------------------------------------------------

void oled_send_buf(uint8_t buf[], int buflen) {
    // in horizontal addressing mode, the column address pointer auto-increments
    // and then wraps around to the next page, so we can send the entire frame
    // buffer in one gooooooo!

    // copy our frame buffer into a new buffer because we need to add the control byte
    // to the beginning

    // TODO find a more memory-efficient way to do this..
    // maybe break the data transfer into pages?
    uint8_t *temp_buf = (uint8_t *)malloc(buflen + 1);

    for (int i = 1; i < buflen + 1; i++) {
        temp_buf[i] = buf[i - 1];
    }
    // Co = 0, D/C = 1 => the driver expects data to be written to RAM
    temp_buf[0] = 0x40;
    i2c_write_blocking(i2c_default, 
        (OLED_ADDR & OLED_WRITE_MODE), 
        temp_buf, 
        buflen + 1, 
        false);

    free(temp_buf);
}

// --------------------------------------------------

//
// oled_i2c_gpio_setup
// 
// SETUP I2C so it works for the OLED on specific pins
// I2C is "open drain", pull ups to keep signal high when no data is being sent
//

void oled_i2c_gpio_setup(unsigned long baud_rate,
                        uint8_t sda_pin,
                        uint8_t scl_pin,
                        enum gpio_function gpio_func) 
{
    return;

    i2c_init(i2c_default, baud_rate);
    gpio_set_function(sda_pin, gpio_func);
    gpio_set_function(scl_pin, gpio_func);
    gpio_pull_up(sda_pin);
    gpio_pull_up(scl_pin);
}

// --------------------------------------------------

void oled_init() 
{
    return;

    // some of these commands are not strictly necessary as the reset
    // process defaults to some of these but they are shown here
    // to demonstrate what the initialization sequence looks like

    // some configuration values are recommended by the board manufacturer

    oled_send_cmd(SSD1306_SET_DISPLAY_OFF);                     // set display off

    /* memory mapping */
    oled_send_cmd(SSD1306_SET_MEMORY_ADDRESSING_MODE);          // set memory address mode
    oled_send_cmd(SSD1306_MEMORY_ADDRESSING_MODE_HORIZONTAL);   // horizontal addressing mode

    /* resolution and layout */
    oled_send_cmd(SSD1306_SET_DISPLAY_START_LINE);              // set display start line to 0

    oled_send_cmd(SSD1306_SET_SEGMENT_REMAP_127 );              // set segment re-map column address 127 is mapped to SEG0

    oled_send_cmd(SSD1306_SET_MULTIPLEX_RATIO);                 // set multiplex ratio
    oled_send_cmd(OLED_HEIGHT-1);                               // display height - 1

    oled_send_cmd(SSD1306_SET_COM_OUTPUT_SCAN_DIR_LEFT);        // set COM (common) output scan direction

    oled_send_cmd(SSD1306_SET_DISPLAY_OFFSET);                  // set display offset
    oled_send_cmd(0x00);                                        // no offset

    oled_send_cmd(SSD1306_SET_COM_PIN_HARDWARE_CONFIG);         // set COM (common) pins hardware configuration

    if (OLED_HEIGHT<=_u(32))
    {
        oled_send_cmd(0x02);        // TODO what is this?
    }
    else
    {
        oled_send_cmd(0x12); 
    }

    /* timing and driving scheme */
    oled_send_cmd(SSD1306_SET_DISPLAY_CLOCK_DIVIDE);            // set display clock divide ratio
    oled_send_cmd(0x80);                                        // div ratio of 1, standard freq

    oled_send_cmd(SSD1306_SET_PRECHARGE_PERIOD );               // set pre-charge period
    oled_send_cmd(0xF1);                                        // Vcc internally generated on our board

    oled_send_cmd(SSD1306_SET_VCOM_DESELECT_LEVEL);             // set VCOMH deselect level
    oled_send_cmd(SSD1306_VCOM_DESELECT_083_TIMES);             // 0.83xVcc

    /* display */
    oled_send_cmd(SSD1306_SET_CONTRAST_CONTROL);                // set contrast control
    oled_send_cmd(0xFF);

    oled_send_cmd(SSD1306_SET_ENTIRE_DISPLAY_RAM);              // set entire display on to follow RAM content

    oled_send_cmd(SSD1306_SET_NORMAL_DISPLAY );                 // set normal (not inverse, B=W & W=B) display

    oled_send_cmd(SSD1306_SET_CHARGE_PUMP);                     // set charge pump
    oled_send_cmd(0x14);                                        // Vcc internally generated on our board

    /* scrolling safety */
    oled_send_cmd(SSD1306_SET_CONTINUOUS_SCROLL_DEACTIVATE );   // deactivate horizontal scrolling if set
                                                                // this is necessary as memory writes will corrupt if scrolling was enabled

    oled_send_cmd(SSD1306_SET_DISPLAY_ON );                     // turn display on ( surely this should be client option)
}

// --------------------------------------------------

void render(uint8_t *buf, struct render_area *area) {

    return;

    // update a portion of the display with a render area
    
    oled_send_cmd(SSD1306_SET_COLUMN_ADDRESS);
    oled_send_cmd(area->start_col);
    oled_send_cmd(area->end_col);

    oled_send_cmd(SSD1306_SET_PAGE_ADDRESS);
    oled_send_cmd(area->start_page);
    oled_send_cmd(area->end_page);

    oled_send_buf(buf, area->buflen);
}

// --------------------------------------------------
