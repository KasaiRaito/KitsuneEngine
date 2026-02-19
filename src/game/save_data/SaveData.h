#pragma once

#include <filesystem>

class SaveData
{
public:
    static SaveData& Instance();

    int GetDinoBestScore();
    int GetAngryBestScore();

    void SetDinoBestScore(int score);
    void SetAngryBestScore(int score);

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

    int dinoBestScore = 0;
    int angryBestScore = 0;
    bool loaded = false;
};
