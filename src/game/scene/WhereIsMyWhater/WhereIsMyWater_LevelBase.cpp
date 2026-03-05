#include "WhereIsMyWater_LevelBase.h"

#include "CollisionSystem2D.h"
#include "SceneManager.h"
#include "save_data/SaveData.h"

#include "raygui.h"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <string>

namespace
{
    struct CircleColliderProxy
    {
        Transform2D transform{};
        CircleColliderData data{};
        Collider2D collider{};

        CircleColliderProxy(float centerX, float centerY, float radius)
        {
            transform.location.value = { centerX, centerY };
            data.radius = std::max(0.0f, radius);
            collider.type = ColliderType2D::Circle;
            collider.transform = &transform;
            collider.data = &data;
        }
    };

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

std::string WhereIsMyWaterLevelBase::ResolveAssetPath(const std::string& repoRelativePath)
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

std::string WhereIsMyWaterLevelBase::GetLevelJsonPath(int levelNumber) const
{
    const int clampedLevel = std::clamp(levelNumber, 1, kTotalLevels);
    return "src/game/assets/levels/WhereIsMyWater/where_is_my_water_level" + std::to_string(clampedLevel) + ".json";
}

void WhereIsMyWaterLevelBase::RefreshProgressFromSave()
{
    unlockedLevelCount = std::clamp(SaveData::Instance().GetWaterUnlockedLevelCount(), 1, kTotalLevels);
}

void WhereIsMyWaterLevelBase::PersistLevelCompletion()
{
    if (!won || completionPersisted)
        return;

    completionPersisted = true;

    const int levelNumber = GetLevelNumber();
    SaveData::Instance().SetWaterBestScore(levelNumber, collectedDrops);
    SaveData::Instance().SetWaterCurrentLevel(levelNumber);

    const int unlockedAfterWin = std::min(kTotalLevels, levelNumber + 1);
    SaveData::Instance().SetWaterUnlockedLevelCount(unlockedAfterWin);
    RefreshProgressFromSave();

    bestScore = SaveData::Instance().GetWaterBestScore(levelNumber);
}

bool WhereIsMyWaterLevelBase::LoadLevelFromJson(const std::string& repoRelativePath)
{
    const std::string resolvedPath = ResolveAssetPath(repoRelativePath);
    std::ifstream input(resolvedPath);
    if (!input.is_open())
        return false;

    nlohmann::json data;
    try
    {
        input >> data;
    }
    catch (...)
    {
        TraceLog(LOG_WARNING, "WhereIsMyWater: invalid level JSON (%s)", resolvedPath.c_str());
        return false;
    }

    const int dirtStartRow = std::clamp(data.value("dirt_start_row", 2), 0, rows);
    for (int r = dirtStartRow; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
            SetTile(c, r, Tile::Dirt);
    }

    targetDrops = std::max(1, data.value("target_drops", targetDrops));
    maxSpawnedDrops = std::max(targetDrops, data.value("max_spawned_drops", maxSpawnedDrops));

    if (data.contains("source") && data["source"].is_object())
    {
        const auto& source = data["source"];
        sourceCol = std::clamp(source.value("col", sourceCol), 0, cols - 1);
        sourceRow = std::clamp(source.value("row", sourceRow), 0, rows - 1);
    }
    else
    {
        sourceCol = std::clamp(sourceCol, 0, cols - 1);
        sourceRow = std::clamp(sourceRow, 0, rows - 1);
    }

    int tubWidthCells = 5;
    int tubHeightCells = 2;
    int tubCol = std::max(2, cols - tubWidthCells - 2);
    int tubRow = std::max(2, rows - tubHeightCells - 1);

    if (data.contains("tub") && data["tub"].is_object())
    {
        const auto& tub = data["tub"];
        tubWidthCells = std::clamp(tub.value("width_cells", tubWidthCells), 2, cols);
        tubHeightCells = std::clamp(tub.value("height_cells", tubHeightCells), 1, rows);
        tubCol = std::clamp(tub.value("col", tubCol), 0, std::max(0, cols - tubWidthCells));
        tubRow = std::clamp(tub.value("row", tubRow), 0, std::max(0, rows - tubHeightCells));
    }

    auto ApplyRect = [this](int col, int row, int width, int height, Tile tile)
    {
        if (width <= 0 || height <= 0)
            return;

        const int beginCol = std::max(0, col);
        const int endCol = std::min(cols - 1, col + width - 1);
        const int beginRow = std::max(0, row);
        const int endRow = std::min(rows - 1, row + height - 1);

        for (int r = beginRow; r <= endRow; ++r)
        {
            for (int c = beginCol; c <= endCol; ++c)
                SetTile(c, r, tile);
        }
    };

    if (data.contains("operations") && data["operations"].is_array())
    {
        for (const auto& operation : data["operations"])
        {
            if (!operation.is_object())
                continue;

            const std::string type = operation.value("type", std::string());
            if (type == "clear_rect")
            {
                ApplyRect(
                    operation.value("col", 0),
                    operation.value("row", 0),
                    operation.value("width", 0),
                    operation.value("height", 0),
                    Tile::Empty
                );
            }
            else if (type == "dirt_rect")
            {
                ApplyRect(
                    operation.value("col", 0),
                    operation.value("row", 0),
                    operation.value("width", 0),
                    operation.value("height", 0),
                    Tile::Dirt
                );
            }
            else if (type == "rock_rect")
            {
                ApplyRect(
                    operation.value("col", 0),
                    operation.value("row", 0),
                    operation.value("width", 0),
                    operation.value("height", 0),
                    Tile::Rock
                );
            }
            else if (type == "rock_line_h" || type == "rock_line_v")
            {
                const int startCol = operation.value("col", 0);
                const int startRow = operation.value("row", 0);
                const int length = std::max(0, operation.value("length", 0));

                std::vector<int> holes;
                if (operation.contains("holes") && operation["holes"].is_array())
                {
                    for (const auto& hole : operation["holes"])
                    {
                        if (!hole.is_number_integer())
                            continue;
                        holes.push_back(std::max(0, hole.get<int>()));
                    }
                }

                for (int i = 0; i < length; ++i)
                {
                    if (std::find(holes.begin(), holes.end(), i) != holes.end())
                        continue;

                    const int c = (type == "rock_line_h") ? (startCol + i) : startCol;
                    const int r = (type == "rock_line_v") ? (startRow + i) : startRow;
                    SetTile(c, r, Tile::Rock);
                }
            }
        }
    }

    const int clearPaddingCols = std::max(0, data.value("tub_clear_padding_cols", 0));
    const int clearPaddingRows = std::max(0, data.value("tub_clear_padding_rows", 0));
    if (clearPaddingCols > 0 || clearPaddingRows > 0)
    {
        ApplyRect(
            tubCol - clearPaddingCols,
            tubRow - clearPaddingRows,
            tubWidthCells + clearPaddingCols * 2,
            tubHeightCells + clearPaddingRows * 2,
            Tile::Empty
        );
    }

    SetTile(sourceCol, sourceRow, Tile::Empty);

    tubRect = {
        playArea.x + (float)tubCol * (float)cellSize,
        playArea.y + (float)tubRow * (float)cellSize,
        (float)tubWidthCells * (float)cellSize,
        (float)tubHeightCells * (float)cellSize
    };

    return true;
}

WhereIsMyWaterLevelBase::WhereIsMyWaterLevelBase(SceneManager* manager)
    : sceneManager(manager)
{
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/DKKitsuneTail.ttf");
    uiFont = resources.GetOrLoadFont(fontPath);
}

void WhereIsMyWaterLevelBase::EnsureLevelInitialized()
{
    if (levelInitialized)
        return;

    levelInitialized = true;
    ResetLevel();
}

int WhereIsMyWaterLevelBase::Index(int c, int r) const
{
    return (r * cols) + c;
}

bool WhereIsMyWaterLevelBase::IsInsideGrid(int c, int r) const
{
    return c >= 0 && c < cols && r >= 0 && r < rows;
}

WhereIsMyWaterLevelBase::Tile WhereIsMyWaterLevelBase::GetTile(int c, int r) const
{
    if (!IsInsideGrid(c, r))
        return Tile::Rock;

    return tiles[(size_t)Index(c, r)];
}

void WhereIsMyWaterLevelBase::SetTile(int c, int r, Tile tile)
{
    if (!IsInsideGrid(c, r))
        return;

    tiles[(size_t)Index(c, r)] = tile;
}

Vector2 WhereIsMyWaterLevelBase::CellCenter(int c, int r) const
{
    return {
        playArea.x + ((float)c + 0.5f) * (float)cellSize,
        playArea.y + ((float)r + 0.5f) * (float)cellSize
    };
}

void WhereIsMyWaterLevelBase::BuildLevel()
{
    RefreshProgressFromSave();

    SaveData::Instance().SetWaterCurrentLevel(GetLevelNumber());
    bestScore = SaveData::Instance().GetWaterBestScore(GetLevelNumber());

    cols = std::max(8, (int)(playArea.width / (float)cellSize));
    rows = std::max(8, (int)(playArea.height / (float)cellSize));
    tiles.assign((size_t)(cols * rows), Tile::Empty);

    if (LoadLevelFromJson(GetLevelJsonPath(GetLevelNumber())))
        return;

    if (GetLevelNumber() != 1 && LoadLevelFromJson(GetLevelJsonPath(1)))
        return;

    for (int r = 2; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
            SetTile(c, r, Tile::Dirt);
    }

    sourceCol = 2;
    sourceRow = 1;

    for (int r = 0; r <= 3; ++r)
    {
        for (int c = 0; c <= 6; ++c)
            SetTile(c, r, Tile::Empty);
    }

    const int rowA = std::clamp(rows / 3, 4, rows - 6);
    const int rowB = std::clamp((rows * 2) / 3, 7, rows - 3);

    for (int c = 5; c < cols - 4; ++c)
    {
        if (c >= (cols / 2) - 1 && c <= (cols / 2) + 1)
            continue;

        SetTile(c, rowA, Tile::Rock);
    }

    for (int c = 2; c < cols - 7; ++c)
    {
        if (c >= cols / 3 && c <= (cols / 3) + 2)
            continue;

        SetTile(c, rowB, Tile::Rock);
    }

    const int pillarCol = std::clamp((cols * 3) / 4, 6, cols - 3);
    for (int r = rowA + 2; r < rows - 2; ++r)
    {
        if (r >= rowB - 1 && r <= rowB + 1)
            continue;

        SetTile(pillarCol, r, Tile::Rock);
    }

    const int tubWidthCells = 5;
    const int tubHeightCells = 2;
    const int tubCol = std::max(2, cols - tubWidthCells - 2);
    const int tubRow = std::max(2, rows - tubHeightCells - 1);

    for (int r = std::max(0, tubRow - 4); r < rows; ++r)
    {
        for (int c = std::max(0, tubCol - 3); c < cols; ++c)
            SetTile(c, r, Tile::Empty);
    }

    SetTile(sourceCol, sourceRow, Tile::Empty);

    tubRect = {
        playArea.x + (float)tubCol * (float)cellSize,
        playArea.y + (float)tubRow * (float)cellSize,
        (float)tubWidthCells * (float)cellSize,
        (float)tubHeightCells * (float)cellSize
    };
}

void WhereIsMyWaterLevelBase::ResetLevel()
{
    started = false;
    won = false;
    failed = false;
    completionPersisted = false;

    targetDrops = 130;
    maxSpawnedDrops = 420;

    spawnAccumulator = 0.0f;
    spawnedDrops = 0;
    collectedDrops = 0;

    drops.clear();
    BuildLevel();
}

void WhereIsMyWaterLevelBase::DigAt(const Vector2& position, int radiusInCells)
{
    const int centerCol = (int)((position.x - playArea.x) / (float)cellSize);
    const int centerRow = (int)((position.y - playArea.y) / (float)cellSize);

    for (int r = centerRow - radiusInCells; r <= centerRow + radiusInCells; ++r)
    {
        for (int c = centerCol - radiusInCells; c <= centerCol + radiusInCells; ++c)
        {
            if (!IsInsideGrid(c, r))
                continue;

            const int dx = c - centerCol;
            const int dy = r - centerRow;
            if ((dx * dx) + (dy * dy) > (radiusInCells * radiusInCells))
                continue;

            if (GetTile(c, r) != Tile::Dirt)
                continue;

            SetTile(c, r, Tile::Empty);
        }
    }
}

void WhereIsMyWaterLevelBase::HandleDigInput()
{
    if (won || failed)
        return;

    const Vector2 mouse = GetMousePosition();
    if (!IsPointInPlayArea(mouse))
        return;

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        DigAt(mouse, digRadiusCells);
}

bool WhereIsMyWaterLevelBase::IsPointInPlayArea(const Vector2& point) const
{
    return point.x >= playArea.x
        && point.x <= playArea.x + playArea.width
        && point.y >= playArea.y
        && point.y <= playArea.y + playArea.height;
}

bool WhereIsMyWaterLevelBase::IsDropInTub(const WaterDrop& drop) const
{
    CircleColliderProxy dropCollider(drop.position.x, drop.position.y, 3.0f);
    const SquareColliderProxy tubCollider = MakeTopLeftSquareCollider(tubRect.x, tubRect.y, tubRect.width, tubRect.height);
    return CollisionSystem2D::Test(dropCollider.collider, tubCollider.collider).colliding;
}

bool WhereIsMyWaterLevelBase::IsStartKeyPressed() const
{
    return IsKeyPressed(KEY_SPACE)
        || IsKeyPressed(KEY_LEFT)
        || IsKeyPressed(KEY_RIGHT)
        || IsKeyPressed(KEY_UP)
        || IsKeyPressed(KEY_A)
        || IsKeyPressed(KEY_D)
        || IsKeyPressed(KEY_W);
}

void WhereIsMyWaterLevelBase::SpawnWater(float dt)
{
    const float spawnRatePerSecond = 120.0f;
    spawnAccumulator += dt * spawnRatePerSecond;

    const Vector2 source = CellCenter(sourceCol, sourceRow);

    while (spawnAccumulator >= 1.0f && spawnedDrops < maxSpawnedDrops)
    {
        spawnAccumulator -= 1.0f;

        WaterDrop drop;
        drop.position = {
            source.x + (float)GetRandomValue(-7, 7),
            source.y + 2.0f
        };
        drop.velocity = {
            (float)GetRandomValue(-18, 18),
            (float)GetRandomValue(30, 75)
        };

        drops.push_back(drop);
        ++spawnedDrops;
    }
}

void WhereIsMyWaterLevelBase::KeepDropInBounds(WaterDrop& drop, float radius) const
{
    const float minX = playArea.x + radius + 1.0f;
    const float maxX = playArea.x + playArea.width - radius - 1.0f;
    const float minY = playArea.y + radius + 1.0f;

    if (drop.position.x < minX)
    {
        drop.position.x = minX;
        drop.velocity.x = std::abs(drop.velocity.x) * 0.35f;
    }
    else if (drop.position.x > maxX)
    {
        drop.position.x = maxX;
        drop.velocity.x = -std::abs(drop.velocity.x) * 0.35f;
    }

    if (drop.position.y < minY)
    {
        drop.position.y = minY;
        if (drop.velocity.y < 0.0f)
            drop.velocity.y = 0.0f;
    }
}

void WhereIsMyWaterLevelBase::UpdateWaterDrops(float dt)
{
    if (drops.empty())
        return;

    const float radius = 3.0f;
    const float gravity = 540.0f;

    for (WaterDrop& drop : drops)
    {
        if (!drop.alive)
            continue;

        drop.velocity.y += gravity * dt;
        drop.velocity.x += ((float)GetRandomValue(-100, 100) / 100.0f) * 20.0f * dt;

        drop.velocity.x = std::clamp(drop.velocity.x, -145.0f, 145.0f);
        drop.velocity.y = std::clamp(drop.velocity.y, -120.0f, 280.0f);

        constexpr int subSteps = 3;
        const float stepDt = dt / (float)subSteps;

        for (int step = 0; step < subSteps && drop.alive; ++step)
        {
            drop.position.x += drop.velocity.x * stepDt;
            drop.position.y += drop.velocity.y * stepDt;
            KeepDropInBounds(drop, radius);

            for (int solverPass = 0; solverPass < 2; ++solverPass)
            {
                CircleColliderProxy dropCollider(drop.position.x, drop.position.y, radius);

                const int minCol = std::clamp(
                    (int)std::floor((drop.position.x - radius - playArea.x) / (float)cellSize) - 1,
                    0,
                    cols - 1
                );
                const int maxCol = std::clamp(
                    (int)std::floor((drop.position.x + radius - playArea.x) / (float)cellSize) + 1,
                    0,
                    cols - 1
                );
                const int minRow = std::clamp(
                    (int)std::floor((drop.position.y - radius - playArea.y) / (float)cellSize) - 1,
                    0,
                    rows - 1
                );
                const int maxRow = std::clamp(
                    (int)std::floor((drop.position.y + radius - playArea.y) / (float)cellSize) + 1,
                    0,
                    rows - 1
                );

                bool collided = false;
                for (int r = minRow; r <= maxRow; ++r)
                {
                    for (int c = minCol; c <= maxCol; ++c)
                    {
                        const Tile tile = GetTile(c, r);
                        if (tile == Tile::Empty)
                            continue;

                        const float tileX = playArea.x + (float)c * (float)cellSize;
                        const float tileY = playArea.y + (float)r * (float)cellSize;
                        const SquareColliderProxy tileCollider =
                            MakeTopLeftSquareCollider(tileX, tileY, (float)cellSize, (float)cellSize);

                        const CollisionManifold2D manifold =
                            CollisionSystem2D::Test(dropCollider.collider, tileCollider.collider);
                        if (!manifold.colliding)
                            continue;

                        collided = true;

                        const float correction = manifold.penetration + 0.02f;
                        drop.position.x -= manifold.normal.x * correction;
                        drop.position.y -= manifold.normal.y * correction;

                        const float normalVelocity =
                            (drop.velocity.x * manifold.normal.x) + (drop.velocity.y * manifold.normal.y);
                        if (normalVelocity > 0.0f)
                        {
                            const float bounce = (manifold.normal.y > 0.70f) ? 0.05f : 0.18f;
                            drop.velocity.x -= manifold.normal.x * normalVelocity * (1.0f + bounce);
                            drop.velocity.y -= manifold.normal.y * normalVelocity * (1.0f + bounce);
                        }

                        if (manifold.normal.y > 0.70f && std::fabs(drop.velocity.y) < 10.0f)
                            drop.velocity.x += (float)GetRandomValue(-30, 30) * 0.5f;

                        dropCollider.transform.location.value = { drop.position.x, drop.position.y };
                    }
                }

                if (!collided)
                    break;
            }

            drop.velocity.x *= 0.995f;

            if (IsDropInTub(drop))
            {
                drop.alive = false;
                ++collectedDrops;
            }
        }

        if (!drop.alive)
            continue;

        if (drop.position.y > playArea.y + playArea.height + 28.0f
            || drop.position.x < playArea.x - 28.0f
            || drop.position.x > playArea.x + playArea.width + 28.0f)
        {
            drop.alive = false;
        }
    }

    drops.erase(
        std::remove_if(
            drops.begin(),
            drops.end(),
            [](const WaterDrop& drop)
            {
                return !drop.alive;
            }
        ),
        drops.end()
    );

    if (!won && collectedDrops >= targetDrops)
        won = true;

    if (!won && spawnedDrops >= maxSpawnedDrops && (int)drops.size() <= 5)
        failed = true;
}

void WhereIsMyWaterLevelBase::Update(float dt)
{
    EnsureLevelInitialized();

    if (dt <= 0.0f)
        return;

    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (sceneManager)
            sceneManager->LoadScene(kWaterMenuSceneIndex);
        return;
    }

