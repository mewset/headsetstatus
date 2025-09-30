cmake_minimum_required(VERSION 3.16)
project(HeadsetStatus VERSION 1.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
