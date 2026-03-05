//
// Created by Eduardo Huerta on 19/02/26.
//

#include "DinoJump/DinoJump.h"

#include "InputSystem.h"
#include "SceneManager.h"
#include "save_data/SaveData.h"
#include "raylib.h"
#include "raygui.h"

#include <algorithm>
#include <string>

namespace
{
    static std::string ResolveAssetPath(const std::string& repoRelativePath)
    {
        const std::string candidates[] = {
            repoRelativePath,
            "../" + repoRelativePath,
            "../../" + repoRelativePath,
            "../../../" + repoRelativePath
        };

        for (const std::string& candidate : candidates)
        {
            if (FileExists(candidate.c_str()))
                return candidate;
        }

        return repoRelativePath;
    }
}

DinoJump::DinoJump(SceneManager* manager)
    : sceneManager(manager)
{
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/DKKitsuneTail.ttf");
    uiFont = resources.GetOrLoadFont(fontPath);
    bestScore = SaveData::Instance().GetDinoBestScore();

    ResetRound();

    clouds.reserve(6);
    for (int i = 0; i < 6; ++i)
    {
        Cloud cloud;
        cloud.x = (float)GetRandomValue(40, GetScreenWidth() + 260);
        cloud.y = (float)GetRandomValue(40, 190);
        cloud.size = (float)GetRandomValue(7, 16) / 10.0f;
        cloud.speed = (float)GetRandomValue(15, 55);
        clouds.push_back(cloud);
    }
}

void DinoJump::ResetRound()
{
    obstacles.clear();

    running = false;
    gameOver = false;

    score = 0;
    scoreAccumulator = 0.0f;

    worldSpeed = worldSpeedBase;
    obstacleSpawnTimer = 0.0f;
    obstacleSpawnInterval = 1.0f;
    groundTickOffset = 0.0f;

    dinoY = groundY - dinoHeight;
    dinoVelocityY = 0.0f;
    dinoOnGround = true;
    jumpBufferTimer = 0.0f;
    coyoteTimer = coyoteDuration;
    jumpHoldTimer = 0.0f;

    runAnimTimer = 0.0f;
    runFrame = 0;
}

bool DinoJump::IsJumpPressed() const
{
    return InputSystem::IsAnyKeyPressed({ KEY_SPACE, KEY_UP, KEY_W });
}

Rectangle DinoJump::GetDinoBounds() const
{
    return { dinoX + 4.0f, dinoY + 4.0f, dinoWidth - 8.0f, dinoHeight - 6.0f };
}

void DinoJump::SpawnObstacle()
{
    Obstacle obstacle;

    const bool spawnBird = (GetRandomValue(0, 100) < 18) && worldSpeed > 390.0f;

    obstacle.flying = spawnBird;
    obstacle.x = (float)GetScreenWidth() + (float)GetRandomValue(0, 120);

    if (spawnBird)
    {
        obstacle.width = 34.0f;
        obstacle.height = 22.0f;
        obstacle.y = groundY - dinoHeight - (float)GetRandomValue(18, 68);
    }
    else
    {
        const bool tall = GetRandomValue(0, 100) < 40;
        obstacle.width = tall ? 26.0f : 22.0f;
        obstacle.height = tall ? 54.0f : 42.0f;
        obstacle.y = groundY - obstacle.height;
    }

    obstacles.push_back(obstacle);
}

void DinoJump::UpdateClouds(float dt)
{
    const float screenW = (float)GetScreenWidth();

    for (Cloud& cloud : clouds)
    {
        cloud.x -= (cloud.speed + (worldSpeed * 0.18f)) * dt;
        if (cloud.x < -110.0f)
        {
            cloud.x = screenW + (float)GetRandomValue(30, 260);
            cloud.y = (float)GetRandomValue(35, 190);
            cloud.size = (float)GetRandomValue(7, 16) / 10.0f;
            cloud.speed = (float)GetRandomValue(15, 55);
        }
    }
}

void DinoJump::UpdateObstacles(float dt)
{
    for (Obstacle& obstacle : obstacles)
    {
        obstacle.x -= worldSpeed * dt;
    }

    obstacles.erase(
        std::remove_if(
            obstacles.begin(),
            obstacles.end(),
            [](const Obstacle& obstacle)
            {
                return obstacle.x + obstacle.width < -10.0f;
            }
        ),
        obstacles.end()
    );

    obstacleSpawnTimer += dt;
    if (obstacleSpawnTimer >= obstacleSpawnInterval)
    {
        obstacleSpawnTimer = 0.0f;
        SpawnObstacle();

        const float speedT = std::clamp((worldSpeed - worldSpeedBase) / (worldSpeedMax - worldSpeedBase), 0.0f, 1.0f);
        const float minGap = 0.72f - (speedT * 0.20f);
        const float maxGap = 1.35f - (speedT * 0.28f);
        obstacleSpawnInterval = (float)GetRandomValue((int)(minGap * 1000.0f), (int)(maxGap * 1000.0f)) / 1000.0f;
    }
}