    if (IsKeyPressed(KEY_R))
    {
        ResetLevel();
        return;
    }

    HandleDigInput();

    if (!started && IsStartKeyPressed())
        started = true;

    if (!started)
        return;

    if (won)
    {
        PersistLevelCompletion();

        if ((IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_N))
            && GetLevelNumber() < unlockedLevelCount
            && GetLevelNumber() < kTotalLevels
            && sceneManager
            && GetNextLevelSceneIndex() >= 0)
        {
            SaveData::Instance().SetWaterCurrentLevel(GetLevelNumber() + 1);
            sceneManager->LoadScene(GetNextLevelSceneIndex());
            return;
        }

        if (IsKeyPressed(KEY_L) && sceneManager)
            sceneManager->LoadScene(kWaterMenuSceneIndex);
        return;
    }

    if (failed)
    {
        if (IsKeyPressed(KEY_L) && sceneManager)
            sceneManager->LoadScene(kWaterMenuSceneIndex);
        return;
    }

    SpawnWater(dt);
    UpdateWaterDrops(dt);

    if (won)
        PersistLevelCompletion();
}

void WhereIsMyWaterLevelBase::DrawGridAndTerrain() const
{
    DrawRectangleRec(playArea, Color{ 31, 42, 65, 255 });

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            const float x = playArea.x + (float)c * (float)cellSize;
            const float y = playArea.y + (float)r * (float)cellSize;
            const Rectangle cellRect = { x, y, (float)cellSize, (float)cellSize };

            const Tile tile = GetTile(c, r);
            if (tile == Tile::Dirt)
            {
                DrawRectangleRec(cellRect, Color{ 123, 85, 56, 255 });
                DrawRectangle((int)x + 2, (int)y + 2, cellSize - 5, cellSize - 5, Color{ 140, 101, 67, 255 });
            }
            else if (tile == Tile::Rock)
            {
                DrawRectangleRec(cellRect, Color{ 96, 101, 112, 255 });
                DrawRectangle((int)x + 3, (int)y + 3, cellSize - 6, cellSize - 6, Color{ 123, 128, 139, 255 });
            }
        }
    }

    DrawRectangleLinesEx(playArea, 2.0f, Fade(WHITE, 0.45f));
}

