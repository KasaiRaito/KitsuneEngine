#pragma once
#include "../lib/object/Object.h"

class Ball : public Object
{
    public:
    void Update(float dt) override;

    void Inputs();

};
