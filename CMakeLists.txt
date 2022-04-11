# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set($ENV{PICO_SDK_PATH})
set($ENV{FREERTOS_KERNEL_PATH})

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
include(FreeRTOS_Kernel_import.cmake)


#Nombre del proyecto y estándares del lenguaje aceptados
project(myblinkkey C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
if (TARGET tinyusb_device)
        add_executable(${PROJECT_NAME} 
                src/main.c
                src/timer.c
                src/events.c
                src/pwm.c
                src/bluetooth.c 
        )
        include_directories(include)


        # Add any user requested libraries
        target_link_libraries(${PROJECT_NAME}
                FreeRTOS-Kernel
                FreeRTOS-Kernel-Heap4
                pico_stdlib
                pico_multicore
                hardware_irq
                hardware_gpio
                hardware_uart
                hardware_pwm
        )

        pico_enable_stdio_uart(${PROJECT_NAME} 0)
        pico_enable_stdio_usb(${PROJECT_NAME} 1)
        
        pico_add_extra_outputs(${PROJECT_NAME})

elseif(PICO_ON_DEVICE)
        message(WARNING "not building hello_usb because TinyUSB submodule is not initialized in the SDK")
endif()
