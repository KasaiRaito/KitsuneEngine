//
// Created by Eduardo Huerta on 05/03/26.
//

#include "SpaceInvaders.h"

#include "CollisionSystem2D.h"
#include "SceneManager.h"

#include "raygui.h"

#include <algorithm>
#include <string>
#include <vector>

namespace
{
    struct SquareColliderProxy
    {
        Transform2D transform{};
        AABBColliderData data{};
        Collider2D collider{};

        SquareColliderProxy(float centerX, float centerY, float width, float height)
        {
            transform.location.value = { centerX, centerY };
            data.halfWidth = std::max(0.0f, width * 0.5f);
            data.halfHeight = std::max(0.0f, height * 0.5f);
            collider.type = ColliderType2D::Square;
            collider.transform = &transform;
            collider.data = &data;
        }
    };

    static SquareColliderProxy MakeTopLeftSquareCollider(float x, float y, float width, float height)
    {
        return {
            x + (width * 0.5f),
            y + (height * 0.5f),
            width,
            height
        };
    }
}

std::string SpaceInvaders::ResolveAssetPath(const std::string& repoRelativePath)
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

SpaceInvaders::SpaceInvaders(SceneManager* manager)
    : sceneManager(manager)
{
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/DKKitsuneTail.ttf");
    uiFont = resources.GetOrLoadFont(fontPath);

    stars.reserve(100);
    for (int i = 0; i < 100; ++i)
    {
        Star star;
        star.position = {
            (float)GetRandomValue(0, std::max(GetScreenWidth(), 1)),
            (float)GetRandomValue(0, std::max(GetScreenHeight(), 1))
        };
        star.speed = (float)GetRandomValue(30, 130);
        star.size = (float)GetRandomValue(10, 24) / 10.0f;
        stars.push_back(star);
    }

    ResetGame();
}

void SpaceInvaders::ResetGame()
{
    score = 0;
    lives = 3;
    wave = 1;
    roundStarted = false;
    gameOver = false;
    playerShotCooldown = 0.0f;
    playerHitFlashTimer = 0.0f;

    bullets.clear();

    const int screenW = std::max(GetScreenWidth(), 1);
    const int screenH = std::max(GetScreenHeight(), 1);
    playerX = ((float)screenW - kPlayerWidth) * 0.5f;
    playerY = (float)screenH - 54.0f;

    SpawnWave();
}

void SpaceInvaders::SpawnWave()
{
    enemies.clear();

    const int rows = 5;
    const int columns = 9;
    const float spacingX = 16.0f;
    const float spacingY = 16.0f;
    const float formationWidth = ((kEnemyWidth + spacingX) * (float)columns) - spacingX;
    const float startX = ((float)GetScreenWidth() - formationWidth) * 0.5f;
    const float startY = 76.0f;

    enemies.reserve((size_t)(rows * columns));
    for (int row = 0; row < rows; ++row)
    {
        for (int column = 0; column < columns; ++column)
        {
            Enemy enemy;
            enemy.row = row;
            enemy.column = column;
            enemy.alive = true;
            enemy.position = {
                startX + (float)column * (kEnemyWidth + spacingX),
                startY + (float)row * (kEnemyHeight + spacingY)
            };
            enemies.push_back(enemy);
        }
    }

    enemyDirection = 1.0f;
    enemyMoveTimer = 0.0f;
    enemyFireTimer = 0.0f;
    waveBannerTimer = 1.15f;

    enemyMoveInterval = std::max(0.09f, 0.46f - ((float)(wave - 1) * 0.045f));
    enemyFireInterval = std::max(0.24f, 0.78f - ((float)(wave - 1) * 0.05f));
}

void SpaceInvaders::StartNextWave()
{
    bullets.clear();
    ++wave;
    SpawnWave();
}

void SpaceInvaders::TriggerGameOver()
{
    gameOver = true;
}

bool SpaceInvaders::IsStartKeyPressed() const
{
    return IsKeyPressed(KEY_SPACE)
        || IsKeyPressed(KEY_LEFT)
        || IsKeyPressed(KEY_RIGHT)
        || IsKeyPressed(KEY_UP)
        || IsKeyPressed(KEY_A)
        || IsKeyPressed(KEY_D)
        || IsKeyPressed(KEY_W);
}

