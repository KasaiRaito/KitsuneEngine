#include "AngryBalls/AngryBalls.h"

#include "CircleRenderComponent.h"
#include "SquareCollider2D.h"
#include "ColliderComponent2D.h"
#include "CollisionSystem2D.h"
#include "Debug.h"
#include "SceneManager.h"
#include "save_data/SaveData.h"
#include "physics_system/PhysicsComponent.h"
#include "physics_system/PhysicsSystem.h"
#include "raylib.h"
#include "raygui.h"

#include <algorithm>
#include <array>
#include <string>
#include <vector>

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

    static Vector2 ToRayVector(const Vector2D& value)
    {
        return { value.x, value.y };
    }

    static Vector2D ToEngineVector(const Vector2& value)
    {
        return { value.x, value.y };
    }

    static Vector2D ClampLength(const Vector2D& value, float maxLength)
    {
        const float length = value.Length();
        if (length <= maxLength || length <= 0.00001f)
            return value;

        return value * (maxLength / length);
    }

    static Color LerpColor(Color a, Color b, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);
        Color out{};
        out.r = (unsigned char)(a.r + (b.r - a.r) * t);
        out.g = (unsigned char)(a.g + (b.g - a.g) * t);
        out.b = (unsigned char)(a.b + (b.b - a.b) * t);
        out.a = (unsigned char)(a.a + (b.a - a.a) * t);
        return out;
    }

    static void DrawTextOutlined(const char* text, int x, int y, int fontSize, Color fill, Color outline, int thickness = 2)
    {
        if (!text || text[0] == '\0')
            return;

        for (int oy = -thickness; oy <= thickness; ++oy)
        {
            for (int ox = -thickness; ox <= thickness; ++ox)
            {
                if (ox == 0 && oy == 0)
                    continue;

                DrawText(text, x + ox, y + oy, fontSize, outline);
            }
        }

        DrawText(text, x, y, fontSize, fill);
    }

    static void ExchangeCollisionImpulse(Object* a, Object* b, const Vector2D& normal, float restitution = 0.28f)
    {
        if (!a || !b) return;

        auto* pa = a->GetComponent<PhysicsComponent>();
        auto* pb = b->GetComponent<PhysicsComponent>();
        if (!pa && !pb) return;

        const float invMassA = pa ? pa->GetInvMass() : 0.0f;
        const float invMassB = pb ? pb->GetInvMass() : 0.0f;
        const float invMassSum = invMassA + invMassB;
        if (invMassSum <= 0.0f) return;

        const Vector2D relativeVelocity = b->velocity - a->velocity;
        const float velocityAlongNormal = relativeVelocity.Dot(normal);
        if (velocityAlongNormal > 0.0f) return;

        const float impulseMagnitude = -(1.0f + restitution) * velocityAlongNormal / invMassSum;
        const Vector2D impulse = normal * impulseMagnitude;

        if (pa) a->velocity -= impulse * invMassA;
        if (pb) b->velocity += impulse * invMassB;
    }
}

AngryBalls::AngryBalls(SceneManager* manager)
    : sceneManager(manager)
{
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/DKKitsuneTail.ttf");
    const std::string backgroundPath = ResolveAssetPath("src/game/assets/textures/AngryBallsBackground.png");

    uiFont = resources.GetOrLoadFont(fontPath);
    backgroundImage = resources.GetOrLoadTexture(backgroundPath);
    bestScore = SaveData::Instance().GetAngryBestScore();

    BuildLevel();
}

AngryBalls::~AngryBalls()
{
    ClearSceneObjects();
}

