
cmake_minimum_required(VERSION 3.13)


#
# lets pull in the Existing Adafruit GFX Lib
#

include(FetchContent)

# FetchContent_Declare(
#     Adafruit_BusIO
#     GIT_REPOSITORY "https://github.com/adafruit/Adafruit_BusIO"
# )
 
# FetchContent_Declare(
#     Adafruit_GFX
#     GIT_REPOSITORY "https://github.com/adafruit/Adafruit-GFX-Library"
# )

# FetchContent_MakeAvailable(
#     Adafruit_GFX 
#     Adafruit_BusIO
#     )

#
# necessary to incorporate the RPI PICO SDK
#

# initialize pico-sdk from GIT
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_FETCH_FROM_GIT on)

# pico_sdk_import.cmake is a single file copied from this SDK
# note: this must happen before project()
include(pico_sdk_import.cmake)


project(pico_oled_test_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


#
# rest of the project
#

add_executable(pico_oled_test
    pico_lib_SSD13XX.cpp
    main.cpp
)

# pull in common  dependencies
target_link_libraries(pico_oled_test
    pico_stdlib 
    hardware_rtc
    hardware_adc
    hardware_i2c)

# enable usb output, disable uart output
pico_enable_stdio_usb(pico_oled_test 1)
pico_enable_stdio_uart(pico_oled_test 0)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(pico_oled_test)

# add url via pico_set_program_url
# example_auto_set_url(blink)



