//
// Created by Eduardo Huerta on 26/02/26.
//

#include "AngryBalls_Level1.h"

AngryBalls_Level1::AngryBalls_Level1(SceneManager* manager)
    : AngryBallsLevelBase(manager)
{
}

void AngryBalls_Level1::BuildLevelLayout()
{
    CreateBlock({ 560.0f, 330.0f }, 18.0f, 58.0f, 1.2f);
    CreateBlock({ 620.0f, 330.0f }, 18.0f, 58.0f, 1.2f);
    CreateBlock({ 590.0f, 265.0f }, 70.0f, 15.0f, 1.0f);
    CreatePig({ 590.0f, 225.0f });

    CreateBlock({ 700.0f, 340.0f }, 24.0f, 48.0f, 1.3f);
    CreateBlock({ 740.0f, 340.0f }, 24.0f, 48.0f, 1.3f);
    CreateBlock({ 720.0f, 282.0f }, 55.0f, 14.0f, 1.0f);
    CreatePig({ 720.0f, 245.0f });

    CreatePig({ 660.0f, 370.0f });
}

int AngryBalls_Level1::GetBirdCount() const
{
    return 4;
}

int AngryBalls_Level1::GetLevelNumber() const
{
    return 1;
}

const char* AngryBalls_Level1::GetLevelTitle() const
{
    return "AngryBalls_Level1";
}

int AngryBalls_Level1::GetNextLevelSceneIndex() const
{
    return 4;
}
