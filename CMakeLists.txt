cmake_minimum_required(VERSION 4.1)
project(KitsuneEngine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# -----------------------------
# Dependencies
# -----------------------------
find_package(raylib CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(box2d CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-lua CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)

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
target_link_libraries(KitsuneEngine PRIVATE raylib)
target_link_libraries(KitsuneEngine PRIVATE glfw)
target_link_libraries(KitsuneEngine PRIVATE box2d::box2d)
target_link_libraries(KitsuneEngine PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(KitsuneEngine PRIVATE unofficial::lua::lua)
target_link_libraries(KitsuneEngine PRIVATE sol2::sol2)

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
