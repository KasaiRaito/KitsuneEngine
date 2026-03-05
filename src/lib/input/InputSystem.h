#pragma once
#include "raylib.h"
#include <initializer_list>
#include <vector>

class Object;

class InputSystem
{
public:
    static void Register(Object* object);
    static void Unregister(Object* object);

    static bool IsKeyDown(int key);
    static bool IsKeyPressed(int key);
    static bool IsKeyReleased(int key);

    static bool IsAnyKeyDown(std::initializer_list<int> keys);
    static bool IsAnyKeyPressed(std::initializer_list<int> keys);
    static bool IsAnyKeyReleased(std::initializer_list<int> keys);

    static bool IsMouseButtonDown(int button);
    static bool IsMouseButtonPressed(int button);
    static bool IsMouseButtonReleased(int button);
    static Vector2 GetMousePosition();

    static void ProcessInput();

private:
    static std::vector<Object*> s_objects;
};
