#include "SceneMain.h"
#include "CollisionSystem2D.h"
#include "Debug.h"
#include "InputSystem.h"
#include "SceneManager.h"
#include "ResourceManager.h"

#include "raylib.h"
#include "raygui.h"
#include "nlohmann/json.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

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

static std::filesystem::path ResolveDirectoryPath(const std::string& repoRelativeDirectory)
{
    const std::string candidates[] = {
        repoRelativeDirectory,
        "../" + repoRelativeDirectory,
        "../../" + repoRelativeDirectory,
        "../../../" + repoRelativeDirectory
    };

    for (const std::string& candidate : candidates)
    {
        std::error_code ec;
        if (std::filesystem::is_directory(candidate, ec))
            return candidate;
    }

    return repoRelativeDirectory;
}

static std::filesystem::path ResolveRepoRoot()
{
    const std::filesystem::path candidates[] = {
        std::filesystem::path("."),
        std::filesystem::path(".."),
        std::filesystem::path("../.."),
        std::filesystem::path("../../..")
    };

    for (const auto& candidate : candidates)
    {
        std::error_code ec;
        if (std::filesystem::exists(candidate / "src", ec)
            && std::filesystem::exists(candidate / "CMakeLists.txt", ec))
        {
            return std::filesystem::absolute(candidate);
        }
    }

    return std::filesystem::absolute(".");
}

static bool IsImageFile(const std::filesystem::path& path)
{
    if (!path.has_extension())
        return false;

    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return (char)std::tolower(c); });

    return extension == ".png"
        || extension == ".jpg"
        || extension == ".jpeg"
        || extension == ".bmp"
        || extension == ".tga"
        || extension == ".webp";
}

static bool IsVideoFile(const std::filesystem::path& path)
{
    if (!path.has_extension())
        return false;

    std::string extension = path.extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(),
                   [](unsigned char c) { return (char)std::tolower(c); });

    return extension == ".mp4"
        || extension == ".mov"
        || extension == ".m4v"
        || extension == ".webm";
}

static std::string ResolveFfmpegExecutable()
{
    const std::filesystem::path candidates[] = {
        "/opt/homebrew/bin/ffmpeg",
        "/usr/local/bin/ffmpeg"
    };

    std::error_code ec;
    for (const auto& candidate : candidates)
    {
        if (std::filesystem::exists(candidate, ec))
            return candidate.string();
    }

    return "ffmpeg";
}

static std::string EscapeShellArg(const std::string& value)
{
    std::string escaped = "'";
    for (char c : value)
    {
        if (c == '\'')
            escaped += "'\\''";
        else
            escaped += c;
    }
    escaped += "'";
    return escaped;
}

static std::vector<std::filesystem::path> CollectSortedImagePaths(const std::filesystem::path& directoryPath)
{
    std::vector<std::filesystem::path> framePaths;

    std::error_code ec;
    if (!std::filesystem::is_directory(directoryPath, ec))
        return framePaths;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath, ec))
    {
        if (!entry.is_regular_file())
            continue;

        if (!IsImageFile(entry.path()))
            continue;

        framePaths.push_back(entry.path());
    }

    std::sort(framePaths.begin(), framePaths.end());
    return framePaths;
}

static bool ExtractFramesFromVideo(const std::filesystem::path& videoPath,
                                   const std::filesystem::path& outputDirectory,
                                   float fps)
{
    std::error_code ec;
    std::filesystem::create_directories(outputDirectory, ec);

    auto cachedFrames = CollectSortedImagePaths(outputDirectory);
    if (!cachedFrames.empty())
    {
        const auto videoWriteTime = std::filesystem::last_write_time(videoPath, ec);
        if (!ec)
        {
            const auto cachedWriteTime = std::filesystem::last_write_time(cachedFrames.back(), ec);
            if (!ec && cachedWriteTime >= videoWriteTime)
                return true;
        }
    }

    for (const auto& cached : cachedFrames)
        std::filesystem::remove(cached, ec);

    const int targetFps = std::max(1, (int)fps);
    const std::string filter = "fps=" + std::to_string(targetFps);
    const std::string outputPattern = (outputDirectory / "frame_%05d.png").string();

    std::string command = EscapeShellArg(ResolveFfmpegExecutable()) + " -y -loglevel error -i "
        + EscapeShellArg(videoPath.string())
        + " -vf " + EscapeShellArg(filter)
        + " " + EscapeShellArg(outputPattern)
        + " > /dev/null 2>&1";

    const int result = std::system(command.c_str());
    if (result != 0)
        return false;

    cachedFrames = CollectSortedImagePaths(outputDirectory);
    return !cachedFrames.empty();
}

