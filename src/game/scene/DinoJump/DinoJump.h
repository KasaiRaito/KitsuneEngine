//
// Created by Eduardo Huerta on 19/02/26.
//

#ifndef KITSUNEENGINE_DINOJUMP_H
#define KITSUNEENGINE_DINOJUMP_H

#pragma once

#include "SceneBase.h"
#include "ResourceManager.h"
#include <memory>
#include <vector>

class SceneManager;

class DinoJump : public SceneBase
{
public:
    explicit DinoJump(SceneManager* manager);
    ~DinoJump() override = default;

    void Update(float dt) override;
    void Draw() override;

private:
    struct Obstacle
    {
        float x = 0.0f;
        float y = 0.0f;
        float width = 0.0f;
        float height = 0.0f;
        bool flying = false;
    };

    struct Cloud
    {
        float x = 0.0f;
        float y = 0.0f;
        float size = 1.0f;
        float speed = 0.0f;
    };

    SceneManager* sceneManager = nullptr;
    std::shared_ptr<ResourceManager::FontResource> uiFont;

    std::vector<Obstacle> obstacles;
    std::vector<Cloud> clouds;

    float groundY = 370.0f;
    float dinoX = 110.0f;
    float dinoY = 0.0f;
    float dinoWidth = 44.0f;
    float dinoHeight = 48.0f;
    float dinoVelocityY = 0.0f;
    bool dinoOnGround = true;

    bool running = false;
    bool gameOver = false;
    int score = 0;
    int bestScore = 0;
    float scoreAccumulator = 0.0f;

    float worldSpeed = 300.0f;
    float worldSpeedBase = 300.0f;
    float worldSpeedMax = 640.0f;
    float speedRamp = 10.0f;
    float gravity = 1800.0f;
    float jumpImpulse = -690.0f;
    float jumpBufferDuration = 0.13f;
    float jumpBufferTimer = 0.0f;
    float coyoteDuration = 0.09f;
    float coyoteTimer = 0.0f;
    float jumpHoldDuration = 0.19f;
    float jumpHoldTimer = 0.0f;
    float jumpHoldGravityScale = 0.52f;
    float jumpCutGravityScale = 1.85f;
    float fallGravityScale = 1.25f;

    float obstacleSpawnTimer = 0.0f;
    float obstacleSpawnInterval = 1.1f;
    float groundTickOffset = 0.0f;
    float runAnimTimer = 0.0f;
    int runFrame = 0;

    void ResetRound();
    void SpawnObstacle();
    void UpdateClouds(float dt);
    void UpdateObstacles(float dt);
    void UpdateDino(float dt);
    bool IsJumpPressed() const;
    Rectangle GetDinoBounds() const;
    void TriggerGameOver();

    void DrawWorld();
    void DrawDino() const;
    void DrawUI() const;
};


#endif //KITSUNEENGINE_DINOJUMP_H
