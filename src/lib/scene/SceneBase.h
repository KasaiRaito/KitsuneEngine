#pragma once

#include "List.h"
#include "../object/Object.h"

class SceneBase {
public:
    SceneBase();

    virtual ~SceneBase();

    virtual void AddObject(Object* obj);
    virtual void DeleteObjects();

    virtual void Update(float dt);

    virtual void Draw();

private:
    List<Object*> objects; // Object*
};
