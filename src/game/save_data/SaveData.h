#pragma once

#include <array>
#include <filesystem>

class SaveData
{
public:
    static SaveData& Instance();

    int GetDinoBestScore();
    int GetAngryBestScore(int levelNumber);
    int GetAngryUnlockedLevelCount();
    int GetAngryCurrentScene();

    void SetDinoBestScore(int score);
    void SetAngryBestScore(int levelNumber, int score);
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
    std::filesystem::path legacyScoreFile;
    std::filesystem::path legacyScoresFile;

    static constexpr int kAngryLevelCount = 4;

    int dinoBestScore = 0;
    std::array<int, kAngryLevelCount> angryBestScoresByLevel = { 0, 0, 0, 0 };
    int angryUnlockedLevelCount = 1;
    int angryCurrentScene = 0;
    bool loaded = false;
};
