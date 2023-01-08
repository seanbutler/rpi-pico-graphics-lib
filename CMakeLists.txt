
cmake_minimum_required(VERSION 3.13)

#
# necessary to incorporate the RPI PICO SDK
#

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)


#
# this is our library test executable
#

project(pico_graphics_lib)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

#
# rest of the project
#

add_executable(pico_graphics_test
    main.cpp
)

# pull in common  dependencies
target_link_libraries(pico_graphics_test
    pico_stdlib 
    hardware_rtc
    hardware_adc
    hardware_i2c)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_graphics_test 1)
pico_enable_stdio_uart(pico_graphics_test 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(pico_graphics_test)

# add url via pico_set_program_url
# example_auto_set_url(blink)



