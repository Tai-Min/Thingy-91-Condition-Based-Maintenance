cmake_minimum_required(VERSION 3.20.0)

#set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(vibration_sensor)

add_definitions(-DEIDSP_USE_CMSIS_DSP=1
                -DEI_CLASSIFIER_TFLITE_ENABLE_CMSIS_NN=1
                -DARM_MATH_LOOPUNROLL
                )

include(edge-impulse-sdk/cmake/utils.cmake)
add_subdirectory(edge-impulse-sdk/cmake/zephyr)

set(INCLUDES
    .
    include
    tflite-model
    model-parameters
    )
include_directories(${INCLUDES})

RECURSIVE_FIND_FILE(C_SOURCE_FILES "src" "*.c")
RECURSIVE_FIND_FILE(CPP_SOURCE_FILES "src" "*.cpp")
RECURSIVE_FIND_FILE(MODEL_FILES "tflite-model" "*.cpp")
list(APPEND SOURCE_FILES ${C_SOURCE_FILES} ${CPP_SOURCE_FILES} ${MODEL_FILES})

target_sources(app PRIVATE ${SOURCE_FILES})
target_sources(app PRIVATE src/main.cpp)