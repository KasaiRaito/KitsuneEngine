#include "SceneMain.h"
#include "Ball.h"
#include "CircleRenderComponent.h"
#include "ColliderComponent2D.h"
#include "CollisionSystem2D.h"
#include "Debug.h"
#include "InputSystem.h"
#include "SceneManager.h"
#include "ResourceManager.h"

#include "raylib.h"
#include "raygui.h"

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

SceneMain::SceneMain(SceneManager* manager)
    : sceneManager(manager)
{
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/DKKitsuneTail.ttf");

    uiFont = resources.GetOrLoadFont(fontPath);

    Ball* ball = new Ball();
    ball->transform.location.value = {400, 225};

    auto* render = new CircleRenderComponent();
    render->radius = 30.0f;
    render->color = RED;

    auto* collider = new ColliderComponent2D(ColliderType2D::Circle, render);

    ball->AddComponent(render);
    ball->AddComponent(collider);

    objects.Add(ball); // objects should be List<Object*>
}

SceneMain::~SceneMain()
{
    for (size_t i = 0; i < objects.Size(); i++)
        delete objects.Get(i);
}

void SceneMain::Update(float dt)
{
    // Input once per frame
    InputSystem::ProcessInput();

    // Update objects
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Update(dt);
    }

    // Collision pass
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* a = objects.Get(i);
        if (!a || !a->collider) continue;

        for (size_t j = i + 1; j < objects.Size(); j++)
        {
            Object* b = objects.Get(j);
            if (!b || !b->collider) continue;

            auto result = CollisionSystem2D::Test(*a->collider, *b->collider);
            if (result.colliding)
            {
                // later: resolve using manifold
            }
        }
    }
}

void SceneMain::Draw()
{
    // If SceneBase::Draw() is abstract/empty, you can remove this call.
    SceneBase::Draw();

    if (GuiButton({ 620, 20, 160, 32 }, "Go to SceneGame") && sceneManager)
        sceneManager->LoadScene(1);

    // Draw objects
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Draw();
    }

    if (uiFont)
        DrawTextEx(uiFont->value, "KitsuneEngine", {20.0f, 20.0f}, 24.0f, 1.0f, BLACK);
    else
        DrawText("KitsuneEngine", 20, 20, 20, RED);

    // Debug overlay
    if (!Debug::GetDebug()) return;

    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj || !obj->collider) continue;

        obj->collider->DebugDraw();
    }
}
