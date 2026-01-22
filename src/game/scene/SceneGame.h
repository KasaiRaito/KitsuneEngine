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

    void SetTime(int h, int m, int s, int ms);

private:
    List<Object*> objects;

    void UpdateClock(float dt);

    int hours = 20;
    int minutes = 15;
    int seconds = 11;
    int milliseconds = 0;
};

#endif //KITSUNEENGINE_SCENEGAME_H