void AngryBalls::BuildLevel()
{
    ClearSceneObjects();

    score = 0;
    roundElapsedSeconds = 0.0f;
    pigsRemaining = 0;
    birdsLeftToSpawn = 4;
    levelWon = false;
    levelLost = false;
    restartRequested = false;

    const float screenW = (float)GetScreenWidth();
    const float screenH = (float)GetScreenHeight();
    const float wallThickness = 40.0f;

    CreateBoundary({ screenW * 0.5f, screenH + wallThickness * 0.5f }, screenW * 0.5f, wallThickness * 0.5f);
    CreateBoundary({ -wallThickness * 0.5f, screenH * 0.5f }, wallThickness * 0.5f, screenH * 0.5f);
    CreateBoundary({ screenW + wallThickness * 0.5f, screenH * 0.5f }, wallThickness * 0.5f, screenH * 0.5f);
    CreateBoundary({ screenW * 0.5f, -wallThickness * 0.5f }, screenW * 0.5f, wallThickness * 0.5f);

    CreateBlock({ 560.0f, 330.0f }, 18.0f, 58.0f, 1.2f);
    CreateBlock({ 620.0f, 330.0f }, 18.0f, 58.0f, 1.2f);
    CreateBlock({ 590.0f, 265.0f }, 70.0f, 15.0f, 1.0f);
    CreatePig({ 590.0f, 225.0f });

    CreateBlock({ 700.0f, 340.0f }, 24.0f, 48.0f, 1.3f);
    CreateBlock({ 740.0f, 340.0f }, 24.0f, 48.0f, 1.3f);
    CreateBlock({ 720.0f, 282.0f }, 55.0f, 14.0f, 1.0f);
    CreatePig({ 720.0f, 245.0f });

    CreatePig({ 660.0f, 370.0f });

    SpawnNextBird();
}

void AngryBalls::ClearSceneObjects()
{
    for (size_t i = 0; i < objects.Size(); ++i)
    {
        delete objects.Get((int)i);
    }

    objects.Clear();
    visuals.clear();
    blockHealth.clear();

    activeBird = nullptr;
    activeBirdLaunched = false;
    isDraggingBird = false;
    launchedBirdIdleSeconds = 0.0f;
}

Object* AngryBalls::CreateBird(const Vector2D& position, Color color)
{
    Object* bird = new Object();
    bird->transform.location.value = position;

    auto* render = new CircleRenderComponent();
    render->radius = 16.0f;
    render->color = color;

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    auto* physics = new PhysicsComponent();
    physics->SetMass(1.0f);
    physics->useGravity = false;
    physics->kinematic = true;
    physics->linearDamping = 0.0f;
    physics->maxSpeed = -1.0f;

    bird->AddComponent(render);
    bird->AddComponent(collider);
    bird->AddComponent(physics);

    objects.Add(bird);
    visuals[bird] = { EntityKind::Bird, color, render->radius, 0.0f, 0.0f };

    return bird;
}

Object* AngryBalls::CreatePig(const Vector2D& position)
{
    Object* pig = new Object();
    pig->transform.location.value = position;

    auto* render = new CircleRenderComponent();
    render->radius = 18.0f;
    render->color = Color{ 124, 204, 80, 255 };

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    auto* physics = new PhysicsComponent();
    physics->SetMass(1.0f);
    physics->useGravity = true;
    physics->linearDamping = 1.8f;
    physics->maxSpeed = 760.0f;

    pig->AddComponent(render);
    pig->AddComponent(collider);
    pig->AddComponent(physics);

    objects.Add(pig);
    visuals[pig] = { EntityKind::Pig, render->color, render->radius, 0.0f, 0.0f };
    pigsRemaining++;

    return pig;
}

Object* AngryBalls::CreateBlock(const Vector2D& position, float halfWidth, float halfHeight, float mass)
{
    Object* block = new Object();
    block->transform.location.value = position;

    SquareCollider2D squareShape(&block->transform.location.value, halfWidth, halfHeight);
    auto* collider = new ColliderComponent2D(ColliderType2D::Square, &squareShape);

    auto* physics = new PhysicsComponent();
    physics->SetMass(mass);
    physics->useGravity = true;
    physics->linearDamping = 1.6f;
    physics->maxSpeed = 720.0f;

    block->AddComponent(collider);
    block->AddComponent(physics);

    objects.Add(block);
    visuals[block] = { EntityKind::Block, Color{ 186, 132, 82, 255 }, 0.0f, halfWidth, halfHeight };
    blockHealth[block] = blockMaxHealth;

    return block;
}

