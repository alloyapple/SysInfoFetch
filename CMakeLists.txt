cmake_minimum_required(VERSION 3.16)
project(NeoFetchPro VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Quick)

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h")

set(CMAKE_AUTOMOC_MOC_OPTIONS -DSOME_DEFINES)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Quick
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        userenv.lib
        advapi32.lib
    )
endif()

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
