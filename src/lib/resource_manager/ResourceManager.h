#pragma once

#include "raylib.h"

#include <memory>
#include <string>
#include <unordered_map>

// ResourceManager centralizes loading/caching of raylib assets.
// Each cache entry is a shared_ptr to a resource struct, so callers can safely
// keep references while the manager owns the cache and unload behavior.
class ResourceManager {
public:
    struct TextureResource {
        std::string path;
        Texture2D value{};
    };

    struct FontResource {
        std::string path;
        Font value{};
    };

    struct SoundResource {
        std::string path;
        Sound value{};
    };

    struct MusicResource {
        std::string path;
        Music value{};
    };

public:
    // Singleton-style global access point.
    static ResourceManager& Instance();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager(ResourceManager&&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;

    // Lazy-load helpers:
    // 1) If path is cached, return existing shared_ptr.
    // 2) Otherwise load from disk, cache it in hash map, and return shared_ptr.
    std::shared_ptr<TextureResource> GetOrLoadTexture(const std::string& path);
    std::shared_ptr<FontResource> GetOrLoadFont(const std::string& path);
    std::shared_ptr<SoundResource> GetOrLoadSound(const std::string& path);
    std::shared_ptr<MusicResource> GetOrLoadMusic(const std::string& path);

    // Cache state query helpers.
    bool HasTexture(const std::string& path) const;
    bool HasFont(const std::string& path) const;
    bool HasSound(const std::string& path) const;
    bool HasMusic(const std::string& path) const;

    // Unloads every cached resource and clears all maps.
    void Clear();

private:
    ResourceManager();
    ~ResourceManager();

    // Audio resources need a ready audio device before load.
    void EnsureAudioReady();

private:
    std::unordered_map<std::string, std::shared_ptr<TextureResource>> textures;
    std::unordered_map<std::string, std::shared_ptr<FontResource>> fonts;
    std::unordered_map<std::string, std::shared_ptr<SoundResource>> sounds;
    std::unordered_map<std::string, std::shared_ptr<MusicResource>> musics;

    // Tracks whether this manager initialized the audio device itself.
    bool ownsAudioDevice = false;
};
