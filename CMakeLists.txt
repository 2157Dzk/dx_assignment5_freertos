cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME dx_assignment5_freertos)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME}
        Task/WaterFull_Light.c
        Task/Motor_Spd_Sin_Control.cpp
        Task/dbus.cpp
        Task/dbus.h
        Task/Motor_Spd_Sin_Control.h
        User_Driver/Components/Algorithm/Include/arm_math.h
        User_Driver/Components/Algorithm/QuaternionEKF.c
        User_Driver/Components/Algorithm/QuaternionEKF.c
        Task/Power_Meter.cpp
        Task/Power_Meter.h
)

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
file(GLOB_RECURSE USER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/Bsp/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/User_Driver/*.*
        ${CMAKE_CURRENT_SOURCE_DIR}/Task/*.*
)
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user sources here
        ${USER_SOURCES}
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined include paths
        Bsp
        Bsp/INS_Bsp
        User_Driver
        User_Driver/Motor_Driver
        User_Driver/Components
        User_Driver/Components/Algorithm
        User_Driver/Components/Algorithm/Include
        User_Driver/Components/Algorithm/Include/Source
        User_Driver/Components/Controller
        User_Driver/Components/Devices
        Task
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
        ARM_MATH_CM4
    __FPU_PRESENT=1
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