void DinoJump::UpdateDino(float dt)
{
    const bool jumpPressed = IsJumpPressed();
    const bool jumpHeld = InputSystem::IsAnyKeyDown({ KEY_SPACE, KEY_UP, KEY_W });

    if (jumpPressed)
    {
        jumpBufferTimer = jumpBufferDuration;
        if (!running)
            running = true;
    }

    jumpBufferTimer = std::max(0.0f, jumpBufferTimer - dt);

    if (dinoOnGround)
        coyoteTimer = coyoteDuration;
    else
        coyoteTimer = std::max(0.0f, coyoteTimer - dt);

    auto StartJump = [this]()
    {
        dinoVelocityY = jumpImpulse;
        dinoOnGround = false;
        jumpBufferTimer = 0.0f;
        coyoteTimer = 0.0f;
        jumpHoldTimer = jumpHoldDuration;
    };

    if (jumpBufferTimer > 0.0f && coyoteTimer > 0.0f)
    {
        StartJump();
    }

    float gravityScale = 1.0f;
    if (!dinoOnGround && dinoVelocityY < 0.0f)
    {
        if (jumpHeld && jumpHoldTimer > 0.0f)
        {
            jumpHoldTimer = std::max(0.0f, jumpHoldTimer - dt);
            gravityScale = jumpHoldGravityScale;
        }
        else
        {
            gravityScale = jumpCutGravityScale;
            jumpHoldTimer = 0.0f;
        }
    }
    else if (!dinoOnGround && dinoVelocityY > 0.0f)
    {
        gravityScale = fallGravityScale;
    }

    dinoVelocityY += gravity * gravityScale * dt;
    dinoY += dinoVelocityY * dt;

    const float groundTop = groundY - dinoHeight;
    if (dinoY >= groundTop)
    {
        dinoY = groundTop;
        dinoVelocityY = 0.0f;
        dinoOnGround = true;
        coyoteTimer = coyoteDuration;
        jumpHoldTimer = 0.0f;

        // Jump buffer lets a just-before-landing press fire instantly on touchdown.
        if (jumpBufferTimer > 0.0f)
        {
            StartJump();
        }
    }

    if (running && dinoOnGround)
    {
        runAnimTimer += dt;
        if (runAnimTimer >= 0.11f)
        {
            runAnimTimer = 0.0f;
            runFrame = 1 - runFrame;
        }
    }
    else
    {
        runFrame = 0;
        runAnimTimer = 0.0f;
    }
}

void DinoJump::TriggerGameOver()
{
    gameOver = true;
    running = false;

    if (score > bestScore)
    {
        bestScore = score;
        SaveData::Instance().SetDinoBestScore(bestScore);
    }
}

void DinoJump::Update(float dt)
{
    if (dt <= 0.0f)
        return;

    if (gameOver)
    {
        if (InputSystem::IsKeyPressed(KEY_R) || IsJumpPressed())
            ResetRound();

        return;
    }

    UpdateDino(dt);
    UpdateClouds(dt);

    if (running)
    {
        worldSpeed += speedRamp * dt;
        if (worldSpeed > worldSpeedMax)
            worldSpeed = worldSpeedMax;

        groundTickOffset += worldSpeed * dt;
        if (groundTickOffset >= 42.0f)
            groundTickOffset -= 42.0f;

        scoreAccumulator += dt * 100.0f;
        score = (int)scoreAccumulator;

        UpdateObstacles(dt);

        const Rectangle dinoBounds = GetDinoBounds();
        for (const Obstacle& obstacle : obstacles)
        {
            Rectangle obstacleBounds = { obstacle.x, obstacle.y, obstacle.width, obstacle.height };
            if (CheckCollisionRecs(dinoBounds, obstacleBounds))
            {
                TriggerGameOver();
                break;
            }
        }
    }
}