SceneMain::SceneMain(SceneManager* manager, bool preloadPreviewFrames)
    : sceneManager(manager)
{
    auto& resources = ResourceManager::Instance();
    const std::string fontPath = ResolveAssetPath("src/game/assets/fonts/DKKitsuneTail.ttf");

    uiFont = resources.GetOrLoadFont(fontPath);

    if (!preloadPreviewFrames)
        return;

    PreloadAngryPreviewFrames();
    PreloadDinoPreviewFrames();
    PreloadSpacePreviewFrames();
    PreloadWaterPreviewFrames();
}

SceneMain::~SceneMain()
{
    for (size_t i = 0; i < objects.Size(); i++)
        delete objects.Get(i);
}

void SceneMain::PreloadAngryPreviewFrames()
{
    if (angryPreviewLoadAttempted)
        return;

    angryPreviewLoadAttempted = true;
    angryPreviewFrames = LoadPreviewFrames("src/game/assets/previews/angryballs");
}

void SceneMain::PreloadDinoPreviewFrames()
{
    if (dinoPreviewLoadAttempted)
        return;

    dinoPreviewLoadAttempted = true;
    dinoPreviewFrames = LoadPreviewFrames("src/game/assets/previews/dinojump");
}

void SceneMain::PreloadSpacePreviewFrames()
{
    if (spacePreviewLoadAttempted)
        return;

    spacePreviewLoadAttempted = true;
    spacePreviewFrames = LoadPreviewFrames("src/game/assets/previews/space_invaders");
    if (spacePreviewFrames.empty())
        spacePreviewFrames = LoadPreviewFrames("src/game/assets/previews/spaceinvaders");
}

void SceneMain::PreloadWaterPreviewFrames()
{
    if (waterPreviewLoadAttempted)
        return;

    waterPreviewLoadAttempted = true;
    waterPreviewFrames = LoadPreviewFrames("src/game/assets/previews/WhereIsMyWater");
    if (waterPreviewFrames.empty())
        waterPreviewFrames = LoadPreviewFrames("src/game/assets/previews/where_is_my_water");
    if (waterPreviewFrames.empty())
        waterPreviewFrames = LoadPreviewFrames("src/game/assets/previews/whereismywater");
    if (waterPreviewFrames.empty())
        waterPreviewFrames = LoadPreviewFrames("src/game/assets/previews/where_is_my_whater");
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

std::vector<std::shared_ptr<ResourceManager::TextureResource>> SceneMain::LoadPreviewFrames(const std::string& repoRelativeDirectory)
{
    std::vector<std::shared_ptr<ResourceManager::TextureResource>> frames;

    const std::filesystem::path directoryPath = ResolveDirectoryPath(repoRelativeDirectory);
    std::error_code ec;
    if (!std::filesystem::is_directory(directoryPath, ec))
        return frames;

    std::vector<std::filesystem::path> framePaths = CollectSortedImagePaths(directoryPath);

    if (framePaths.empty())
    {
        std::vector<std::filesystem::path> videoPaths;
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath, ec))
        {
            if (!entry.is_regular_file())
                continue;

            if (!IsVideoFile(entry.path()))
                continue;

            videoPaths.push_back(entry.path());
        }

        std::sort(videoPaths.begin(), videoPaths.end());

        if (!videoPaths.empty())
        {
            const std::filesystem::path repoRoot = ResolveRepoRoot();
            const std::string folderKey = directoryPath.filename().string().empty()
                ? "preview"
                : directoryPath.filename().string();
            const std::filesystem::path cacheDirectory = repoRoot / "save_data" / "preview_cache" / folderKey;

            if (ExtractFramesFromVideo(videoPaths.front(), cacheDirectory, previewFps))
                framePaths = CollectSortedImagePaths(cacheDirectory);
        }
    }

    auto& resources = ResourceManager::Instance();
    for (const auto& framePath : framePaths)
    {
        auto texture = resources.GetOrLoadTexture(framePath.string());
        if (texture && texture->value.id != 0)
            frames.push_back(texture);
    }

    return frames;
}

