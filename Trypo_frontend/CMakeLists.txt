cmake_minimum_required(VERSION 3.16)
project(Trypo_frontend VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Aici adăugăm fișierele noastre sursă
set(PROJECT_SOURCES
        main.cpp
        MainWindow.cpp
        MainWindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Trypo_frontend MANUAL_FINALIZATION ${PROJECT_SOURCES})
else()
    add_executable(Trypo_frontend ${PROJECT_SOURCES})
endif()

target_link_libraries(Trypo_frontend PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Trypo_frontend)
endif()