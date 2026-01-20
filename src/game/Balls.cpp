#include "Balls.h"
#include "raylib.h"
#include "Vector2D.h"
#include "List.h"

Ball::Ball(const Vector2D& pos, float radius){
    transform.location.value = pos;
    collider.SetCircle(radius);
}

void Ball::Update(float dt){
    Input();

    //if (transform.location.value != Vector2D::Zero())
    transform.location.value += movement * dt;
}

void Ball::Input() {
    InputVal = Vector2D::Zero();

    if (IsKeyDown(KEY_W)) {InputVal.y -= 1.0f;}
    if (IsKeyDown(KEY_S)) {InputVal.y += 1.0f;}
    if (IsKeyDown(KEY_D)) {InputVal.x += 1.0f;}
    if (IsKeyDown(KEY_A)) {InputVal.x -= 1.0f;}

    InputVal = InputVal.Normalize();
    movement = Vector2D(InputVal.x * speed, InputVal.y * speed);
}

void Ball::Draw() {
    DrawCircle(
        (int)transform.location.value.x,
        (int)transform.location.value.y,
        collider.circle.radius,
        RED
    );
}

void Balls::SpawnBalls(int number) {
    for (int i = 0; i < number; i++)
    {

    }
}