void SpaceInvaders::UpdateStars(float dt)
{
    if (dt <= 0.0f)
        return;

    const float screenW = (float)std::max(GetScreenWidth(), 1);
    const float screenH = (float)std::max(GetScreenHeight(), 1);
    for (Star& star : stars)
    {
        star.position.y += star.speed * dt;
        if (star.position.y > screenH + 4.0f)
        {
            star.position.y = -4.0f;
            star.position.x = (float)GetRandomValue(0, (int)screenW);
            star.speed = (float)GetRandomValue(30, 130);
            star.size = (float)GetRandomValue(10, 24) / 10.0f;
        }
    }
}

void SpaceInvaders::FirePlayerShot()
{
    Bullet bullet;
    bullet.fromEnemy = false;
    bullet.position = { playerX + (kPlayerWidth * 0.5f), playerY - 7.0f };
    bullet.speedY = -620.0f;
    bullets.push_back(bullet);
}

void SpaceInvaders::FireEnemyShot()
{
    std::vector<const Enemy*> frontlineShooters;
    frontlineShooters.reserve(9);

    for (int column = 0; column < 9; ++column)
    {
        const Enemy* shooter = nullptr;
        for (const Enemy& enemy : enemies)
        {
            if (!enemy.alive || enemy.column != column)
                continue;

            if (!shooter || enemy.position.y > shooter->position.y)
                shooter = &enemy;
        }

        if (shooter)
            frontlineShooters.push_back(shooter);
    }

    if (frontlineShooters.empty())
        return;

    const int shooterIndex = GetRandomValue(0, (int)frontlineShooters.size() - 1);
    const Enemy& shooter = *frontlineShooters[(size_t)shooterIndex];

    Bullet bullet;
    bullet.fromEnemy = true;
    bullet.position = {
        shooter.position.x + (kEnemyWidth * 0.5f),
        shooter.position.y + kEnemyHeight + 2.0f
    };
    bullet.speedY = 320.0f + ((float)wave * 28.0f);
    bullets.push_back(bullet);
}

void SpaceInvaders::UpdatePlayer(float dt)
{
    if (dt <= 0.0f)
        return;

    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        playerX -= playerSpeed * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        playerX += playerSpeed * dt;

    playerX = std::clamp(playerX, 8.0f, (float)GetScreenWidth() - kPlayerWidth - 8.0f);
    playerY = (float)GetScreenHeight() - 54.0f;

    playerShotCooldown = std::max(0.0f, playerShotCooldown - dt);
    if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        && playerShotCooldown <= 0.0f)
    {
        FirePlayerShot();
        playerShotCooldown = kPlayerShotInterval;
    }
}

void SpaceInvaders::UpdateEnemies(float dt)
{
    if (dt <= 0.0f)
        return;

    enemyMoveTimer += dt;
    if (enemyMoveTimer >= enemyMoveInterval)
    {
        enemyMoveTimer -= enemyMoveInterval;

        bool hitBoundary = false;
        for (Enemy& enemy : enemies)
        {
            if (!enemy.alive)
                continue;

            enemy.position.x += enemyDirection * kEnemyStep;
            if (enemy.position.x <= 12.0f || enemy.position.x + kEnemyWidth >= (float)GetScreenWidth() - 12.0f)
                hitBoundary = true;
        }

        if (hitBoundary)
        {
            enemyDirection *= -1.0f;
            for (Enemy& enemy : enemies)
            {
                if (!enemy.alive)
                    continue;
                enemy.position.y += kEnemyDrop;
            }
        }
    }

    enemyFireTimer += dt;
    if (enemyFireTimer >= enemyFireInterval)
    {
        enemyFireTimer = 0.0f;
        FireEnemyShot();
    }
}