void WhereIsMyWaterLevelBase::DrawDigPreview() const
{
    const Vector2 mouse = GetMousePosition();
    if (!IsPointInPlayArea(mouse))
        return;

    const int centerCol = (int)((mouse.x - playArea.x) / (float)cellSize);
    const int centerRow = (int)((mouse.y - playArea.y) / (float)cellSize);
    if (!IsInsideGrid(centerCol, centerRow))
        return;

    const bool canDigNow = !won && !failed;
    const Color diggableFill = canDigNow
        ? Color{ 93, 220, 135, 95 }
        : Color{ 165, 181, 205, 70 };
    const Color blockedFill = canDigNow
        ? Color{ 240, 109, 109, 50 }
        : Color{ 128, 140, 160, 45 };
    const Color ringColor = canDigNow
        ? Color{ 146, 238, 170, 220 }
        : Color{ 190, 201, 218, 200 };

    for (int r = centerRow - digRadiusCells; r <= centerRow + digRadiusCells; ++r)
    {
        for (int c = centerCol - digRadiusCells; c <= centerCol + digRadiusCells; ++c)
        {
            if (!IsInsideGrid(c, r))
                continue;

            const int dx = c - centerCol;
            const int dy = r - centerRow;
            if ((dx * dx) + (dy * dy) > (digRadiusCells * digRadiusCells))
                continue;

            const Rectangle cellRect = {
                playArea.x + (float)c * (float)cellSize,
                playArea.y + (float)r * (float)cellSize,
                (float)cellSize,
                (float)cellSize
            };

            if (GetTile(c, r) == Tile::Dirt)
                DrawRectangleRec(cellRect, diggableFill);
            else
                DrawRectangleRec(cellRect, blockedFill);

            DrawRectangleLinesEx(cellRect, 1.0f, Fade(ringColor, 0.9f));
        }
    }

    const Vector2 center = CellCenter(centerCol, centerRow);
    const float radiusPixels = (float)digRadiusCells * (float)cellSize + ((float)cellSize * 0.38f);
    DrawCircleLines((int)center.x, (int)center.y, radiusPixels, ringColor);
}

