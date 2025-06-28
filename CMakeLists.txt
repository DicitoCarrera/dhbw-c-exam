cmake_minimum_required(VERSION 3.10)

# ReqNonFunc04: Set project name and executable name
project(morse C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ReqNonFunc01, ReqNonFunc02: Ensure compatibility with GNU-C/C++ for UNIX and WSL
if(UNIX)
    set(CMAKE_C_COMPILER gcc)
endif()

# ReqOptFunc04, ReqOptFunc05: MinGW64 support for Windows
if(WIN32)
    set(CMAKE_C_COMPILER gcc)
endif()

# ReqNonFunc06: Include directories (system include paths)
include_directories(SYSTEM /usr/include)

# Add compiler flags for better error checking
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

# ReqNonFunc04: Create executable named 'morse'
add_executable(morse morse.c)

# Link necessary libraries
if(UNIX)
    # No additional libraries needed for basic UNIX functionality
endif()

if(WIN32)
    # Additional libraries might be needed for Windows
endif()

# Installation
install(TARGETS morse DESTINATION bin)

# Optional: Add documentation target if doxygen is available
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    # ReqOptFunc06, ReqOptFunc07: Doxygen documentation
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
endif()
