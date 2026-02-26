#pragma once

#include <filesystem>

class SaveData
{
public:
    static SaveData& Instance();

    int GetDinoBestScore();
    int GetAngryBestScore();
    int GetAngryUnlockedLevelCount();
    int GetAngryCurrentScene();

    void SetDinoBestScore(int score);
    void SetAngryBestScore(int score);
    void SetAngryUnlockedLevelCount(int unlockedLevelCount);
    void SetAngryCurrentScene(int sceneId);

    const std::filesystem::path& GetSaveDirectoryPath();
    const std::filesystem::path& GetSaveFilePath();

private:
    SaveData();

    void EnsureLoaded();
    bool LoadFromDisk();
    bool SaveToDisk() const;

    static std::filesystem::path ResolveSaveDirectory();

    std::filesystem::path saveDirectory;
    std::filesystem::path saveFile;
    std::filesystem::path legacySaveFile;

    int dinoBestScore = 0;
    int angryBestScore = 0;
    int angryUnlockedLevelCount = 1;
    int angryCurrentScene = 0;
    bool loaded = false;
};
