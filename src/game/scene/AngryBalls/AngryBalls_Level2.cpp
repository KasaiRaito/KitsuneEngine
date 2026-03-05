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
    if (LoadLevelLayoutFromJson("src/game/assets/levels/AngryBalls/angryballs_level2.json"))
        return;
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
    return 17;
}