void WhereIsMyWaterLevelBase::DrawSourceAndTub() const
{
    const Vector2 sourceCenter = CellCenter(sourceCol, sourceRow);

    DrawRectangle((int)(sourceCenter.x - 42.0f), (int)(playArea.y - 24.0f), 52, 14, Color{ 106, 114, 126, 255 });
    DrawRectangle((int)(sourceCenter.x - 7.0f), (int)(playArea.y - 10.0f), 14, 24, Color{ 154, 162, 176, 255 });
    DrawCircleV({ sourceCenter.x, sourceCenter.y + 5.0f }, 5.0f, SKYBLUE);

    const Rectangle tubOuter = {
        tubRect.x - 8.0f,
        tubRect.y - 8.0f,
        tubRect.width + 16.0f,
        tubRect.height + 12.0f
    };

    DrawRectangleRounded(tubOuter, 0.24f, 6, Color{ 214, 224, 236, 255 });
    DrawRectangleRoundedLinesEx(tubOuter, 0.24f, 6, 3.0f, Color{ 147, 176, 208, 255 });

    const float fillT = std::clamp((float)collectedDrops / (float)targetDrops, 0.0f, 1.0f);
    const Rectangle fillRect = {
        tubRect.x,
        tubRect.y + (tubRect.height * (1.0f - fillT)),
        tubRect.width,
        tubRect.height * fillT
    };
    if (fillRect.height > 0.5f)
        DrawRectangleRec(fillRect, Color{ 80, 174, 255, 230 });

    const Vector2 duckCenter = {
        tubOuter.x + tubOuter.width * 0.72f,
        tubOuter.y - 10.0f
    };
    DrawCircleV(duckCenter, 10.0f, Color{ 255, 221, 79, 255 });
    DrawCircleV({ duckCenter.x + 8.0f, duckCenter.y - 5.0f }, 5.0f, Color{ 255, 221, 79, 255 });
    DrawTriangle(
        { duckCenter.x + 12.0f, duckCenter.y - 4.0f },
        { duckCenter.x + 19.0f, duckCenter.y - 2.0f },
        { duckCenter.x + 12.0f, duckCenter.y + 0.0f },
        ORANGE
    );
    DrawCircleV({ duckCenter.x + 9.0f, duckCenter.y - 6.0f }, 1.3f, BLACK);
}

