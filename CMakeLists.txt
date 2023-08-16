cmake_minimum_required(VERSION 3.11)

project(fProject)

include(ExternalProject)

set(CMAKE_CXX_STANDARD 11) #optional to use c++11

# Set install directory
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH ${CMAKE_SOURCE_DIR}/dist FORCE)
set(CMAKE_BUILD_TYPE "Debug")
if(MSVC OR XCODE)
    set(CMAKE_CONFIGURATION_TYPES "Debug" CACHE STRING "Debug" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED COMPONENTS OpenGL)

include(BuildGLEW)
include(BuildGLFW)
include(BuildGLM)

include(FetchContent)
set(FETCHCONTENT_BASE_DIR ${PROJECT_SOURCE_DIR}/thirdparty CACHE PATH "Missing description." FORCE)
FetchContent_Declare(assimp
GIT_REPOSITORY https://github.com/assimp/assimp.git
GIT_TAG master)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(assimp)

# fProject
set(EXEC fProject)

set(ASSETS assets)

file(GLOB SRC src/*.cpp)

# Function to add all files from a directory and its subdirectories
function(add_source_files_recursive SOURCE_DIR)
    file(GLOB_RECURSE SOURCE_FILES "${SOURCE_DIR}/*.cpp")
    list(APPEND ALL_SOURCE_FILES ${SOURCE_FILES})
    set(ALL_SOURCE_FILES ${ALL_SOURCE_FILES} PARENT_SCOPE)
endfunction()

# Add source files from src directory and its subdirectories
add_source_files_recursive(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(${EXEC} ${ALL_SOURCE_FILES})

target_include_directories(${EXEC} PRIVATE include thirdparty/assimp-src/include)

target_link_libraries(${EXEC} OpenGL::GL glew_s glfw glm assimp)

list(APPEND BIN ${EXEC})
# end fProject

# install files to install location
install(TARGETS ${BIN} DESTINATION ${CMAKE_INSTALL_PREFIX})
install(DIRECTORY ${ASSETS} DESTINATION ${CMAKE_INSTALL_PREFIX})