void SpaceInvaders::UpdateBullets(float dt)
{
    if (dt <= 0.0f)
        return;

    const float maxY = (float)GetScreenHeight() + 22.0f;
    const float minY = -22.0f;
    for (Bullet& bullet : bullets)
    {
        bullet.position.y += bullet.speedY * dt;
    }

    bullets.erase(
        std::remove_if(
            bullets.begin(),
            bullets.end(),
            [minY, maxY](const Bullet& bullet)
            {
                return bullet.position.y < minY || bullet.position.y > maxY;
            }
        ),
        bullets.end()
    );
}

void SpaceInvaders::HandleCollisions()
{
    SquareColliderProxy playerCollider(
        playerX + 4.0f + ((kPlayerWidth - 8.0f) * 0.5f),
        playerY + 4.0f + ((kPlayerHeight - 4.0f) * 0.5f),
        kPlayerWidth - 8.0f,
        kPlayerHeight - 4.0f
    );
    bool shouldStartNextWave = false;

    for (size_t i = 0; i < bullets.size();)
    {
        bool removeBullet = false;
        Bullet& bullet = bullets[i];
        SquareColliderProxy bulletCollider = MakeTopLeftSquareCollider(
            bullet.position.x - 2.0f,
            bullet.position.y - 8.0f,
            4.0f,
            12.0f
        );

        if (bullet.fromEnemy)
        {
            const CollisionManifold2D hit = CollisionSystem2D::Test(
                bulletCollider.collider,
                playerCollider.collider
            );
            if (hit.colliding)
            {
                removeBullet = true;
                playerHitFlashTimer = 0.18f;
                --lives;
                if (lives <= 0)
                {
                    TriggerGameOver();
                    break;
                }
            }
        }
        else
        {
            for (Enemy& enemy : enemies)
            {
                if (!enemy.alive)
                    continue;

                SquareColliderProxy enemyCollider = MakeTopLeftSquareCollider(
                    enemy.position.x,
                    enemy.position.y,
                    kEnemyWidth,
                    kEnemyHeight
                );
                const CollisionManifold2D hit = CollisionSystem2D::Test(
                    bulletCollider.collider,
                    enemyCollider.collider
                );
                if (!hit.colliding)
                    continue;

                enemy.alive = false;
                removeBullet = true;
                score += 18 + ((4 - enemy.row) * 6);
                break;
            }
        }

        if (removeBullet)
            bullets.erase(bullets.begin() + (long)i);
        else
            ++i;
    }

    if (gameOver)
        return;

    SquareColliderProxy defeatLineCollider = MakeTopLeftSquareCollider(
        0.0f,
        playerY - 8.0f,
        (float)GetScreenWidth(),
        4.0f
    );

    bool hasAliveEnemies = false;
    for (const Enemy& enemy : enemies)
    {
        if (!enemy.alive)
            continue;

        hasAliveEnemies = true;
        SquareColliderProxy enemyCollider = MakeTopLeftSquareCollider(
            enemy.position.x,
            enemy.position.y,
            kEnemyWidth,
            kEnemyHeight
        );
        const CollisionManifold2D hitDefeatLine = CollisionSystem2D::Test(
            enemyCollider.collider,
            defeatLineCollider.collider
        );
        if (hitDefeatLine.colliding)
        {
            TriggerGameOver();
            return;
        }
    }

    if (!hasAliveEnemies)
        shouldStartNextWave = true;

    if (shouldStartNextWave)
        StartNextWave();
}

