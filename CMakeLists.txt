cmake_minimum_required(VERSION 3.10)

set(PROJECT_VER "0.1.0")

set(SUPPORTED_TARGETS esp32)


set(EXTRA_COMPONENT_DIRS app/ ${ESP_ADDITIONAL_COMPONENTS})

set(IDF_CMAKE ${CMAKE_SOURCE_DIR}/app/externals/esp-idf/tools/cmake/project.cmake)
# set(IDF_CMAKE $ENV{IDF_PATH}/tools/cmake/project.cmake)  # TODO - check if should replace the command above

if (DEFINED ENV{IDF_PATH})
    set(IDF_PATH $ENV{IDF_PATH})
    set(ENV{IDF_COMPONENT_MANAGER} 1)
endif()
include(${IDF_CMAKE})

option(USE_LTE_M "Using LTE-M" ON)
option(USE_NB_IOT "Using NB_IoT" OFF)

if (${USE_LTE_M} AND ${USE_NB_IOT})
    message(FATAL_ERROR "Cannot select two connectivity methods at the same time")
endif()


if ((NOT ${USE_LTE_M}) AND (NOT ${USE_NB_IOT}))
    message(FATAL_ERROR "No connectivity method selected")
endif()

if (${USE_LTE_M})
    set(APN_NAME "\"iot.1nce.net\"")
    add_definitions(-DUSE_LTE_M)
    message(WARNING "Using LTE_M with APN: " ${APN_NAME})
endif()

if (${USE_NB_IOT})
    set(APN_NAME "\"internet.gma.iot\"")
    add_definitions(-DUSE_NB_IOT)
    message(WARNING "Using NB_IOT with APN: " ${APN_NAME})
endif()

add_definitions(-DAPN_NAME=${APN_NAME})

# option(BUILD_WITH_PRINTS_AND_LOGS "Enable printing log messages" OFF)

project(esp32-cellular-lpwa-modem-example)