Object* AngryBalls::CreateBoundary(const Vector2D& position, float halfWidth, float halfHeight)
{
    Object* boundary = new Object();
    boundary->transform.location.value = position;

    SquareCollider2D squareShape(&boundary->transform.location.value, halfWidth, halfHeight);
    auto* collider = new ColliderComponent2D(ColliderType2D::Square, &squareShape);

    auto* physics = new PhysicsComponent();
    physics->kinematic = true;
    physics->useGravity = false;
    physics->linearDamping = 0.0f;

    boundary->AddComponent(collider);
    boundary->AddComponent(physics);

    objects.Add(boundary);
    visuals[boundary] = { EntityKind::Boundary, Color{ 0, 0, 0, 0 }, 0.0f, halfWidth, halfHeight };

    return boundary;
}

void AngryBalls::SpawnNextBird()
{
    if (activeBird || birdsLeftToSpawn <= 0 || levelWon || levelLost)
        return;

    static const std::array<Color, 4> birdPalette = {
        Color{ 214, 58, 56, 255 },
        Color{ 242, 198, 69, 255 },
        Color{ 65, 160, 219, 255 },
        Color{ 250, 128, 98, 255 }
    };

    const int paletteIndex = 4 - birdsLeftToSpawn;
    activeBird = CreateBird(slingAnchor, birdPalette[paletteIndex % birdPalette.size()]);

    activeBirdLaunched = false;
    isDraggingBird = false;
    launchedBirdIdleSeconds = 0.0f;
    birdsLeftToSpawn--;
}

void AngryBalls::HandleBirdDragAndLaunch()
{
    if (!activeBird || activeBirdLaunched || levelWon || levelLost)
        return;

    auto* physics = activeBird->GetComponent<PhysicsComponent>();
    if (!physics)
        return;

    if (!isDraggingBird)
    {
        activeBird->transform.location.value = slingAnchor;
        activeBird->velocity = Vector2D::Zero();
    }

    if (IsStartLockActive())
    {
        isDraggingBird = false;
        activeBird->transform.location.value = slingAnchor;
        activeBird->velocity = Vector2D::Zero();
        return;
    }

    float pickupRadius = 20.0f;
    if (auto it = visuals.find(activeBird); it != visuals.end())
        pickupRadius = it->second.radius + 8.0f;

    const Vector2D mouse = ToEngineVector(GetMousePosition());
    const float distanceToBird = (mouse - activeBird->transform.location.value).Length();

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && distanceToBird <= pickupRadius)
    {
        isDraggingBird = true;
    }

    if (!isDraggingBird)
        return;

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
    {
        Vector2D stretch = mouse - slingAnchor;
        stretch = ClampLength(stretch, slingMaxStretch);

        activeBird->transform.location.value = slingAnchor + stretch;
        activeBird->velocity = Vector2D::Zero();
        return;
    }

    if (!IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        return;

    isDraggingBird = false;

    const Vector2D launchVector = slingAnchor - activeBird->transform.location.value;
    if (launchVector.Length() < 8.0f)
    {
        activeBird->transform.location.value = slingAnchor;
        return;
    }

    physics->kinematic = false;
    physics->useGravity = true;

    activeBird->velocity = launchVector * launchPower;
    activeBirdLaunched = true;
}

void AngryBalls::MarkObjectForRemoval(Object* obj, std::vector<Object*>& removeList)
{
    if (!obj)
        return;

    if (std::find(removeList.begin(), removeList.end(), obj) != removeList.end())
        return;

    removeList.push_back(obj);
}

void AngryBalls::RemoveObjects(const std::vector<Object*>& removeList)
{
    for (Object* obj : removeList)
    {
        RemoveObject(obj);
    }
}

void AngryBalls::RemoveObject(Object* obj)
{
    if (!obj)
        return;

    if (obj == activeBird)
    {
        activeBird = nullptr;
        activeBirdLaunched = false;
        isDraggingBird = false;
        launchedBirdIdleSeconds = 0.0f;
    }

    if (auto it = visuals.find(obj); it != visuals.end())
    {
        if (it->second.kind == EntityKind::Pig)
        {
            pigsRemaining = std::max(0, pigsRemaining - 1);
            score += 5000;
        }
        else if (it->second.kind == EntityKind::Block)
        {
            score += 750;
        }

        if (score > bestScore)
        {
            bestScore = score;
            SaveData::Instance().SetAngryBestScore(bestScore);
        }

        visuals.erase(it);
    }

    blockHealth.erase(obj);

    for (size_t i = 0; i < objects.Size(); ++i)
    {
        if (objects.Get((int)i) != obj)
            continue;

        objects.RemoveAt(i);
        break;
    }

    delete obj;
}

