cmake_minimum_required(VERSION 3.10)


set (SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)


set (ALL_SOURCES
    ${SRC_DIR}/main.cpp
    ${SRC_DIR}/common/defines.h
    ${SRC_DIR}/common/sleep.h
    ${SRC_DIR}/common/utils.c
    ${SRC_DIR}/common/utils.h
    ${SRC_DIR}/common/error_codes.c
    ${SRC_DIR}/common/error_codes.h
    ${SRC_DIR}/connectivity/lpwa_communication_controller/lpwa_communication_controller.cpp
    ${SRC_DIR}/connectivity/lpwa_communication_controller/lpwa_communication_controller.h
    ${SRC_DIR}/connectivity/simple_mqtt_client_controller/simple_mqtt_client_controller.cpp
    ${SRC_DIR}/connectivity/simple_mqtt_client_controller/simple_mqtt_client_controller.h
    ${SRC_DIR}/connectivity/aws_configuration.h

    ${SRC_DIR}/hardware/hardware_definitions.h
    )


set (ALL_INCLUDE_DIRS
    ${SRC_DIR}/
    ${SRC_DIR}/common/
    ${SRC_DIR}/connectivity
    ${SRC_DIR}/connectivity/lpwa_communication_controller
    ${SRC_DIR}/connectivity/simple_mqtt_client_controller
    ${SRC_DIR}/hardware/
    )


set(COMPONENT_REQUIRES 
    nvs_flash
    log
    freertos
    driver
    lwip
    bt
    esp_eth
    esp_modem
    mqtt
    )

set(COMPONENT_PRIV_REQUIRES)

set(COMPONENT_SRCS ${ALL_SOURCES})
set(COMPONENT_ADD_INCLUDEDIRS ${ALL_INCLUDE_DIRS})


register_component()

#if (${BUILD_WITH_PRINTS_AND_LOGS})
#    message(WARNING "!!! Building program with prints and logs!")
#    add_definitions(-DENABLE_PRINTS_AND_LOGS)
#endif()


#target_compile_options(${COMPONENT_TARGET} PRIVATE -std=gnu99)  // already set by idf.py
target_compile_options(${COMPONENT_TARGET} PRIVATE -Wall)
