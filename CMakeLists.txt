cmake_minimum_required(VERSION 3.25)
project(KitsuneEngine LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(KITSUNE_USE_SYSTEM_DEPS "Use find_package() dependencies instead of vendored third_party libraries." OFF)

function(kitsune_require_file path description)
    if (NOT EXISTS "${path}")
        message(FATAL_ERROR
                "Missing ${description} at: ${path}\n"
                "Run:\n"
                "  git submodule update --init --recursive\n"
                "or clone with:\n"
                "  git clone --recurse-submodules <repo-url>")
    endif ()
endfunction()

set(KITSUNE_THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
kitsune_require_file("${KITSUNE_THIRD_PARTY_DIR}/raygui/src/raygui.h" "raygui submodule")

add_library(raygui INTERFACE)
target_include_directories(raygui INTERFACE
        "${KITSUNE_THIRD_PARTY_DIR}/raygui/src")
add_library(raygui::raygui ALIAS raygui)

if (KITSUNE_USE_SYSTEM_DEPS)
    find_package(raylib CONFIG REQUIRED)
    find_package(nlohmann_json CONFIG REQUIRED)
    find_package(unofficial-lua CONFIG REQUIRED)
    find_package(sol2 CONFIG REQUIRED)

    set(KITSUNE_RAYLIB_TARGET raylib)
    set(KITSUNE_JSON_TARGET nlohmann_json::nlohmann_json)
    set(KITSUNE_LUA_TARGET unofficial::lua::lua)
    set(KITSUNE_SOL_TARGET sol2::sol2)
else ()
    kitsune_require_file("${KITSUNE_THIRD_PARTY_DIR}/raylib/CMakeLists.txt" "raylib submodule")
    kitsune_require_file("${KITSUNE_THIRD_PARTY_DIR}/nlohmann_json/include/nlohmann/json.hpp" "nlohmann/json submodule")
    kitsune_require_file("${KITSUNE_THIRD_PARTY_DIR}/sol2/include/sol/sol.hpp" "sol2 submodule")
    kitsune_require_file("${KITSUNE_THIRD_PARTY_DIR}/lua/lua.h" "lua submodule")

    set(BUILD_EXAMPLES OFF CACHE BOOL "Build raylib examples" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "Build raylib games" FORCE)
    add_subdirectory("${KITSUNE_THIRD_PARTY_DIR}/raylib" "${CMAKE_BINARY_DIR}/third_party/raylib" EXCLUDE_FROM_ALL)

    file(GLOB LUA_SOURCES CONFIGURE_DEPENDS
            "${KITSUNE_THIRD_PARTY_DIR}/lua/*.c")
    list(REMOVE_ITEM LUA_SOURCES
            "${KITSUNE_THIRD_PARTY_DIR}/lua/lua.c"
            "${KITSUNE_THIRD_PARTY_DIR}/lua/luac.c"
            "${KITSUNE_THIRD_PARTY_DIR}/lua/onelua.c"
            "${KITSUNE_THIRD_PARTY_DIR}/lua/ltests.c")

    add_library(lua STATIC ${LUA_SOURCES})
    target_include_directories(lua PUBLIC
            "${KITSUNE_THIRD_PARTY_DIR}/lua")
    if (WIN32)
        target_compile_definitions(lua PRIVATE LUA_USE_WINDOWS)
    elseif (APPLE)
        target_compile_definitions(lua PRIVATE LUA_USE_MACOSX)
    elseif (UNIX)
        target_compile_definitions(lua PRIVATE LUA_USE_LINUX)
    endif ()

    target_link_libraries(lua PUBLIC ${CMAKE_DL_LIBS})
    if (UNIX AND NOT APPLE)
        target_link_libraries(lua PUBLIC m)
    endif ()
    add_library(lua::lua ALIAS lua)

    add_library(nlohmann_json INTERFACE)
    target_include_directories(nlohmann_json INTERFACE
            "${KITSUNE_THIRD_PARTY_DIR}/nlohmann_json/include")
    add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)

    add_library(sol2 INTERFACE)
    target_include_directories(sol2 INTERFACE
            "${KITSUNE_THIRD_PARTY_DIR}/sol2/include")
    add_library(sol2::sol2 ALIAS sol2)

    set(KITSUNE_RAYLIB_TARGET raylib)
    set(KITSUNE_JSON_TARGET nlohmann_json::nlohmann_json)
    set(KITSUNE_LUA_TARGET lua::lua)
    set(KITSUNE_SOL_TARGET sol2::sol2)
endif ()

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
        src/lib/scene/SceneManager.cpp
        src/lib/scene/SceneManager.h
        src/game/scene/SceneMain.cpp
        src/game/scene/SceneMain.h
        src/game/scene/AngryBalls/AngryBalls.cpp
        src/game/scene/AngryBalls/AngryBalls.h
        src/game/scene/AngryBalls/AngryBalls_LevelBase.cpp
        src/game/scene/AngryBalls/AngryBalls_LevelBase.h
        src/lib/events/EventManager.cpp
        src/lib/events/EventManager.h
        src/lib/events/Events.cpp
        src/lib/events/Events.h
        src/lib/events/Subscription.cpp
        src/lib/events/Subscription.h
        src/lib/resource_manager/ResourceManager.cpp
        src/lib/resource_manager/ResourceManager.h
        src/game/save_data/SaveData.cpp
        src/game/save_data/SaveData.h
        src/game/scene/DinoJump/DinoJump.cpp
        src/game/scene/DinoJump/DinoJump.h
        src/game/scene/AngryBalls/AngryBalls_Level1.cpp
        src/game/scene/AngryBalls/AngryBalls_Level1.h
        src/game/scene/AngryBalls/AngryBalls_Level2.cpp
        src/game/scene/AngryBalls/AngryBalls_Level2.h
        src/game/scene/AngryBalls/AngryBalls_Level3.cpp
        src/game/scene/AngryBalls/AngryBalls_Level3.h
        src/game/scene/AngryBalls/AngryBalls_Level4.cpp
        src/game/scene/AngryBalls/AngryBalls_Level4.h
        src/game/scene/SpaceInvaders/SpaceInvaders.cpp
        src/game/scene/SpaceInvaders/SpaceInvaders.h
        src/game/scene/WhereIsMyWater/WhereIsMyWater.cpp
        src/game/scene/WhereIsMyWater/WhereIsMyWater.h
        src/game/scene/WhereIsMyWater/WhereIsMyWater_LevelBase.cpp
        src/game/scene/WhereIsMyWater/WhereIsMyWater_LevelBase.h
        src/game/scene/WhereIsMyWater/WhereIsMyWater_Level1.cpp
        src/game/scene/WhereIsMyWater/WhereIsMyWater_Level1.h
        src/game/scene/WhereIsMyWater/WhereIsMyWater_Level2.cpp
        src/game/scene/WhereIsMyWater/WhereIsMyWater_Level2.h
        src/game/scene/WhereIsMyWater/WhereIsMyWater_Level3.cpp
        src/game/scene/WhereIsMyWater/WhereIsMyWater_Level3.h
        src/game/scene/WhereIsMyWater/WhereIsMyWater_Level4.cpp
        src/game/scene/WhereIsMyWater/WhereIsMyWater_Level4.h
)

# -----------------------------
# Include directories
# -----------------------------
target_include_directories(KitsuneEngine PRIVATE
        src

        /* Game */
        src/game
        src/game/scene


        /* Engine */
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
        src/lib/scene
        src/lib/events
        src/lib/resource_manager
)

# -----------------------------
# Link libraries
# -----------------------------
target_link_libraries(KitsuneEngine PRIVATE
        ${KITSUNE_RAYLIB_TARGET}
        ${KITSUNE_JSON_TARGET}
        ${KITSUNE_LUA_TARGET}
        ${KITSUNE_SOL_TARGET}
        raygui::raygui
)

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
