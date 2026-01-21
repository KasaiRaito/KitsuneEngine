#pragma once
#include "Component.h"

class InputComponent : public Component
{
public:
    virtual void HandleInput() = 0;

    void Update(float dt) override
    {
        HandleInput();
    }

    void OnAdded() override;
    void OnRemoved() override;
};
