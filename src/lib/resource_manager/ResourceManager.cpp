#include "ResourceManager.h"

#include "raylib.h"
#include <cctype>

static bool EndsWithIgnoreCase(const std::string& value, const std::string& suffix)
{
    if (suffix.size() > value.size())
        return false;

    const size_t offset = value.size() - suffix.size();
    for (size_t i = 0; i < suffix.size(); ++i)
    {
        const char a = (char)std::tolower((unsigned char)value[offset + i]);
        const char b = (char)std::tolower((unsigned char)suffix[i]);
        if (a != b)
            return false;
    }
    return true;
}

ResourceManager& ResourceManager::Instance()
{
    static ResourceManager manager;
    return manager;
}

ResourceManager::ResourceManager() = default;

ResourceManager::~ResourceManager()
{
    // Release all loaded assets when the manager is destroyed.
    Clear();
}

std::shared_ptr<ResourceManager::TextureResource> ResourceManager::GetOrLoadTexture(const std::string& path)
{
    // Cache hit: return already loaded texture.
    if (auto it = textures.find(path); it != textures.end())
        return it->second;

    Texture2D texture = LoadTexture(path.c_str());
    if (texture.id == 0)
        return nullptr;

    auto resource = std::make_shared<TextureResource>();
    resource->path = path;
    resource->value = texture;

    textures[path] = resource;
    return resource;
}

std::shared_ptr<ResourceManager::FontResource> ResourceManager::GetOrLoadFont(const std::string& path)
{
    // Cache hit: return already loaded font.
    if (auto it = fonts.find(path); it != fonts.end())
        return it->second;

    Font font = LoadFont(path.c_str());
    if (font.texture.id == 0)
        return nullptr;

    auto resource = std::make_shared<FontResource>();
    resource->path = path;
    resource->value = font;

    fonts[path] = resource;
    return resource;
}

std::shared_ptr<ResourceManager::SoundResource> ResourceManager::GetOrLoadSound(const std::string& path)
{
    EnsureAudioReady();

    // Cache hit: return already loaded sound.
    if (auto it = sounds.find(path); it != sounds.end())
        return it->second;

    // First attempt: direct decoder path.
    Sound sound = LoadSound(path.c_str());

    // Fallback path for files that fail direct LoadSound (commonly some mp3 encodings):
    // decode into Wave first, then convert Wave -> Sound.
    if (sound.frameCount == 0)
    {
        Wave wave = LoadWave(path.c_str());
        if (wave.frameCount > 0)
        {
            sound = LoadSoundFromWave(wave);
            UnloadWave(wave);
        }
    }

    if (sound.frameCount == 0)
    {
        // Clear warning so caller knows file exists but decoding was unsupported.
        if (EndsWithIgnoreCase(path, ".mp3"))
            TraceLog(LOG_WARNING, "ResourceManager: mp3 decode failed for '%s'. Prefer wav/ogg for SFX.", path.c_str());
        return nullptr;
    }

    auto resource = std::make_shared<SoundResource>();
    resource->path = path;
    resource->value = sound;

    sounds[path] = resource;
    return resource;
}

std::shared_ptr<ResourceManager::MusicResource> ResourceManager::GetOrLoadMusic(const std::string& path)
{
    EnsureAudioReady();

    // Cache hit: return already loaded music stream.
    if (auto it = musics.find(path); it != musics.end())
        return it->second;

    Music music = LoadMusicStream(path.c_str());
    if (music.frameCount == 0)
        return nullptr;

    auto resource = std::make_shared<MusicResource>();
    resource->path = path;
    resource->value = music;

    musics[path] = resource;
    return resource;
}

bool ResourceManager::HasTexture(const std::string& path) const
{
    return textures.find(path) != textures.end();
}

bool ResourceManager::HasFont(const std::string& path) const
{
    return fonts.find(path) != fonts.end();
}

bool ResourceManager::HasSound(const std::string& path) const
{
    return sounds.find(path) != sounds.end();
}

bool ResourceManager::HasMusic(const std::string& path) const
{
    return musics.find(path) != musics.end();
}

void ResourceManager::Clear()
{
    // Unload assets first, then close audio device if we opened it.
    for (auto& [key, resource] : musics)
    {
        (void)key;
        if (resource)
            UnloadMusicStream(resource->value);
    }
    musics.clear();

    for (auto& [key, resource] : sounds)
    {
        (void)key;
        if (resource)
            UnloadSound(resource->value);
    }
    sounds.clear();

    for (auto& [key, resource] : fonts)
    {
        (void)key;
        if (resource)
            UnloadFont(resource->value);
    }
    fonts.clear();

    for (auto& [key, resource] : textures)
    {
        (void)key;
        if (resource)
            UnloadTexture(resource->value);
    }
    textures.clear();

    if (ownsAudioDevice && IsAudioDeviceReady())
    {
        CloseAudioDevice();
        ownsAudioDevice = false;
    }
}

void ResourceManager::EnsureAudioReady()
{
    if (IsAudioDeviceReady())
        return;

    InitAudioDevice();
    ownsAudioDevice = true;
}
