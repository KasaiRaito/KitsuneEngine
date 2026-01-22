#include "Ball.h"

#include <raylib.h>

void Ball::Update(float dt) {
    Object::Update(dt);

    velocity = Vector2D(20.0f, 20.0f);
}

void Ball::Inputs() {

}
