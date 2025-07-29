# GenerateCompileCommands.cmake

# Ensure C++ standard is set
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()

# Ensure C++ standard is required
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Check if CMAKE_CXX_COMPILER is defined
if(NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "CMAKE_CXX_COMPILER is not set. Please specify a valid C++ compiler.")
endif()

# Fallback for CMAKE_CXX_COMPILER if not set
if(NOT CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "g++") # Default to g++ if no compiler is specified
    message(WARNING "CMAKE_CXX_COMPILER is not set. Defaulting to g++.")
endif()

# List of all include directories
set(ALL_INCLUDE_DIR     
    "${CMAKE_SOURCE_DIR}/3rdparty/raylib-5.5_win64_msvc16/include"
    "${CMAKE_SOURCE_DIR}/includes"
)

# Common compile options (simplified for MSVC)
set(common_compile_options
    "/std:c++${CMAKE_CXX_STANDARD}"
    "/W3"
    "/D_MT"
    "/D_DLL"
    "/DWIN32"
    "/D_WINDOWS"
    "/DCMAKE_INTDIR=\\\"Debug\\\""
    "/D_DEBUG_FUNCTIONAL_MACHINERY"
)

# Add include directories as /I<path> (MSVC syntax)
foreach(INCLUDE_DIR ${ALL_INCLUDE_DIR})
    if(INCLUDE_DIR)
        file(TO_CMAKE_PATH "${INCLUDE_DIR}" INCLUDE_DIR_FIXED)
        list(APPEND common_compile_options "/I${INCLUDE_DIR_FIXED}")
    endif()
endforeach()

# Collect all source files including headers from src, includes, and third-party directories
file(GLOB_RECURSE ALL_SOURCE_FILES 
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_SOURCE_DIR}/src/*.hpp"
    "${CMAKE_SOURCE_DIR}/src/*.h"
    "${CMAKE_SOURCE_DIR}/includes/*.cpp"
    "${CMAKE_SOURCE_DIR}/includes/*.hpp"
    "${CMAKE_SOURCE_DIR}/includes/*.h"
    "${CMAKE_SOURCE_DIR}/3rdparty/GLM/glm/*.hpp"
    "${CMAKE_SOURCE_DIR}/3rdparty/GLM/glm/*.h"
    #"${CMAKE_SOURCE_DIR}/3rdparty/GLM/glm/*.inl"
    "${CMAKE_SOURCE_DIR}/3rdparty/GLFW/include/GLFW/*.h"
)

# Add Vulkan headers if VULKAN_SDK is available
if(VULKAN_INCLUDE_DIR)
    file(GLOB_RECURSE VULKAN_HEADERS "${VULKAN_INCLUDE_DIR}/vulkan/*.h")
    list(APPEND ALL_SOURCE_FILES ${VULKAN_HEADERS})
endif()

# Handle case where no source files are found
if(ALL_SOURCE_FILES)
    # Initialize compile_commands as JSON array start
    set(compile_commands "[")
    
    # For each source file, add a JSON entry
    foreach(SOURCE_FILE ${ALL_SOURCE_FILES})
        file(TO_CMAKE_PATH "${SOURCE_FILE}" SOURCE_FILE_FIXED)
        string(APPEND compile_commands
            "    {\n"
            "        \"directory\": \"${CMAKE_CURRENT_BINARY_DIR}\",\n"
            "        \"command\": \"\\\"${CMAKE_CXX_COMPILER}\\\" /TP \\\"${SOURCE_FILE_FIXED}\\\""
        )
        foreach(OPTION ${common_compile_options})
            string(APPEND compile_commands " ${OPTION}")
        endforeach()
        string(APPEND compile_commands
            "\",\n"
            "        \"file\": \"${SOURCE_FILE_FIXED}\"\n"
            "    },\n"
        )
    endforeach()

    # Remove trailing comma from last entry and close JSON array
    string(REGEX REPLACE ",\n$" "\n" compile_commands "${compile_commands}")
    string(APPEND compile_commands "]")

    # Write to compile_commands.json
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json" "${compile_commands}")
else()
    message(WARNING "No source files found. compile_commands.json will not be generated.")
endif()

# Add custom target to generate compile_commands.json
add_custom_target(generate_compile_commands
    DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json"
    COMMENT "Generating compile_commands.json"
    COMMAND ${CMAKE_COMMAND} -E echo "compile_commands.json generation completed."
)
