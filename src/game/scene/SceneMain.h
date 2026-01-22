//
// Created by Eduardo Huerta on 22/01/26.
//

#ifndef KITSUNEENGINE_SCENEMAIN_H
#define KITSUNEENGINE_SCENEMAIN_H

#pragma once
#include "SceneBase.h"

class SceneMain : public SceneBase
{
public:
    SceneMain();
    ~SceneMain() override;

    void Update(float dt) override;
    void Draw() override;

    List<Object*> objects;
};


#endif //KITSUNEENGINE_SCENEMAIN_H