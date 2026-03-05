//
// Created by Eduardo Huerta on 05/03/26.
//

#ifndef KITSUNEENGINE_SPACEINVADERS_H
#define KITSUNEENGINE_SPACEINVADERS_H

#pragma once

#include "SceneBase.h"
#include "ResourceManager.h"

#include "raylib.h"

#include <memory>
#include <string>
#include <vector>

class SceneManager;

class SpaceInvaders : public SceneBase
{
public:
    explicit SpaceInvaders(SceneManager* manager);
    ~SpaceInvaders() override = default;

    void Update(float dt) override;
    void Draw() override;

private:
    struct Enemy
    {
        Vector2 position = { 0.0f, 0.0f };
        int row = 0;
        int column = 0;
        bool alive = true;
    };

    struct Bullet
    {
        Vector2 position = { 0.0f, 0.0f };
        float speedY = 0.0f;
        bool fromEnemy = false;
    };

    struct Star
    {
        Vector2 position = { 0.0f, 0.0f };
        float speed = 0.0f;
        float size = 1.0f;
    };

    SceneManager* sceneManager = nullptr;
    std::shared_ptr<ResourceManager::FontResource> uiFont;

    std::vector<Enemy> enemies;
    std::vector<Bullet> bullets;
    std::vector<Star> stars;

    float playerX = 0.0f;
    float playerY = 0.0f;
    float playerSpeed = 360.0f;
    float playerShotCooldown = 0.0f;
    float enemyDirection = 1.0f;
    float enemyMoveTimer = 0.0f;
    float enemyMoveInterval = 0.45f;
    float enemyFireTimer = 0.0f;
    float enemyFireInterval = 0.75f;
    float waveBannerTimer = 0.0f;
    float playerHitFlashTimer = 0.0f;

    int lives = 3;
    int score = 0;
    int wave = 1;
    bool roundStarted = false;
    bool gameOver = false;

    static constexpr float kPlayerWidth = 56.0f;
    static constexpr float kPlayerHeight = 24.0f;
    static constexpr float kEnemyWidth = 42.0f;
    static constexpr float kEnemyHeight = 26.0f;
    static constexpr float kEnemyStep = 22.0f;
    static constexpr float kEnemyDrop = 20.0f;
    static constexpr float kPlayerShotInterval = 0.18f;

    static std::string ResolveAssetPath(const std::string& repoRelativePath);

    void ResetGame();
    void SpawnWave();
    void StartNextWave();
    void TriggerGameOver();

    void UpdateStars(float dt);
    void UpdatePlayer(float dt);
    void UpdateEnemies(float dt);
    void UpdateBullets(float dt);
    void HandleCollisions();
    void FirePlayerShot();
    void FireEnemyShot();
    bool IsStartKeyPressed() const;
};

#endif // KITSUNEENGINE_SPACEINVADERS_H