void DinoJump::DrawDino() const
{
    const Color bodyColor = gameOver ? Color{ 100, 100, 100, 255 } : DARKGRAY;
    DrawRectangleRounded({ dinoX, dinoY, dinoWidth, dinoHeight }, 0.18f, 5, bodyColor);

    DrawRectangle((int)(dinoX + 26), (int)(dinoY + 8), 9, 10, Color{ 230, 230, 230, 255 });
    DrawRectangle((int)(dinoX + 30), (int)(dinoY + 12), 4, 4, BLACK);

    const int legY = (int)(dinoY + dinoHeight - 12);
    if (!dinoOnGround)
    {
        DrawRectangle((int)(dinoX + 8), legY, 10, 12, bodyColor);
        DrawRectangle((int)(dinoX + 24), legY, 10, 12, bodyColor);
        return;
    }

    const int offset = (runFrame == 0) ? 0 : 4;
    DrawRectangle((int)(dinoX + 8), legY + offset, 10, 12 - offset, bodyColor);
    DrawRectangle((int)(dinoX + 24), legY + (4 - offset), 10, 12 - (4 - offset), bodyColor);
}

void DinoJump::DrawWorld()
{
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 247, 247, 247, 255 });

    for (const Cloud& cloud : clouds)
    {
        const float r = 14.0f * cloud.size;
        DrawCircleV({ cloud.x, cloud.y }, r, Color{ 228, 228, 228, 255 });
        DrawCircleV({ cloud.x + r * 0.8f, cloud.y - 6.0f * cloud.size }, r * 0.9f, Color{ 232, 232, 232, 255 });
        DrawCircleV({ cloud.x + r * 1.5f, cloud.y }, r * 0.8f, Color{ 224, 224, 224, 255 });
    }

    DrawLineEx({ 0.0f, groundY }, { (float)GetScreenWidth(), groundY }, 2.0f, GRAY);

    for (int x = -60; x < GetScreenWidth() + 80; x += 42)
    {
        const float drawX = (float)x - groundTickOffset;
        DrawLineEx({ drawX, groundY + 6.0f }, { drawX + 18.0f, groundY + 6.0f }, 2.0f, LIGHTGRAY);
    }

    for (const Obstacle& obstacle : obstacles)
    {
        if (obstacle.flying)
        {
            DrawRectangleRounded(
                { obstacle.x, obstacle.y, obstacle.width, obstacle.height },
                0.25f,
                4,
                DARKGRAY
            );
            DrawLineEx(
                { obstacle.x + 5.0f, obstacle.y + obstacle.height * 0.5f },
                { obstacle.x + obstacle.width - 5.0f, obstacle.y + obstacle.height * 0.5f },
                2.0f,
                LIGHTGRAY
            );
        }
        else
        {
            DrawRectangleRounded(
                { obstacle.x, obstacle.y, obstacle.width, obstacle.height },
                0.20f,
                3,
                DARKGREEN
            );
            DrawRectangle((int)(obstacle.x + obstacle.width * 0.55f), (int)(obstacle.y + 8.0f), 5, 12, GREEN);
            DrawRectangle((int)(obstacle.x + obstacle.width * 0.15f), (int)(obstacle.y + 16.0f), 5, 10, GREEN);
        }
    }

    DrawDino();
}

void DinoJump::DrawUI() const
{
    if (uiFont)
        DrawTextEx(uiFont->value, "Dino Jump", { 20.0f, 16.0f }, 34.0f, 1.0f, DARKGRAY);
    else
        DrawText("Dino Jump", 20, 16, 34, DARKGRAY);

    const char* scoreText = TextFormat("SCORE: %05d", score);
    const char* bestText = TextFormat("BEST: %05d", bestScore);
    const int hudSize = 24;
    const int centerX = GetScreenWidth() / 2;
    const int scoreW = MeasureText(scoreText, hudSize);
    const int bestW = MeasureText(bestText, hudSize);

    DrawText(scoreText, centerX - (scoreW / 2), 18, hudSize, DARKGRAY);
    DrawText(bestText, centerX - (bestW / 2), 46, hudSize, GRAY);

    if (!running && !gameOver)
    {
        DrawText("PRESS SPACE TO START", 235, 120, 28, DARKGRAY);
        DrawText("SPACE / UP / W = JUMP (HOLD FOR HIGHER)", 190, 156, 20, GRAY);
    }

    if (gameOver)
    {
        DrawText("GAME OVER", 290, 112, 44, BLACK);
        DrawText("PRESS SPACE OR R TO RESTART", 218, 158, 24, DARKGRAY);
    }

    DrawText("Avoid cacti and birds. Survive as long as possible.", 20, GetScreenHeight() - 54, 20, GRAY);
}

void DinoJump::Draw()
{
    DrawWorld();

    if (GuiButton({ 620, 20, 160, 32 }, "Exit to Main") && sceneManager)
        sceneManager->LoadScene(0);

    DrawUI();
}
