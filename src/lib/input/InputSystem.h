#pragma once
#include <vector>

class Object;

class InputSystem
{
public:
    static void Register(Object* object);
    static void Unregister(Object* object);

    static void ProcessInput();

private:
    static std::vector<Object*> s_objects;
};