void WhereIsMyWaterLevelBase::DrawDrops() const
{
    for (const WaterDrop& drop : drops)
    {
        DrawCircleV(drop.position, 3.0f, Color{ 96, 190, 255, 230 });
        DrawCircleV({ drop.position.x - 1.0f, drop.position.y - 1.0f }, 1.2f, Color{ 182, 230, 255, 230 });
    }
}

void WhereIsMyWaterLevelBase::DrawHud()
{
    if (uiFont)
        DrawTextEx(uiFont->value, GetLevelTitle(), { 20.0f, 16.0f }, 32.0f, 1.0f, RAYWHITE);
    else
        DrawText(GetLevelTitle(), 20, 16, 32, RAYWHITE);

    DrawText(TextFormat("LEVEL: %d", GetLevelNumber()), 20, 50, 24, Color{ 203, 236, 255, 255 });
    DrawText(TextFormat("BEST: %d", bestScore), 150, 50, 24, Color{ 164, 214, 255, 255 });
    DrawText(TextFormat("COLLECTED: %d / %d", collectedDrops, targetDrops), 20, 78, 24, Color{ 158, 227, 255, 255 });
    DrawText(TextFormat("SUPPLY LEFT: %d", std::max(0, maxSpawnedDrops - spawnedDrops)), 280, 78, 24, Color{ 255, 238, 179, 255 });

    DrawText("Left Mouse: Dig dirt | R: Restart | ESC/L: Level Select", 20, GetScreenHeight() - 22, 20, Fade(RAYWHITE, 0.82f));

    if (won)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.45f));
        DrawText(TextFormat("LEVEL %d CLEARED", GetLevelNumber()), 214, 120, 52, Color{ 124, 255, 176, 255 });
        DrawText("Press R to replay level", 270, 176, 28, RAYWHITE);
        DrawText("Press L for level select", 272, 205, 28, RAYWHITE);

        if (GetLevelNumber() < unlockedLevelCount && GetLevelNumber() < kTotalLevels && GetNextLevelSceneIndex() >= 0)
        {
            DrawText("Press N or ENTER for next level", 230, 234, 28, Color{ 181, 232, 255, 255 });
            if (GuiButton({ 312.0f, 272.0f, 178.0f, 36.0f }, "Next Level") && sceneManager)
            {
                SaveData::Instance().SetWaterCurrentLevel(GetLevelNumber() + 1);
                sceneManager->LoadScene(GetNextLevelSceneIndex());
                return;
            }
        }
        else
        {
            DrawText("All unlocked levels cleared", 258, 234, 28, Color{ 181, 232, 255, 255 });
        }

        if (GuiButton({ 300.0f, 316.0f, 198.0f, 36.0f }, "Level Select") && sceneManager)
            sceneManager->LoadScene(kWaterMenuSceneIndex);
    }
    else if (failed)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.45f));
        DrawText("OUT OF WATER", 256, 136, 56, Color{ 255, 142, 136, 255 });
        DrawText("Press R to try again", 284, 190, 30, RAYWHITE);
        DrawText("Press L for level select", 272, 224, 30, RAYWHITE);

        if (GuiButton({ 300.0f, 274.0f, 198.0f, 36.0f }, "Level Select") && sceneManager)
            sceneManager->LoadScene(kWaterMenuSceneIndex);
    }
}

void WhereIsMyWaterLevelBase::Draw()
{
    EnsureLevelInitialized();

    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{ 19, 28, 47, 255 });

    DrawGridAndTerrain();
    DrawDigPreview();
    DrawSourceAndTub();
    DrawDrops();

    if (GuiButton({ (float)GetScreenWidth() - 342.0f, 20.0f, 158.0f, 32.0f }, "Level Select") && sceneManager)
    {
        sceneManager->LoadScene(kWaterMenuSceneIndex);
        return;
    }

    if (GuiButton({ (float)GetScreenWidth() - 174.0f, 20.0f, 154.0f, 32.0f }, "Exit to Main") && sceneManager)
    {
        sceneManager->LoadScene(0);
        return;
    }

    DrawHud();
}
