#include "Ball.h"
#include "InputComponent.h"

void Ball::Update(float dt)
{
    // updates components (InputComponent computes axes/actions)
    Object::Update(dt);

    InputComponent* input = GetComponent<InputComponent>();
    if (!input) return;

    Vector2D move = input->GetAxis2DNormalized("Move");

    const float speed = 200.0f;
    transform.location.value.x += move.x * speed * dt;
    transform.location.value.y += move.y * speed * dt;

    if (input->GetActionPressed("Interact"))
    {
        // do interact once
    }
}