void AngryBalls::ApplyBlockDamage(Object* block, EntityKind otherKind, float impactSpeed, std::vector<Object*>& removeList)
{
    auto hpIt = blockHealth.find(block);
    if (hpIt == blockHealth.end())
        return;

    float impactMultiplier = 1.0f;
    if (otherKind == EntityKind::Bird)
        impactMultiplier = 1.8f;
    else if (otherKind == EntityKind::Pig)
        impactMultiplier = 1.2f;
    else if (otherKind == EntityKind::Boundary)
        impactMultiplier = 0.8f;

    const float damage = std::max(0.0f, impactSpeed - blockDamageThreshold) * blockDamageScale * impactMultiplier;
    if (damage <= 0.0f)
        return;

    hpIt->second -= damage;

    if (auto visualIt = visuals.find(block); visualIt != visuals.end())
    {
        const float ratio = std::clamp(hpIt->second / blockMaxHealth, 0.0f, 1.0f);
        const Color healthy = { 186, 132, 82, 255 };
        const Color damaged = { 86, 62, 46, 255 };
        visualIt->second.color = LerpColor(damaged, healthy, ratio);
    }

    if (hpIt->second <= 0.0f)
        MarkObjectForRemoval(block, removeList);
}

bool AngryBalls::IsDynamic(Object* obj) const
{
    if (!obj)
        return false;

    auto* physics = obj->GetComponent<PhysicsComponent>();
    if (!physics)
        return false;

    return physics->enabled && !physics->kinematic;
}

bool AngryBalls::IsInBounds(Object* obj) const
{
    if (!obj)
        return false;

    const float margin = 220.0f;
    const Vector2D position = obj->transform.location.value;

    return position.x >= -margin
        && position.x <= (float)GetScreenWidth() + margin
        && position.y >= -margin
        && position.y <= (float)GetScreenHeight() + margin;
}

bool AngryBalls::IsBirdReadyToRetire(Object* bird, float dt)
{
    if (!bird)
        return false;

    if (!IsInBounds(bird))
        return true;

    const float speed = bird->velocity.Length();
    if (speed < 25.0f)
        launchedBirdIdleSeconds += dt;
    else
        launchedBirdIdleSeconds = 0.0f;

    return launchedBirdIdleSeconds > 1.5f;
}

bool AngryBalls::IsStartLockActive() const
{
    return roundElapsedSeconds < startLockDurationSeconds;
}

void AngryBalls::ResolveCollisionsAndDamage()
{
    std::vector<Object*> removeList;
    const bool startLockActive = IsStartLockActive();

    for (size_t i = 0; i < objects.Size(); ++i)
    {
        Object* a = objects.Get((int)i);
        if (!a || !a->collider)
            continue;

        for (size_t j = i + 1; j < objects.Size(); ++j)
        {
            Object* b = objects.Get((int)j);
            if (!b || !b->collider)
                continue;

            CollisionManifold2D manifold = CollisionSystem2D::Test(*a->collider, *b->collider);
            if (!manifold.colliding)
                continue;

            const float impactSpeed = (b->velocity - a->velocity).Length();

            const bool dynamicA = IsDynamic(a);
            const bool dynamicB = IsDynamic(b);

            if (dynamicA && dynamicB)
            {
                const Vector2D correction = manifold.normal * (manifold.penetration * 0.5f);
                a->transform.location.Translate(correction * -1.0f);
                b->transform.location.Translate(correction);
            }
            else if (dynamicA && !dynamicB)
            {
                a->transform.location.Translate(manifold.normal * (-manifold.penetration));
            }
            else if (!dynamicA && dynamicB)
            {
                b->transform.location.Translate(manifold.normal * manifold.penetration);
            }

            ExchangeCollisionImpulse(a, b, manifold.normal, 0.30f);

            auto itA = visuals.find(a);
            auto itB = visuals.find(b);
            if (itA == visuals.end() || itB == visuals.end())
                continue;

            const EntityKind kindA = itA->second.kind;
            const EntityKind kindB = itB->second.kind;

            if (!startLockActive && kindA == EntityKind::Pig && impactSpeed > pigImpactThreshold)
                MarkObjectForRemoval(a, removeList);

            if (!startLockActive && kindB == EntityKind::Pig && impactSpeed > pigImpactThreshold)
                MarkObjectForRemoval(b, removeList);

            if (!startLockActive && kindA == EntityKind::Block)
                ApplyBlockDamage(a, kindB, impactSpeed, removeList);

            if (!startLockActive && kindB == EntityKind::Block)
                ApplyBlockDamage(b, kindA, impactSpeed, removeList);
        }
    }

    for (size_t i = 0; i < objects.Size(); ++i)
    {
        Object* obj = objects.Get((int)i);
        if (!obj)
            continue;

        auto it = visuals.find(obj);
        if (it == visuals.end())
            continue;

        const EntityKind kind = it->second.kind;
        if (kind == EntityKind::Boundary)
            continue;

        if (startLockActive)
            continue;

        if (!IsInBounds(obj))
            MarkObjectForRemoval(obj, removeList);
    }

    RemoveObjects(removeList);
}

