cmake_minimum_required(VERSION 3.12)

# Pull in PICO SDK (must be before project)
include(pico_sdk_import.cmake)

# We also need PICO EXTRAS
set(PICO_EXTRAS_PATH "../pico-extras-custom")
include(pico_extras_import.cmake)

project(rgbtest C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#pico extra overrides
add_compile_definitions(PICO_SCANVIDEO_MAX_SCANLINE_BUFFER_WORDS=640)
add_compile_definitions(PICO_SCANVIDEO_ENABLE_DEN_PIN=1)

# Initialize the Pico SDK
pico_sdk_init()
if (TARGET pico_scanvideo) # not all build types support it
    if (TARGET pico_scanvideo_dpi)
        add_executable(rgbtest
                rgbtest.c
                charset.c
                custom_modes.c
                )

        target_link_libraries(rgbtest PRIVATE
                pico_multicore
                pico_stdlib
                pico_scanvideo_dpi)

        pico_add_extra_outputs(rgbtest)
        pico_enable_stdio_uart(rgbtest 1)
        pico_enable_stdio_usb(rgbtest 1)
    endif ()
endif()
