cmake_minimum_required(VERSION 3.5)

project(CarControlApp VERSION 0.1 LANGUAGES CXX)

# Specify the OpenCV installation path  # Adjust this path if necessary

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt and OpenCV
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)
find_package(OpenCV REQUIRED)  # This line finds OpenCV

# Check if OpenCV was found
if(NOT OpenCV_FOUND)
    message(FATAL_ERROR "OpenCV not found. Please set OpenCV_DIR to your OpenCV installation path.")
endif()

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.ui
    carcontroller.cpp
    videostreamwidget.cpp
)

set(PROJECT_HEADERS
    mainwindow.h
    carcontroller.h
    videostreamwidget.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CarControlApp
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
    )
else()
    add_executable(CarControlApp ${PROJECT_SOURCES})
endif()

# Link Qt and OpenCV libraries
target_link_libraries(CarControlApp PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
    ${OpenCV_LIBS}  # Ensure OpenCV libraries are linked
)

# Include OpenCV directories
target_include_directories(CarControlApp PRIVATE
    ${OpenCV_INCLUDE_DIRS}  # Include OpenCV include directories
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set_target_properties(CarControlApp PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Installation properties
include(GNUInstallDirs)
install(TARGETS CarControlApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize executable for Qt 6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CarControlApp)
endif()



