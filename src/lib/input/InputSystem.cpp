#include "InputSystem.h"
#include "../object/Object.h"
#include "InputComponent.h"
#include <algorithm>

std::vector<Object*> InputSystem::s_objects;

void InputSystem::Register(Object* object)
{
    s_objects.push_back(object);
}

void InputSystem::Unregister(Object* object)
{
    s_objects.erase(
        std::remove(s_objects.begin(), s_objects.end(), object),
        s_objects.end()
    );
}

bool InputSystem::IsKeyDown(int key)
{
    return ::IsKeyDown(key);
}

bool InputSystem::IsKeyPressed(int key)
{
    return ::IsKeyPressed(key);
}

bool InputSystem::IsKeyReleased(int key)
{
    return ::IsKeyReleased(key);
}

bool InputSystem::IsAnyKeyDown(std::initializer_list<int> keys)
{
    for (int key : keys)
    {
        if (IsKeyDown(key))
            return true;
    }

    return false;
}

bool InputSystem::IsAnyKeyPressed(std::initializer_list<int> keys)
{
    for (int key : keys)
    {
        if (IsKeyPressed(key))
            return true;
    }

    return false;
}

bool InputSystem::IsAnyKeyReleased(std::initializer_list<int> keys)
{
    for (int key : keys)
    {
        if (IsKeyReleased(key))
            return true;
    }

    return false;
}

bool InputSystem::IsMouseButtonDown(int button)
{
    return ::IsMouseButtonDown(button);
}

bool InputSystem::IsMouseButtonPressed(int button)
{
    return ::IsMouseButtonPressed(button);
}

bool InputSystem::IsMouseButtonReleased(int button)
{
    return ::IsMouseButtonReleased(button);
}

Vector2 InputSystem::GetMousePosition()
{
    return ::GetMousePosition();
}

void InputSystem::ProcessInput()
{
    for (Object* obj : s_objects)
    {
        auto input = obj->GetComponent<InputComponent>();
        if (input) {
            //input->OnInput();
        }
    }
}
