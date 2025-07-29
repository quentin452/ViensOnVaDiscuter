cmake_minimum_required(VERSION 3.16)

project(ViensOnVaDiscuter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/$<CONFIG>)

add_executable(${PROJECT_NAME}
    src/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/3rdparty/raylib-5.5_win64_msvc16/include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/3rdparty/raylib-5.5_win64_msvc16/lib/raylib.lib
    winmm.lib    # Pour timeBeginPeriod/timeEndPeriod
    gdi32.lib    # Pour les fonctions de dessin
    user32.lib   # Pour les fonctions de fenêtre
    shell32.lib  # Pour certaines fonctions système
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

if(WIN32)
    target_sources(${PROJECT_NAME} PRIVATE "assets/exe/app.rc")
    target_sources(${PROJECT_NAME} PRIVATE "assets/exe/resource.rc")

    if(MSVC)
        set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
        )
    else()
        set_target_properties(${PROJECT_NAME} PROPERTIES
            LINK_FLAGS "-mwindows"
        )
    endif()
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG_MODE=1)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /Zi /Od)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
    endif()
    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/Debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RELEASE_MODE=1)
    if(MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /O2 /DNDEBUG)
    else()
        target_compile_options(${PROJECT_NAME} PRIVATE -O2 -DNDEBUG)
    endif()
    set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/build/Release)
endif()

add_custom_target(sync_assets
    COMMAND ${CMAKE_COMMAND}
        -DASSETS_DIR=${CMAKE_SOURCE_DIR}/assets
        -DASSETS_DEST=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
        -P ${CMAKE_SOURCE_DIR}/scripts/sync_assets.cmake
    COMMENT "Synchronizing assets directory"
)

add_dependencies(${PROJECT_NAME} sync_assets)
