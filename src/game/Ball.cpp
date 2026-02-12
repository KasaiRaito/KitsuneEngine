#include "Ball.h"
#include "InputComponent.h"
#include "physics_system/PhysicsComponent.h"

void Ball::Update(float dt)
{
    // updates components (InputComponent computes axes/actions)
    Object::Update(dt);

    InputComponent* input = GetComponent<InputComponent>();
    if (!input) return;

    Vector2D move = input->GetAxis2DNormalized("Move");

    if (auto* physics = GetComponent<PhysicsComponent>())
    {
        const float acceleration = 1400.0f;
        physics->AddForce(move * (physics->GetMass() * acceleration));
    }
    else
    {
        const float speed = 200.0f;
        transform.location.value.x += move.x * speed * dt;
        transform.location.value.y += move.y * speed * dt;
    }

    if (input->GetActionPressed("Interact"))
    {
        // do interact once
    }

    if (input->GetActionPressed("Toggle Movement")) {
        movement = !movement;

        MovementToggleEvent e;
        e.who = nullptr;
        e.enabled = movement;
        GEvents.OnMovementToggle.Emit(e);
    }
}
