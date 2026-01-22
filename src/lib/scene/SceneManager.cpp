#include "SceneManager.h"

SceneManager::SceneManager() = default;

void SceneManager::AddScene(const SceneInfo& scene)
{
    scenes.Add(scene);

    // If this is the first scene added, auto-select it
    if (currentSceneIndex == -1)
        currentSceneIndex = 0;
}

SceneInfo SceneManager::GetCurrentScene() const
{
    if (currentSceneIndex < 0 || scenes.Size() == 0)
        return SceneInfo(nullptr, "none", -1);

    return scenes[currentSceneIndex];
}

int SceneManager::GetCurrentSceneIndex() const
{
    return currentSceneIndex;
}

SceneInfo* SceneManager::GetNextScene()
{
    if (scenes.Size() == 0) return nullptr;

    int next = currentSceneIndex + 1;
    if (next >= scenes.Size()) return nullptr;

    currentSceneIndex = next;
    return &scenes[currentSceneIndex];
}

void SceneManager::LoadScene(int index)
{
    if (index < 0 || index >= scenes.Size()) return;
    currentSceneIndex = index;
}

void SceneManager::NextScene()
{
    const int count = (int)scenes.Size();
    if (count <= 0) return;

    int next = currentSceneIndex + 1;
    if (next >= count) next = 0;

    LoadScene(next);
}

