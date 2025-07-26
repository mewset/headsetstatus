cmake_minimum_required(VERSION 3.16)
project(HeadsetStatus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Hitta nödvändiga Qt6-paket
find_package(Qt6 REQUIRED COMPONENTS Core Widgets DBus)

# Lägg till källkod
set(SOURCES
    main.cpp
)

# Skapa körbar fil
add_executable(HeadsetStatus ${SOURCES})

# Länka mot Qt-bibliotek
target_link_libraries(HeadsetStatus PRIVATE Qt6::Core Qt6::Widgets Qt6::DBus)

# Aktivera automatisk MOC-körning för Q_OBJECT-makrot
set_target_properties(HeadsetStatus PROPERTIES AUTOMOC ON)
