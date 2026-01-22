//
// Created by Eduardo Huerta on 22/01/26.
//

#ifndef KITSUNEENGINE_SCENEGAME_H
#define KITSUNEENGINE_SCENEGAME_H

#pragma once
#include "SceneBase.h"
#include "List.h"
#include "Object.h"

class SceneGame : public SceneBase
{
public:
    SceneGame();
    ~SceneGame() override;

    void Update(float dt) override;
    void Draw() override;

private:
    List<Object*> objects;
};

#endif //KITSUNEENGINE_SCENEGAME_H