void SceneMain::DrawPreviewPanel(const Rectangle& panelRect,
                                 const std::vector<std::shared_ptr<ResourceManager::TextureResource>>& frames,
                                 const char* title,
                                 const char* emptyHint) const
{
    DrawRectangleRounded(panelRect, 0.08f, 8, Fade(BLACK, 0.70f));
    DrawRectangleRoundedLinesEx(panelRect, 0.08f, 8, 2.0f, Fade(WHITE, 0.75f));

    DrawText(title, (int)panelRect.x + 14, (int)panelRect.y + 10, 20, WHITE);

    Rectangle content = panelRect;
    content.x += 10.0f;
    content.y += 38.0f;
    content.width -= 20.0f;
    content.height -= 48.0f;

    if (frames.empty())
    {
        DrawRectangleRec(content, Fade(BLACK, 0.35f));
        DrawRectangleLinesEx(content, 1.0f, Fade(WHITE, 0.35f));
        DrawText(emptyHint, (int)content.x + 10, (int)(content.y + content.height * 0.5f) - 10, 18, LIGHTGRAY);
        return;
    }

    const int frameIndex = ((int)(GetTime() * previewFps)) % (int)frames.size();
    const Texture2D& texture = frames[(size_t)frameIndex]->value;

    Rectangle src = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
    Rectangle dst = content;

    const float textureAspect = (float)texture.width / (float)texture.height;
    const float panelAspect = content.width / content.height;

    if (textureAspect > panelAspect)
    {
        const float fittedHeight = content.width / textureAspect;
        dst.y += (content.height - fittedHeight) * 0.5f;
        dst.height = fittedHeight;
    }
    else
    {
        const float fittedWidth = content.height * textureAspect;
        dst.x += (content.width - fittedWidth) * 0.5f;
        dst.width = fittedWidth;
    }

    DrawRectangleRec(content, Fade(BLACK, 0.20f));
    DrawTexturePro(texture, src, dst, { 0.0f, 0.0f }, 0.0f, WHITE);
}

