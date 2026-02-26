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
    if (LoadLevelLayoutFromJson("src/game/assets/levels/angryballs_level1.json"))
        return;
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
