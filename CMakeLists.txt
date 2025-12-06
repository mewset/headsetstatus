cmake_minimum_required(VERSION 3.16)
project(HeadsetStatus VERSION 1.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compiler warnings for better code quality
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# Platform check - HeadsetStatus requires Linux with D-Bus
if(NOT UNIX OR APPLE)
    message(FATAL_ERROR "HeadsetStatus requires Linux with D-Bus/UPower")
endif()

# Release optimizations (demoscene-inspired)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Link Time Optimization
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

    # Optimize for size
    add_compile_options(-Os)

    # Hide symbols by default (reduces binary size)
    add_compile_options(-fvisibility=hidden)

    # Put functions/data in separate sections for garbage collection
    add_compile_options(-ffunction-sections -fdata-sections)
    add_link_options(-Wl,--gc-sections)
endif()

# Generate version header
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/version.h"
)

# Find required Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets DBus)

# Source files
set(SOURCES
    main.cpp
    src/HeadsetManager.cpp
    src/TrayIconController.cpp
    src/NotificationManager.cpp
    src/ConfigManager.cpp
    src/SettingsDialog.cpp
)

# Create executable
add_executable(HeadsetStatus ${SOURCES})

# Include directories
target_include_directories(HeadsetStatus PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link against Qt libraries
target_link_libraries(HeadsetStatus PRIVATE Qt6::Core Qt6::Widgets Qt6::DBus)

# Enable automatic MOC for Q_OBJECT macro
set_target_properties(HeadsetStatus PROPERTIES AUTOMOC ON)

# Installation targets
install(TARGETS HeadsetStatus DESTINATION bin)
install(FILES HeadsetStatus.desktop DESTINATION share/applications)
install(FILES headsetstatus.service DESTINATION lib/systemd/user)

# Strip and compress binary in release mode
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    # Strip all symbols
    add_custom_command(TARGET HeadsetStatus POST_BUILD
        COMMAND ${CMAKE_STRIP} --strip-all $<TARGET_FILE:HeadsetStatus>
        COMMENT "Stripping binary...")

    # UPX compression (if available)
    find_program(UPX_EXECUTABLE upx)
    if(UPX_EXECUTABLE)
        add_custom_command(TARGET HeadsetStatus POST_BUILD
            COMMAND ${UPX_EXECUTABLE} --best --lzma $<TARGET_FILE:HeadsetStatus>
            COMMENT "Compressing with UPX...")
    endif()
endif()

# Testing support
option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)

    # HeadsetManager test
    add_executable(test_HeadsetManager
        tests/test_HeadsetManager.cpp
        src/HeadsetManager.cpp
    )
    target_include_directories(test_HeadsetManager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(test_HeadsetManager PRIVATE Qt6::Core Qt6::DBus Qt6::Test)
    set_target_properties(test_HeadsetManager PROPERTIES AUTOMOC ON)
    add_test(NAME HeadsetManagerTests COMMAND test_HeadsetManager)

    # ConfigManager test
    add_executable(test_ConfigManager
        tests/test_ConfigManager.cpp
        src/ConfigManager.cpp
    )
    target_include_directories(test_ConfigManager PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(test_ConfigManager PRIVATE Qt6::Core Qt6::Test)
    set_target_properties(test_ConfigManager PROPERTIES AUTOMOC ON)
    add_test(NAME ConfigManagerTests COMMAND test_ConfigManager)

    message(STATUS "Unit tests enabled - run with: ctest --output-on-failure")
endif()
