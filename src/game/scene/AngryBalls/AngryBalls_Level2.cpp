//
// Created by Eduardo Huerta on 26/02/26.
//

#include "AngryBalls_Level2.h"

AngryBalls_Level2::AngryBalls_Level2(SceneManager* manager)
    : AngryBallsLevelBase(manager)
{
}

void AngryBalls_Level2::BuildLevelLayout()
{
    CreateBlock({ 545.0f, 338.0f }, 16.0f, 52.0f, 1.2f);
    CreateBlock({ 595.0f, 338.0f }, 16.0f, 52.0f, 1.2f);
    CreateBlock({ 570.0f, 280.0f }, 60.0f, 14.0f, 1.1f);
    CreateBlock({ 570.0f, 236.0f }, 16.0f, 38.0f, 1.0f);
    CreatePig({ 570.0f, 252.0f });
    CreatePig({ 570.0f, 370.0f });

    CreateBlock({ 690.0f, 338.0f }, 16.0f, 52.0f, 1.2f);
    CreateBlock({ 740.0f, 338.0f }, 16.0f, 52.0f, 1.2f);
    CreateBlock({ 715.0f, 280.0f }, 60.0f, 14.0f, 1.1f);
    CreateBlock({ 715.0f, 236.0f }, 16.0f, 38.0f, 1.0f);
    CreatePig({ 715.0f, 252.0f });
}

int AngryBalls_Level2::GetBirdCount() const
{
    return 5;
}

int AngryBalls_Level2::GetLevelNumber() const
{
    return 2;
}

const char* AngryBalls_Level2::GetLevelTitle() const
{
    return "AngryBalls_Level2";
}

int AngryBalls_Level2::GetNextLevelSceneIndex() const
{
    return -1;
}
