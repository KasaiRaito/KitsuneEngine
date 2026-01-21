cmake_minimum_required(VERSION 4.1)
project(KitsuneEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# -----------------------------
# Dependencies
# -----------------------------
find_package(raylib CONFIG REQUIRED)

# -----------------------------
# Sources
# -----------------------------
file(GLOB_RECURSE ENGINE_SOURCES
        src/*.cpp
)

add_executable(KitsuneEngine
        ${ENGINE_SOURCES}
        src/lib/collider/CollisionSystem2D.cpp
        src/lib/collider/CollisionSystem2D.h
        src/lib/debug/Debug.cpp
        src/lib/debug/Debug.h
        src/lib/collider/CircleCollider2D.cpp
        src/lib/collider/CircleCollider2D.h
)

# -----------------------------
# Include directories
# -----------------------------
target_include_directories(KitsuneEngine PRIVATE
        src
        src/game
        src/lib
        src/lib/vector
        src/lib/transform
        src/lib/list
        src/lib/component
        src/lib/input
        src/lib/render
        src/lib/collider
        src/lib/debug
        src/lib/object
)

# -----------------------------
# Link libraries
# -----------------------------
target_link_libraries(KitsuneEngine PRIVATE raylib)

# -----------------------------
# macOS frameworks (raylib)
# -----------------------------
if (APPLE)
    target_link_libraries(KitsuneEngine PRIVATE
            "-framework OpenGL"
            "-framework Cocoa"
            "-framework IOKit"
            "-framework CoreVideo"
    )
endif()