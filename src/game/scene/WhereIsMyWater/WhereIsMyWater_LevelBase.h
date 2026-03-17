#pragma once

#include "SceneBase.h"
#include "ResourceManager.h"

#include "raylib.h"

#include <memory>
#include <string>
#include <vector>

class SceneManager;

class WhereIsMyWaterLevelBase : public SceneBase
{
public:
    explicit WhereIsMyWaterLevelBase(SceneManager* manager);
    ~WhereIsMyWaterLevelBase() override = default;

    void Update(float dt) override;
    void Draw() override;

protected:
    virtual int GetLevelNumber() const = 0; // 1-based
    virtual const char* GetLevelTitle() const = 0;
    virtual int GetNextLevelSceneIndex() const = 0;

    static constexpr int kWaterMenuSceneIndex = 8;
    static constexpr int kTotalLevels = 4;

private:
    enum class Tile : unsigned char
    {
        Empty,
        Dirt,
        Rock
    };

    struct WaterDrop
    {
        Vector2 position = { 0.0f, 0.0f };
        Vector2 velocity = { 0.0f, 0.0f };
        bool alive = true;
    };

    SceneManager* sceneManager = nullptr;
    std::shared_ptr<ResourceManager::FontResource> uiFont;

    Rectangle playArea = { 20.0f, 72.0f, 760.0f, 352.0f };
    Rectangle tubRect = { 0.0f, 0.0f, 0.0f, 0.0f };
    int cellSize = 20;
    int cols = 0;
    int rows = 0;
    int sourceCol = 2;
    int sourceRow = 1;

    std::vector<Tile> tiles;
    std::vector<WaterDrop> drops;

    float spawnAccumulator = 0.0f;
    int spawnedDrops = 0;
    int collectedDrops = 0;
    int targetDrops = 130;
    int maxSpawnedDrops = 420;
    int digRadiusCells = 1;

    bool started = false;
    bool won = false;
    bool failed = false;
    bool completionPersisted = false;
    bool levelInitialized = false;

    int unlockedLevelCount = 1;
    int bestScore = 0;

    static std::string ResolveAssetPath(const std::string& repoRelativePath);
    void EnsureLevelInitialized();
    bool LoadLevelFromJson(const std::string& repoRelativePath);
    std::string GetLevelJsonPath(int levelNumber) const;
    void RefreshProgressFromSave();
    void PersistLevelCompletion();

    int Index(int c, int r) const;
    bool IsInsideGrid(int c, int r) const;
    Tile GetTile(int c, int r) const;
    void SetTile(int c, int r, Tile tile);
    Vector2 CellCenter(int c, int r) const;

    void BuildLevel();
    void ResetLevel();

    void HandleDigInput();
    void DigAt(const Vector2& position, int radiusInCells);

    bool IsPointInPlayArea(const Vector2& point) const;
    bool IsDropInTub(const WaterDrop& drop) const;
    bool IsStartKeyPressed() const;

    void SpawnWater(float dt);
    void UpdateWaterDrops(float dt);
    void KeepDropInBounds(WaterDrop& drop, float radius) const;

    void DrawGridAndTerrain() const;
    void DrawDigPreview() const;
    void DrawSourceAndTub() const;
    void DrawDrops() const;
    void DrawHud();
};
