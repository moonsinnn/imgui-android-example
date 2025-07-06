cmake_minimum_required(VERSION 3.18.1)

project(Fuby LANGUAGES C CXX)

# Set C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Base compiler flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Debug flags (uncomment when needed)
# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -DDEBUG")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0 -DDEBUG")

# Release flags (uncomment when needed)
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")

# Android specific flags
if(ANDROID)
    add_definitions(-DANDROID)
    add_definitions(-D__ANDROID__)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
    
    # For native activities
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
endif()

# Include directories
include_directories(
    imgui
    imgui/backends
    json
    # bnm/include
    libs/xDL/include
    libs/KittyMemory
    libs/KittyMemory/Deps/Keystone/includes
    libs/arm64-v8a/curl/include
    libs/arm64-v8a/dobby/include
    libs/arm64-v8a/openssl/include
)

# add_subdirectory(bnm EXCLUDE_FROM_ALL)
# get_target_property(BNM_INCLUDE_DIRECTORIES BNM BNM_INCLUDE_DIRECTORIES)

# Source files
set(SOURCES
    main.cpp
    utils/init.cpp
    hook/hooks.cpp
    hook/input.cpp
    imguisetup.cpp
    imguisetup.h
    menu.cpp
)

# ImGui sources
set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_android.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)
# KittyMemory 
set(KITTY_SOURCES
    libs/KittyMemory/KittyArm64.cpp
    libs/KittyMemory/KittyMemory.cpp
    libs/KittyMemory/KittyPtrValidator.cpp
    libs/KittyMemory/KittyScanner.cpp
    libs/KittyMemory/KittyUtils.cpp
    libs/KittyMemory/MemoryBackup.cpp
    libs/KittyMemory/MemoryPatch.cpp
)

# Add shared library for Android
add_library(${PROJECT_NAME} SHARED
    ${SOURCES}
    libs/xDL/xdl.c
    libs/xDL/xdl_iterate.c
    libs/xDL/xdl_linker.c
    libs/xDL/xdl_lzma.c
    libs/xDL/xdl_util.c
    ${IMGUI_SOURCES}
    ${KITTY_SOURCES}
)

# Link libraries
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    # ${CMAKE_SOURCE_DIR}/libs/arm64-v8a/curl/lib/libcurl.a
    ${CMAKE_SOURCE_DIR}/libs/arm64-v8a/dobby/lib/libdobby.a
    # ${CMAKE_SOURCE_DIR}/libs/arm64-v8a/openssl/lib/libssl.a
    # ${CMAKE_SOURCE_DIR}/libs/arm64-v8a/openssl/lib/libcrypto.a
    ${CMAKE_SOURCE_DIR}/libs/KittyMemory/Deps/Keystone/libs-android/${CMAKE_ANDROID_ARCH_ABI}/libkeystone.a
    atomic
    # BNM
    log
    android
    EGL
    GLESv3
    dl
    z
)

# For Android OpenGL ES implementation
find_library(OPENGLES_LIB GLESv3)
find_library(ANDROID_LIB android)
find_library(LOG_LIB log)

# Additional Android configurations
if(ANDROID)
    # Set the path to your prebuilt libraries
    set_property(TARGET ${PROJECT_NAME} PROPERTY IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/libs/arm64-v8a/curl/lib/libcurl.a)
    
    # Add JNI support if needed
    find_package(JNI REQUIRED)
    include_directories(${JNI_INCLUDE_DIRS})
endif()
