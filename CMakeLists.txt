cmake_minimum_required(VERSION 3.20)
project(PaPaYa
    VERSION 0.1.0
    DESCRIPTION "A C++ game built with OpenGL via sokol"
    LANGUAGES CXX
)

# ── C++ Standard ───────────────────────────────────────────────
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ── Options ────────────────────────────────────────────────────
option(PA_PA_YA_BUILD_TESTS  "Build unit tests"  OFF)
option(PA_PA_YA_ENABLE_ASAN  "Enable AddressSanitizer"  OFF)

# ── Output directories ─────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# ── Dependencies ───────────────────────────────────────────────
include(FetchContent)

# GLM (header-only math library, system package preferred)
find_path(GLM_INCLUDE_DIR glm/glm.hpp)

# spdlog (logging)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.15.2
    GIT_SHALLOW    TRUE
)
set(SPDLOG_BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(spdlog)

# stb_image (texture loading, for later use)
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(stb)

# ── Source files ───────────────────────────────────────────────
file(GLOB_RECURSE SOURCES src/*.cpp)

# ── Target ─────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${SOURCES})

# ── Include directories ────────────────────────────────────────
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/extern/sokol
    ${stb_SOURCE_DIR}
    ${GLM_INCLUDE_DIR}
)

# ── Link libraries ─────────────────────────────────────────────
target_link_libraries(${PROJECT_NAME} PRIVATE
    spdlog::spdlog
)

# sokol_app with X11/GLX backend needs these system libs
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        X11 Xi Xcursor dl pthread GL
    )
endif()

# ── Compile warnings ───────────────────────────────────────────
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter)
endif()

# ── Sanitisers ─────────────────────────────────────────────────
if(PA_PA_YA_ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

# ── Copy assets to build directory ─────────────────────────────
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMENT "Copying assets to build output"
)

# ── Tests (optional) ──────────────────────────────────────────
if(PA_PA_YA_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
