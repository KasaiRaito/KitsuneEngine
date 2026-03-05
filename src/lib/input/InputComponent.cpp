#include "InputComponent.h"
#include "Events.h"
#include "InputSystem.h"
#include "Object.h"
#include <cmath>

static float Len(float x, float y) { return std::sqrt(x*x + y*y); }

bool InputComponent::AnyKeyDown(const List<int>& keys)
{
    for (size_t i = 0; i < keys.Size(); i++)
    {
        if (InputSystem::IsKeyDown(keys.Get(i))) {
            return true;
        }
    }
    return false;
}

void InputComponent::OnMovementEnabledEvent(void *ctx, const MovementToggleEvent &e) {
    Object* self = (Object*)ctx;

    if (e.who == nullptr || e.who == self)
    {
        if (auto* input = self->GetComponent<InputComponent>())
            input->movementEnabled = e.enabled;
    }
}

void InputComponent::OnAdded() {
    Component::OnAdded();

    if (owner) {
        owner->Subscribe(GEvents.OnMovementToggle, &InputComponent::OnMovementEnabledEvent);
    }

}

InputComponent::Axis2D* InputComponent::AddAxis2D(const std::string& name)
{
    Axis2D a;
    a.name = name;
    axes2D.Add(a);
    return &axes2D[axes2D.Size() - 1];
}

InputComponent::Action* InputComponent::AddAction(const std::string& name)
{
    Action a;
    a.name = name;
    actions.Add(a);
    return &actions[actions.Size() - 1];
}

InputComponent::Axis2D* InputComponent::GetAxis2D(const std::string& name)
{
    for (size_t i = 0; i < axes2D.Size(); i++)
        if (axes2D[i].name == name) return &axes2D[i];
    return nullptr;
}

InputComponent::Action* InputComponent::GetAction(const std::string& name)
{
    for (size_t i = 0; i < actions.Size(); i++)
        if (actions[i].name == name) return &actions[i];
    return nullptr;
}

Vector2D InputComponent::GetAxis2DValue(const std::string& name) const
{
    for (size_t i = 0; i < axes2D.Size(); i++)
        if (axes2D[i].name == name) return axes2D[i].value;
    return {0,0};
}

Vector2D InputComponent::GetAxis2DNormalized(const std::string& name) const
{
    for (size_t i = 0; i < axes2D.Size(); i++)
        if (axes2D[i].name == name) return axes2D[i].normalized;
    return {0,0};
}

bool InputComponent::GetActionDown(const std::string& name) const
{
    for (size_t i = 0; i < actions.Size(); i++)
        if (actions[i].name == name) return actions[i].down;
    return false;
}
bool InputComponent::GetActionPressed(const std::string& name) const
{
    for (size_t i = 0; i < actions.Size(); i++)
        if (actions[i].name == name) return actions[i].pressed;
    return false;
}
bool InputComponent::GetActionReleased(const std::string& name) const
{
    for (size_t i = 0; i < actions.Size(); i++)
        if (actions[i].name == name) return actions[i].released;
    return false;
}

void InputComponent::Update(float dt)
{
    (void)dt;

    // --- Update actions (keep working even when movement locked) ---
    for (size_t i = 0; i < actions.Size(); i++)
    {
        Action& a = actions[i];

        const bool nowDown = AnyKeyDown(a.keys);

        a.pressed  = ( nowDown && !a.wasDown);
        a.released = (!nowDown &&  a.wasDown);
        a.down     = nowDown;

        a.wasDown  = nowDown;
    }

    // --- Update axes ---
    if (!movementEnabled)
    {
        // Force movement to zero
        for (size_t i = 0; i < axes2D.Size(); i++)
        {
            axes2D[i].value = {0, 0};
            axes2D[i].normalized = {0, 0};
        }
        return;
    }

    // (your existing axis computation below)
    for (size_t i = 0; i < axes2D.Size(); i++)
    {
        Axis2D& ax = axes2D[i];

        float x = 0.0f;
        float y = 0.0f;

        if (AnyKeyDown(ax.left))  x -= 1.0f;
        if (AnyKeyDown(ax.right)) x += 1.0f;
        if (AnyKeyDown(ax.up))    y -= 1.0f;
        if (AnyKeyDown(ax.down))  y += 1.0f;

        ax.value = {x, y};

        float l = Len(x, y);
        if (l <= ax.deadzone || l == 0.0f)
            ax.normalized = {0, 0};
        else
            ax.normalized = {x / l, y / l};
    }
}
