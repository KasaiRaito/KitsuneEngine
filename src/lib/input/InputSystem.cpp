#include "InputSystem.h"
#include "../object/Object.h"
#include "InputComponent.h"

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