void SpaceInvaders::Update(float dt)
{
    if (dt <= 0.0f)
        return;

    UpdateStars(dt);

    waveBannerTimer = std::max(0.0f, waveBannerTimer - dt);
    playerHitFlashTimer = std::max(0.0f, playerHitFlashTimer - dt);

    if (gameOver)
    {
        if (IsKeyPressed(KEY_R) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
            ResetGame();
        return;
    }

    if (!roundStarted)
    {
        if (IsStartKeyPressed())
            roundStarted = true;
        return;
    }

    UpdatePlayer(dt);
    UpdateEnemies(dt);
    UpdateBullets(dt);
    HandleCollisions();
}

void SpaceInvaders::Draw()
{
    const Color backgroundColor = Color{ 9, 12, 28, 255 };
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), backgroundColor);

    for (const Star& star : stars)
    {
        DrawCircleV(star.position, star.size, Fade(WHITE, 0.72f));
    }

    DrawLineEx(
        { 0.0f, (float)GetScreenHeight() - 22.0f },
        { (float)GetScreenWidth(), (float)GetScreenHeight() - 22.0f },
        2.0f,
        Fade(SKYBLUE, 0.50f)
    );

    for (const Enemy& enemy : enemies)
    {
        if (!enemy.alive)
            continue;

        Color enemyColor = GREEN;
        if (enemy.row == 0)
            enemyColor = ORANGE;
        else if (enemy.row <= 2)
            enemyColor = YELLOW;

        DrawRectangleRounded(
            { enemy.position.x, enemy.position.y, kEnemyWidth, kEnemyHeight },
            0.20f,
            3,
            enemyColor
        );
        DrawRectangle(
            (int)(enemy.position.x + 7.0f),
            (int)(enemy.position.y + 8.0f),
            6,
            6,
            BLACK
        );
        DrawRectangle(
            (int)(enemy.position.x + kEnemyWidth - 13.0f),
            (int)(enemy.position.y + 8.0f),
            6,
            6,
            BLACK
        );
    }

    for (const Bullet& bullet : bullets)
    {
        const Color bulletColor = bullet.fromEnemy ? RED : SKYBLUE;
        DrawRectangle(
            (int)(bullet.position.x - 2.0f),
            (int)(bullet.position.y - 8.0f),
            4,
            12,
            bulletColor
        );
    }

    const Color shipColor = (playerHitFlashTimer > 0.0f) ? ORANGE : SKYBLUE;
    DrawTriangle(
        { playerX + (kPlayerWidth * 0.5f), playerY - 4.0f },
        { playerX + 8.0f, playerY + kPlayerHeight },
        { playerX + kPlayerWidth - 8.0f, playerY + kPlayerHeight },
        shipColor
    );
    DrawRectangleRounded(
        { playerX + 6.0f, playerY + 7.0f, kPlayerWidth - 12.0f, kPlayerHeight - 6.0f },
        0.15f,
        3,
        Fade(shipColor, 0.92f)
    );

    if (uiFont)
        DrawTextEx(uiFont->value, "Space Invaders", { 20.0f, 14.0f }, 34.0f, 1.0f, RAYWHITE);
    else
        DrawText("Space Invaders", 20, 14, 34, RAYWHITE);

    DrawText(TextFormat("SCORE: %05d", score), 22, 56, 24, RAYWHITE);
    DrawText(TextFormat("LIVES: %d", std::max(lives, 0)), 240, 56, 24, SKYBLUE);
    DrawText(TextFormat("WAVE: %d", wave), 370, 56, 24, YELLOW);
    DrawText("Move: A/D or Left/Right   Shoot: Space", 20, GetScreenHeight() - 20, 18, Fade(RAYWHITE, 0.75f));

    if (!roundStarted && !gameOver)
    {
        DrawText("PRESS A VALID KEY TO START", 196, 154, 34, RAYWHITE);
        DrawText("A/D, Left/Right, W/Up, or Space", 216, 190, 24, SKYBLUE);
    }

    if (waveBannerTimer > 0.0f)
    {
        const char* waveText = TextFormat("WAVE %d", wave);
        const int waveSize = 46;
        const int waveTextWidth = MeasureText(waveText, waveSize);
        DrawText(
            waveText,
            (GetScreenWidth() - waveTextWidth) / 2,
            145,
            waveSize,
            Fade(YELLOW, std::min(1.0f, waveBannerTimer))
        );
    }

    if (gameOver)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));
        DrawText("GAME OVER", 286, 134, 50, RED);
        DrawText("Press SPACE, ENTER or R to restart", 200, 192, 28, RAYWHITE);
    }

    if (GuiButton({ (float)GetScreenWidth() - 174.0f, 18.0f, 154.0f, 32.0f }, "Exit to Main") && sceneManager)
        sceneManager->LoadScene(0);
}
