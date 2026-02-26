//
// Created by Eduardo Huerta on 22/01/26.
//

#ifndef KITSUNEENGINE_ANGRYBALLS_H
#define KITSUNEENGINE_ANGRYBALLS_H

#pragma once
#include "SceneBase.h"
#include "List.h"
#include "Object.h"
#include "ResourceManager.h"
#include <memory>
#include <unordered_map>
#include <vector>

class SceneManager;

class AngryBalls : public SceneBase
{
public:
    explicit AngryBalls(SceneManager* manager);
    ~AngryBalls() override;

    void Update(float dt) override;
    void Draw() override;

private:
    enum class EntityKind
    {
        Bird,
        Pig,
        Block,
        Boundary
    };

    struct EntityVisual
    {
        EntityKind kind = EntityKind::Block;
        Color color = GRAY;
        float radius = 0.0f;
        float halfWidth = 0.0f;
        float halfHeight = 0.0f;
    };

    SceneManager* sceneManager = nullptr;
    List<Object*> objects;
    std::unordered_map<Object*, EntityVisual> visuals;
    std::unordered_map<Object*, float> blockHealth;

    std::shared_ptr<ResourceManager::FontResource> uiFont;
    std::shared_ptr<ResourceManager::TextureResource> backgroundImage;

    Object* activeBird = nullptr;
    bool activeBirdLaunched = false;
    bool isDraggingBird = false;
    float launchedBirdIdleSeconds = 0.0f;

    int birdsLeftToSpawn = 0;
    int pigsRemaining = 0;
    int score = 0;
    int bestScore = 0;
    bool levelWon = false;
    bool levelLost = false;
    bool restartRequested = false;

    Vector2D slingAnchor = { 140.0f, 320.0f };
    float slingMaxStretch = 90.0f;
    float launchPower = 7.5f;
    float pigImpactThreshold = 200.0f;
    float blockMaxHealth = 200.0f;
    float blockDamageThreshold = 80.0f;
    float blockDamageScale = 0.65f;
    float startLockDurationSeconds = 1.0f;
    float goTextDurationSeconds = 0.85f;
    float roundElapsedSeconds = 0.0f;

    void BuildLevel();
    void ClearSceneObjects();

    Object* CreateBird(const Vector2D& position, Color color);
    Object* CreatePig(const Vector2D& position);
    Object* CreateBlock(const Vector2D& position, float halfWidth, float halfHeight, float mass);
    Object* CreateBoundary(const Vector2D& position, float halfWidth, float halfHeight);

    void SpawnNextBird();
    void HandleBirdDragAndLaunch();
    void ResolveCollisionsAndDamage();
    void ApplyBlockDamage(Object* block, EntityKind otherKind, float impactSpeed, std::vector<Object*>& removeList);
    void UpdateRoundState(float dt);

    void MarkObjectForRemoval(Object* obj, std::vector<Object*>& removeList);
    void RemoveObjects(const std::vector<Object*>& removeList);
    void RemoveObject(Object* obj);

    bool IsDynamic(Object* obj) const;
    bool IsInBounds(Object* obj) const;
    bool IsBirdReadyToRetire(Object* bird, float dt);
    bool IsStartLockActive() const;

    void DrawWorld();
    void DrawUI();
    void DrawSlingshot() const;
    void DrawTrajectoryPreview() const;

};

#endif //KITSUNEENGINE_ANGRYBALLS_H
