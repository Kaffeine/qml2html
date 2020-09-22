cmake_minimum_required(VERSION 3.2)

project(qml2html)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_AUTORCC ON)

find_package(Qt5 5.6 COMPONENTS Core Qml REQUIRED)

add_executable(qml2html
    main.cpp
    qml.qrc
)

target_link_libraries(qml2html LINK_PUBLIC
    Qt5::Core
    Qt5::Qml
)