void AngryBalls::UpdateRoundState(float dt)
{
    if (pigsRemaining <= 0)
    {
        levelWon = true;
        levelLost = false;
        return;
    }

    if (activeBird && activeBirdLaunched)
    {
        if (IsBirdReadyToRetire(activeBird, dt))
            RemoveObject(activeBird);
    }

    if (!activeBird && birdsLeftToSpawn > 0)
        SpawnNextBird();

    if (!activeBird && birdsLeftToSpawn <= 0 && pigsRemaining > 0)
        levelLost = true;
}

void AngryBalls::Update(float dt)
{
    if (IsKeyPressed(KEY_R))
        restartRequested = true;

    if (restartRequested)
    {
        BuildLevel();
        return;
    }

    roundElapsedSeconds += dt;

    HandleBirdDragAndLaunch();

    for (size_t i = 0; i < objects.Size(); ++i)
    {
        Object* obj = objects.Get((int)i);
        if (!obj)
            continue;

        obj->Update(dt);
    }

    ResolveCollisionsAndDamage();
    UpdateRoundState(dt);
}

void AngryBalls::DrawSlingshot() const
{
    const Vector2 base = ToRayVector(slingAnchor);

    DrawLineEx({ base.x - 26.0f, base.y + 64.0f }, { base.x - 10.0f, base.y - 32.0f }, 8.0f, DARKBROWN);
    DrawLineEx({ base.x + 26.0f, base.y + 64.0f }, { base.x + 10.0f, base.y - 32.0f }, 8.0f, DARKBROWN);

    Vector2 bandTarget = base;
    if (activeBird && !activeBirdLaunched)
        bandTarget = ToRayVector(activeBird->transform.location.value);

    DrawLineEx({ base.x - 10.0f, base.y - 26.0f }, bandTarget, 3.0f, Color{ 87, 44, 18, 255 });
    DrawLineEx({ base.x + 10.0f, base.y - 26.0f }, bandTarget, 3.0f, Color{ 87, 44, 18, 255 });
}

void AngryBalls::DrawTrajectoryPreview() const
{
    if (!activeBird || activeBirdLaunched || !isDraggingBird)
        return;

    const Vector2D start = activeBird->transform.location.value;
    const Vector2D velocity = (slingAnchor - start) * launchPower;
    const Vector2D gravity = PhysicsSystem::GetGravity();

    for (int i = 1; i <= 20; ++i)
    {
        const float t = (float)i * 0.07f;
        const Vector2D point = start + velocity * t + gravity * (0.5f * t * t);
        const float alpha = std::max(0.15f, 0.95f - ((float)i * 0.04f));

        DrawCircleV(ToRayVector(point), 3.0f, ColorAlpha(WHITE, alpha));
    }
}

