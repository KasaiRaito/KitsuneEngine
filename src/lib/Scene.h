#pragma once

#include "List.h"
#include "object/Object.h"

class Scene {
public:
    Scene();
    ~Scene();

    void AddObject(Object* obj);
    void DeleteObjects();

    void Update(float dt);
    void Draw();

private:
    List<Object*> objects; // Object*

};
