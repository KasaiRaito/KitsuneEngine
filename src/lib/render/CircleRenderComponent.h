#pragma once
#include "RenderComponent2D.h"
#include "raylib.h"

class CircleRenderComponent : public RenderComponent2D
{
public:
    float radius = 20.0f;
    Color color = RED;

    void Draw() override;
};
