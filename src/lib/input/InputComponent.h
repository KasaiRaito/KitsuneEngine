#pragma once
#include "Component.h"
#include "List.h"
#include "vector/Vector2D.h"
#include "raylib.h"
#include <string>

class InputComponent : public Component
{
public:
    struct Axis2D
    {
        std::string name;
        List<int> up;
        List<int> down;
        List<int> left;
        List<int> right;

        float deadzone = 0.0f;
        Vector2D value = {0, 0};      // current frame
        Vector2D normalized = {0, 0}; // normalized current frame
    };

    struct Action
    {
        std::string name;
        List<int> keys;

        bool down = false;      // is held
        bool pressed = false;   // went down this frame
        bool released = false;  // went up this frame

        bool wasDown = false;   // internal state
    };

public:
    // Setup
    Axis2D* AddAxis2D(const std::string& name);
    Action* AddAction(const std::string& name);

    Axis2D* GetAxis2D(const std::string& name);
    Action* GetAction(const std::string& name);

    // Convenience getters (safe defaults)
    Vector2D GetAxis2DValue(const std::string& name) const;
    Vector2D GetAxis2DNormalized(const std::string& name) const;

    bool GetActionDown(const std::string& name) const;
    bool GetActionPressed(const std::string& name) const;
    bool GetActionReleased(const std::string& name) const;

    // Per-frame update
    void Update(float dt) override;

private:
    List<Axis2D> axes2D;
    List<Action> actions;

    static bool AnyKeyDown(const List<int>& keys);
};
