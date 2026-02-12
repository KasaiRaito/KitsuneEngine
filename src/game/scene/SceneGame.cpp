#include "SceneGame.h"

#include "Ball.h"
#include "CircleRenderComponent.h"
#include "ColliderComponent2D.h"
#include "CollisionSystem2D.h"
#include "InputComponent.h"
#include "Debug.h"
#include "raylib.h"
#include "resource_manager/ResourceManager.h"

#include <sstream>
#include <iomanip>
#include <vector> // CHANGED
#include <string>

static std::string ResolveAssetPath(const std::string& repoRelativePath)
{
    // Try common working directories (project root and build folders).
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

    // Return original path as fallback so warning logs still show intended file.
    return repoRelativePath;
}

SceneGame::SceneGame()
{
    // Load shared resources once; ResourceManager will return cached entries
    // if another scene/object asked for the same file previously.
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/ui.ttf");
    const std::string collisionPath = ResolveAssetPath("src/game/assets/audio/collision.mp3");
    const std::string bgmPath = ResolveAssetPath("src/game/assets/audio/bgm.ogg");
    const std::string backgroundPath = ResolveAssetPath("src/game/assets/textures/BackGround.jpg");

    uiFont = resources.GetOrLoadFont(fontPath);
    collisionSfx = resources.GetOrLoadSound(collisionPath);
    bgm = resources.GetOrLoadMusic(bgmPath);
    backgroundImage = resources.GetOrLoadTexture(backgroundPath);

    // Helpful runtime hints when files are missing or path is incorrect.
    if (!uiFont) TraceLog(LOG_WARNING, "SceneGame: could not load %s", fontPath.c_str());
    if (!collisionSfx) TraceLog(LOG_WARNING, "SceneGame: could not load %s", collisionPath.c_str());
    if (!bgm) TraceLog(LOG_WARNING, "SceneGame: could not load %s", bgmPath.c_str());
    if (!backgroundImage) TraceLog(LOG_WARNING, "SceneGame: could not load %s", backgroundPath.c_str());


    // Start background music only if the asset was loaded successfully.
    if (bgm)
        PlayMusicStream(bgm->value);

    Ball* Player = new Ball();
    Player->transform.location.value = { 200, 225 };

    auto* render = new CircleRenderComponent();
    render->radius = 25.0f;
    render->color = BLUE;

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    Player->AddComponent(render);
    Player->AddComponent(collider);

    auto* input = new InputComponent();
    Player->AddComponent(input);

    // Move axis
    auto* move = input->AddAxis2D("Move");
    move->up.Add(KEY_W);
    move->left.Add(KEY_A);
    move->down.Add(KEY_S);
    move->right.Add(KEY_D);

    // Interact action
    auto* interact = input->AddAction("Interact");
    interact->keys.Add(KEY_E);

    auto* toggleMovement = input->AddAction("Toggle Movement");
    toggleMovement->keys.Add(KEY_P);

    objects.Add(Player);

    Ball* ball = new Ball();
    ball->transform.location.value = { 300, 300 };
    auto* render2 = new CircleRenderComponent();
    render2->radius = 25.0f;
    render2->color = PINK;

    auto* collider2 = new ColliderComponent2D(ColliderType2D::Circle, render);

    ball->AddComponent(render2);
    ball->AddComponent(collider2);

    objects.Add(ball);

    SetTime(00,00,03,00);
}


SceneGame::~SceneGame()
{
    for (size_t i = 0; i < objects.Size(); i++)
        delete objects.Get(i);
}

