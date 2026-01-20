#ifndef KITSUNEENGINE_BALLS_H
#define KITSUNEENGINE_BALLS_H

#include "Object.h"

class Ball : public Object {
public:
    float speed = 200.0f;
    Vector2D movement = Vector2D::Zero();
    Vector2D InputVal = Vector2D::Zero();


    Ball(const Vector2D& pos, float radius);

    void Update(float dt);

    void Input();
    void Draw();
};

class Balls : public Ball {
public:
    int BallNumber = 0;


    void SpawnBalls(int number);
};

#endif