//
// Created by Eduardo Huerta on 26/02/26.
//

#include "AngryBalls_Level4.h"

AngryBalls_Level4::AngryBalls_Level4(SceneManager* manager)
    : AngryBallsLevelBase(manager)
{
}

void AngryBalls_Level4::BuildLevelLayout()
{
    if (LoadLevelLayoutFromJson("src/game/assets/levels/AngryBalls/angryballs_level4.json"))
        return;
}

int AngryBalls_Level4::GetBirdCount() const
{
    return 7;
}

int AngryBalls_Level4::GetLevelNumber() const
{
    return 4;
}

const char* AngryBalls_Level4::GetLevelTitle() const
{
    return "AngryBalls_Level4";
}

int AngryBalls_Level4::GetNextLevelSceneIndex() const
{
    return -1;
}