void SceneMain::Draw()
{
    // If SceneBase::Draw() is abstract/empty, you can remove this call.
    SceneBase::Draw();

    const float buttonWidth = 260.0f;
    const float buttonHeight = 44.0f;
    const float buttonHeightHovered = 62.0f;
    const float buttonSpacing = 16.0f;
    const float buttonRightMargin = 52.0f;
    const size_t quitButtonIndex = 4;
    const float quitButtonWidth = 206.0f;
    const float quitButtonHeight = 34.0f;
    const float buttonX = (float)GetScreenWidth() - buttonWidth - buttonRightMargin;
    const Vector2 mouse = InputSystem::GetMousePosition();

    const std::array<const char*, 5> labels = {
        "Play Angry Balls",
        "Play Dino Jump",
        "Play Space Invaders",
        "Play Where Is My Water",
        "Quit Game"
    };
    const std::array<int, 4> sceneIndexes = { 1, 13, 14, 8 };

    std::array<Rectangle, 5> probeRects{};
    {
        float baseTotalHeight = buttonSpacing * ((float)probeRects.size() - 1.0f);
        for (size_t i = 0; i < probeRects.size(); ++i)
            baseTotalHeight += (i == quitButtonIndex) ? quitButtonHeight : buttonHeight;

        float y = ((float)GetScreenHeight() - baseTotalHeight) * 0.5f;
        for (size_t i = 0; i < probeRects.size(); ++i)
        {
            const float w = (i == quitButtonIndex) ? quitButtonWidth : buttonWidth;
            const float h = (i == quitButtonIndex) ? quitButtonHeight : buttonHeight;
            const float x = (i == quitButtonIndex) ? (buttonX + (buttonWidth - quitButtonWidth) * 0.5f) : buttonX;
            probeRects[i] = { x, y, w, h };
            y += h + buttonSpacing;
        }
    }

    int hoveredButton = -1;
    for (size_t i = 0; i < probeRects.size(); ++i)
    {
        if (CheckCollisionPointRec(mouse, probeRects[i]))
        {
            hoveredButton = (int)i;
            break;
        }
    }

    float totalStackHeight = buttonSpacing * ((float)probeRects.size() - 1.0f);
    for (size_t i = 0; i < probeRects.size(); ++i)
    {
        if (i == quitButtonIndex)
            totalStackHeight += quitButtonHeight;
        else
            totalStackHeight += ((int)i == hoveredButton) ? buttonHeightHovered : buttonHeight;
    }
    const float startY = ((float)GetScreenHeight() - totalStackHeight) * 0.5f;

    std::array<Rectangle, 5> buttonRects{};
    {
        float y = startY;
        for (size_t i = 0; i < buttonRects.size(); ++i)
        {
            const float h = (i == quitButtonIndex)
                ? quitButtonHeight
                : (((int)i == hoveredButton) ? buttonHeightHovered : buttonHeight);
            const float w = (i == quitButtonIndex) ? quitButtonWidth : buttonWidth;
            const float x = (i == quitButtonIndex) ? (buttonX + (buttonWidth - quitButtonWidth) * 0.5f) : buttonX;
            buttonRects[i] = { x, y, w, h };
            y += h + buttonSpacing;
        }
    }

    if (hoveredButton == 0)
    {
        PreloadAngryPreviewFrames();
        const Rectangle panel = { 26.0f, (float)GetScreenHeight() * 0.5f - 118.0f, 320.0f, 236.0f };
        DrawPreviewPanel(panel, angryPreviewFrames, "Angry Balls", "Add frames or mp4: assets/previews/angryballs");
    }
    else if (hoveredButton == 1)
    {
        PreloadDinoPreviewFrames();
        const Rectangle panel = { 26.0f, (float)GetScreenHeight() * 0.5f - 118.0f, 320.0f, 236.0f };
        DrawPreviewPanel(panel, dinoPreviewFrames, "Dino Jump", "Add frames or mp4: assets/previews/dinojump");
    }
    else if (hoveredButton == 2)
    {
        PreloadSpacePreviewFrames();
        const Rectangle panel = { 26.0f, (float)GetScreenHeight() * 0.5f - 118.0f, 320.0f, 236.0f };
        DrawPreviewPanel(panel, spacePreviewFrames, "Space Invaders", "Add frames or mp4: assets/previews/space_invaders");
    }
    else if (hoveredButton == 3)
    {
        PreloadWaterPreviewFrames();
        const Rectangle panel = { 26.0f, (float)GetScreenHeight() * 0.5f - 118.0f, 320.0f, 236.0f };
        DrawPreviewPanel(panel, waterPreviewFrames, "Where Is My Water", "Add frames or mp4: assets/previews/WhereIsMyWater");
    }

    for (size_t i = 0; i < buttonRects.size(); ++i)
    {
        if (!sceneManager)
            continue;

        bool buttonClicked = false;
        if (i == quitButtonIndex)
        {
            const int prevBorderNormal = GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL);
            const int prevBaseNormal = GuiGetStyle(BUTTON, BASE_COLOR_NORMAL);
            const int prevTextNormal = GuiGetStyle(BUTTON, TEXT_COLOR_NORMAL);
            const int prevBorderFocused = GuiGetStyle(BUTTON, BORDER_COLOR_FOCUSED);
            const int prevBaseFocused = GuiGetStyle(BUTTON, BASE_COLOR_FOCUSED);
            const int prevTextFocused = GuiGetStyle(BUTTON, TEXT_COLOR_FOCUSED);
            const int prevBorderPressed = GuiGetStyle(BUTTON, BORDER_COLOR_PRESSED);
            const int prevBasePressed = GuiGetStyle(BUTTON, BASE_COLOR_PRESSED);
            const int prevTextPressed = GuiGetStyle(BUTTON, TEXT_COLOR_PRESSED);

            GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, ColorToInt(Color{ 128, 30, 30, 255 }));
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, ColorToInt(Color{ 185, 44, 44, 255 }));
            GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, ColorToInt(RAYWHITE));
            GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, ColorToInt(Color{ 150, 48, 48, 255 }));
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, ColorToInt(Color{ 210, 60, 60, 255 }));
            GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, ColorToInt(RAYWHITE));
            GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, ColorToInt(Color{ 95, 22, 22, 255 }));
            GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, ColorToInt(Color{ 145, 33, 33, 255 }));
            GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, ColorToInt(RAYWHITE));

            buttonClicked = GuiButton(buttonRects[i], labels[i]);

            GuiSetStyle(BUTTON, BORDER_COLOR_NORMAL, prevBorderNormal);
            GuiSetStyle(BUTTON, BASE_COLOR_NORMAL, prevBaseNormal);
            GuiSetStyle(BUTTON, TEXT_COLOR_NORMAL, prevTextNormal);
            GuiSetStyle(BUTTON, BORDER_COLOR_FOCUSED, prevBorderFocused);
            GuiSetStyle(BUTTON, BASE_COLOR_FOCUSED, prevBaseFocused);
            GuiSetStyle(BUTTON, TEXT_COLOR_FOCUSED, prevTextFocused);
            GuiSetStyle(BUTTON, BORDER_COLOR_PRESSED, prevBorderPressed);
            GuiSetStyle(BUTTON, BASE_COLOR_PRESSED, prevBasePressed);
            GuiSetStyle(BUTTON, TEXT_COLOR_PRESSED, prevTextPressed);
        }
        else
        {
            buttonClicked = GuiButton(buttonRects[i], labels[i]);
        }

        if (!buttonClicked)
            continue;

        if (i < sceneIndexes.size())
            sceneManager->LoadScene(sceneIndexes[i]);
        else
            sceneManager->RequestQuit();
    }

    // Draw objects
    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj) continue;
        obj->Draw();
    }

    if (uiFont)
        DrawTextEx(uiFont->value, "KitsuneEngine", {20.0f, 20.0f}, 50.0f, 1.0f, BLACK);
    else
        DrawText("KitsuneEngine", 20, 20, 50, RED);

    // Debug overlay
    if (!Debug::GetDebug()) return;

    for (size_t i = 0; i < objects.Size(); i++)
    {
        Object* obj = objects.Get(i);
        if (!obj || !obj->collider) continue;

        obj->collider->DebugDraw();
    }
}