void AngryBalls::DrawWorld()
{
    if (backgroundImage && backgroundImage->value.id != 0)
    {
        Texture2D& bg = backgroundImage->value;
        DrawTexturePro(
            bg,
            { 0.0f, 0.0f, (float)bg.width, (float)bg.height },
            { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() },
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }

    DrawRectangle(0, GetScreenHeight() - 28, GetScreenWidth(), 72, Color{ 113, 80, 52, 255 });

    DrawTrajectoryPreview();
    DrawSlingshot();

    for (const auto& [obj, visual] : visuals)
    {
        if (!obj || visual.kind != EntityKind::Block)
            continue;

        const Vector2 position = ToRayVector(obj->transform.location.value);
        DrawRectanglePro(
            { position.x, position.y, visual.halfWidth * 2.0f, visual.halfHeight * 2.0f },
            { visual.halfWidth, visual.halfHeight },
            0.0f,
            visual.color
        );
    }

    for (size_t i = 0; i < objects.Size(); ++i)
    {
        Object* obj = objects.Get((int)i);
        if (!obj)
            continue;

        obj->Draw();
    }

    if (!Debug::GetDebug())
        return;

    for (size_t i = 0; i < objects.Size(); ++i)
    {
        Object* obj = objects.Get((int)i);
        if (!obj || !obj->collider)
            continue;

        obj->collider->DebugDraw();
    }
}

void AngryBalls::DrawUI()
{
    if (GuiButton({ 620, 20, 160, 32 }, "Exit to Main") && sceneManager)
        sceneManager->LoadScene(0);

    if (GuiButton({ 620, 60, 160, 32 }, "Restart [R]"))
        restartRequested = true;

    if (uiFont)
        DrawTextEx(uiFont->value, "Kitsune Angry Birds", { 20.0f, 20.0f }, 30.0f, 1.0f, BLACK);
    else
        DrawText("Kitsune Angry Birds", 20, 20, 30, BLACK);

    const int birdsInPlay = birdsLeftToSpawn + (activeBird ? 1 : 0);

    DrawText(TextFormat("Score: %d", score), 20, 64, 22, BLACK);
    DrawText(TextFormat("Best: %d", bestScore), 20, 92, 22, BLACK);
    DrawText(TextFormat("Birds: %d", birdsInPlay), 20, 120, 22, BLACK);
    DrawText(TextFormat("Pigs: %d", pigsRemaining), 20, 148, 22, BLACK);

    if (IsStartLockActive())
    {
        const float unlockInSeconds = std::max(0.0f, startLockDurationSeconds - roundElapsedSeconds);
        const int centerX = GetScreenWidth() / 2;
        const int centerY = GetScreenHeight() / 2;

        const char* readyText = "READY!";
        const int readySize = 78;
        const int readyW = MeasureText(readyText, readySize);
        DrawTextOutlined(
            readyText,
            centerX - (readyW / 2),
            centerY - 54,
            readySize,
            Color{ 255, 224, 140, 255 },
            BLACK,
            3
        );

        DrawText(TextFormat("%.1f", unlockInSeconds), centerX - 20, centerY + 24, 44, BLACK);
        DrawText("Round protection active...", 20, 214, 19, BLACK);
    }
    else if (roundElapsedSeconds < startLockDurationSeconds + goTextDurationSeconds)
    {
        const int centerX = GetScreenWidth() / 2;
        const int centerY = GetScreenHeight() / 2;
        const char* goText = "GO!";
        const int goSize = 88;
        const int goW = MeasureText(goText, goSize);
        DrawTextOutlined(
            goText,
            centerX - (goW / 2),
            centerY - 34,
            goSize,
            Color{ 189, 250, 104, 255 },
            BLACK,
            3
        );
    }

    if (!IsStartLockActive())
    {
        DrawText("Click and drag the bird, then release to launch.", 20, 188, 19, BLACK);
        DrawText("Eliminate all pigs. Press R to reset the round.", 20, 212, 19, BLACK);
    }

    if (levelWon)
    {
        DrawText("LEVEL CLEARED", 275, 80, 40, BLACK);
        DrawText("Press R or click Restart to play again", 220, 125, 24, BLACK);
    }
    else if (levelLost)
    {
        DrawText("OUT OF BIRDS", 290, 80, 40, BLACK);
        DrawText("Press R or click Restart to try again", 228, 125, 24, BLACK);
    }
}

void AngryBalls::Draw()
{
    DrawWorld();
    DrawUI();
}
