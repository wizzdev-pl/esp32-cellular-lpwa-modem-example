cmake_minimum_required(VERSION 3.10)


project(esp32_test_on_pc)
set(BASE_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src/")
set(TEST_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

# Test files - independent from the main project sources
set(TEST_FILES
    ${TEST_SRC_DIR}/main.cpp
    ${TEST_SRC_DIR}/hardware_mocks/esp_log.h
    ${TEST_SRC_DIR}/hardware_mocks/esp_log.cpp

    ${TEST_SRC_DIR}/test_utils.cpp
    )

# Main project sources - subject of testing
set(SRC_FILES
    ${BASE_SRC_DIR}/common/utils.c
    ${BASE_SRC_DIR}/common/utils.h
    )

add_executable(esp32_test_on_pc ${SRC_FILES} ${TEST_FILES})
target_include_directories(esp32_test_on_pc PUBLIC
    ${TEST_SRC_DIR}/hardware_mocks/

    ${BASE_SRC_DIR}/
    ${BASE_SRC_DIR}/common/
    )


set(GTEST_INSTALLED_DIR "${CMAKE_CURRENT_BINARY_DIR}/externals/googletest/bin")
set(GTest_DIR "${GTEST_INSTALLED_DIR}/lib/cmake/GTest/" )
find_package(GTest CONFIG REQUIRED NO_DEFAULT_PATH)
include_directories("${GTEST_INSTALLED_DIR}/include")

target_link_libraries(esp32_test_on_pc GTest::gtest)


