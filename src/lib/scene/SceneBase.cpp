#include "ColliderComponent2D.h"
#include "CollisionSystem2D.h"
#include "InputSystem.h"
#include "Debug.h"
#include "SceneBase.h"

SceneBase::SceneBase() {}

SceneBase::~SceneBase() {
    for (size_t i = 0; i < objects.Size(); i++) {
        delete (Object*)objects.Get(i);
    }
}

void SceneBase::AddObject(Object* obj) {
    objects.Add(obj);
}

void SceneBase::DeleteObjects() {
    for (size_t i = 0; i < objects.Size(); i++) {
        delete (Object*)objects.Get(i);
    }
    objects.Clear(); // important
}

void SceneBase::Update(float dt) {

}

void SceneBase::Draw()
{

}