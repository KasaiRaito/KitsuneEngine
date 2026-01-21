#include "CircleRenderComponent.h"
#include "../object/Object.h"

void CircleRenderComponent::Draw()
{
    DrawCircle(
        (int)owner->transform.location.value.x,
        (int)owner->transform.location.value.y,
        radius,
        color
    );
}