void SceneGame::Update(float dt)
{
    UpdateClock(dt, false);

    // Music streams must be updated each frame while playing.
    if (bgm)
        UpdateMusicStream(bgm->value);

    std::vector<Vector2D> previousPositions; // CHANGED
    previousPositions.reserve(objects.Size()); // CHANGED

    for (size_t i = 0; i < objects.Size(); i++) // CHANGED
    { // CHANGED
        Object* obj = objects.Get(i); // CHANGED
        previousPositions.push_back(obj ? obj->transform.location.value : Vector2D::Zero()); // CHANGED
    } // CHANGED

    // Now update objects
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Update(dt);
    }

    // Collision pass (no extra updates here)
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* a = objects.Get(i);
        if (!a || !a->collider) continue;

        for (size_t j = i + 1; j < objects.Size(); j++)
        {
            Object* b = objects.Get(j);
            if (!b || !b->collider) continue;

            auto m = CollisionSystem2D::Test(*a->collider, *b->collider);

            ColliderComponent2D* ca = a->GetComponent<ColliderComponent2D>();
            ColliderComponent2D* cb = b->GetComponent<ColliderComponent2D>();

            if (!ca || !cb) continue;

            bool wasColliding = (ca->collidingWith.count(b) > 0); // CHANGED
            const bool isTriggerPair = a->collider->isTrigger || b->collider->isTrigger; // CHANGED

            if (m.colliding) // CHANGED
            { // CHANGED
                if (!isTriggerPair) // CHANGED
                { // CHANGED
                    const Vector2D prevA = previousPositions[i]; // CHANGED
                    const Vector2D prevB = previousPositions[j]; // CHANGED
                    const Vector2D movedA = a->transform.location.value - prevA; // CHANGED
                    const Vector2D movedB = b->transform.location.value - prevB; // CHANGED
                    const bool aMoved = movedA.Length() > 0.00001f; // CHANGED
                    const bool bMoved = movedB.Length() > 0.00001f; // CHANGED

                    if (aMoved && !bMoved) // CHANGED
                    { // CHANGED
                        a->transform.location.Translate(m.normal * (-m.penetration)); // CHANGED
                    } // CHANGED
                    else if (!aMoved && bMoved) // CHANGED
                    { // CHANGED
                        b->transform.location.Translate(m.normal * m.penetration); // CHANGED
                    } // CHANGED
                    else // CHANGED
                    { // CHANGED
                        const float halfPen = m.penetration * 0.5f; // CHANGED
                        a->transform.location.Translate(m.normal * (-halfPen)); // CHANGED
                        b->transform.location.Translate(m.normal * halfPen); // CHANGED
                    } // CHANGED

                    const float vaN = a->velocity.Dot(m.normal); // CHANGED
                    if (vaN > 0.0f) // CHANGED
                        a->velocity -= m.normal * vaN; // CHANGED

                    const float vbN = b->velocity.Dot(m.normal); // CHANGED
                    if (vbN < 0.0f) // CHANGED
                        b->velocity -= m.normal * vbN; // CHANGED
                } // CHANGED

                if (!wasColliding) // CHANGED
                { // CHANGED
                    ca->collidingWith.insert(b); // CHANGED
                    cb->collidingWith.insert(a); // CHANGED

                    // Real audio usage example: play one-shot SFX on collision enter.
                    if (!isTriggerPair && collisionSfx)
                        PlaySound(collisionSfx->value);

                    if (isTriggerPair) // CHANGED
                    { // CHANGED
                        ca->OnTriggerEnter(b); // CHANGED
                        cb->OnTriggerEnter(a); // CHANGED
                    } // CHANGED
                    else // CHANGED
                    { // CHANGED
                        ca->OnCollisionEnter(b); // CHANGED
                        cb->OnCollisionEnter(a); // CHANGED
                    } // CHANGED
                } // CHANGED
                else // CHANGED
                { // CHANGED
                    if (isTriggerPair) // CHANGED
                    { // CHANGED
                        ca->OnTriggerStay(b); // CHANGED
                        cb->OnTriggerStay(a); // CHANGED
                    } // CHANGED
                    else // CHANGED
                    { // CHANGED
                        ca->OnCollisionStay(b); // CHANGED
                        cb->OnCollisionStay(a); // CHANGED
                    } // CHANGED
                } // CHANGED
            } // CHANGED
            else // CHANGED
            { // CHANGED
                if (wasColliding) // CHANGED
                { // CHANGED
                    ca->collidingWith.erase(b); // CHANGED
                    cb->collidingWith.erase(a); // CHANGED

                    if (isTriggerPair) // CHANGED
                    { // CHANGED
                        ca->OnTriggerExit(b); // CHANGED
                        cb->OnTriggerExit(a); // CHANGED
                    } // CHANGED
                    else // CHANGED
                    { // CHANGED
                        ca->OnCollisionExit(b); // CHANGED
                        cb->OnCollisionExit(a); // CHANGED
                    } // CHANGED
                } // CHANGED
            } // CHANGED

        }
    }
}

void SceneGame::Draw()
{
    // Draw the scene background first so every other draw call appears above it.
    if (backgroundImage && backgroundImage->value.id != 0)
    {
        Texture2D& bg = backgroundImage->value;
        Rectangle src = { 0.0f, 0.0f, (float)bg.width, (float)bg.height };
        Rectangle dst = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
        DrawTexturePro(bg, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
    }
    else
    {
        // Visible hint when the texture failed to load.
        DrawText("Background image not loaded", 20, 130, 20, RED);
    }

    int h, m, s, ms;
    GetClockParts(h, m, s, ms);

    std::ostringstream clockTXT;
    clockTXT << std::setfill('0')
             << std::setw(2) << h << ":"
             << std::setw(2) << m << ":"
             << std::setw(2) << s << ":"
             << std::setw(3) << ms;

    DrawText(clockTXT.str().c_str(), 20, 100, 20, DARKBLUE);

    // Real font usage example:
    // use loaded font if available, otherwise fallback to default DrawText.
    if (uiFont)
        DrawTextEx(uiFont->value, "SceneGame (SPACE to switch)", {20.0f, 20.0f}, 24.0f, 1.0f, DARKBLUE);
    else
        DrawText("SceneGame (SPACE to switch)", 20, 20, 20, DARKBLUE);

    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Draw();
    }

    if (!Debug::GetDebug()) return;

    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj || !obj->collider) continue;
        obj->collider->DebugDraw();
    }
}

void SceneGame::SetTime(int h, int m, int s, int ms)
{
    // clamp negatives
    if (h < 0) h = 0;
    if (m < 0) m = 0;
    if (s < 0) s = 0;
    if (ms < 0) ms = 0;

    // optional: clamp max ranges
    if (h > 23) h = 0;
    if (m > 59) m = 59;
    if (s > 59) s = 59;
    if (ms > 999) ms = 999;

    totalMs = (h * 60 * 60 * 1000)
            + (m * 60 * 1000)
            + (s * 1000)
            + ms;
}

void SceneGame::UpdateClock(float dt, bool grow)
{
    int deltaMs = (int)(dt * 1000.0f);

    if (grow)
        totalMs += deltaMs;
    else
        totalMs -= deltaMs;

    // clamp at 0 (no negative time)
    if (totalMs < 0)
        totalMs = 0;

    if (totalMs >= dayMs)
    {
        totalMs %= dayMs;
    }

    // optional: wrap at 24 hours (uncomment if you want)
    // int dayMs = 24 * 60 * 60 * 1000;
    // totalMs %= dayMs;
}

void SceneGame::GetClockParts(int& h, int& m, int& s, int& ms) const
{
    int t = totalMs;

    ms = t % 1000;
    t /= 1000;

    s = t % 60;
    t /= 60;

    m = t % 60;
    t /= 60;

    h = t; // if you want wrap 24h: h %= 24;
}
