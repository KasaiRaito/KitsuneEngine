#include "SaveData.h"

#include "nlohmann/json.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>

namespace
{
    static std::string Trim(const std::string& value)
    {
        size_t begin = 0;
        while (begin < value.size() && std::isspace((unsigned char)value[begin]))
            ++begin;

        size_t end = value.size();
        while (end > begin && std::isspace((unsigned char)value[end - 1]))
            --end;

        return value.substr(begin, end - begin);
    }

    static int ParseIntOrDefault(const std::string& value, int fallback)
    {
        try
        {
            size_t used = 0;
            const int parsed = std::stoi(value, &used);
            if (used != value.size())
                return fallback;

            return parsed;
        }
        catch (...)
        {
            return fallback;
        }
    }
}

SaveData& SaveData::Instance()
{
    static SaveData saveData;
    return saveData;
}

SaveData::SaveData()
    : saveDirectory(ResolveSaveDirectory())
    , saveFile(saveDirectory / "score.json")
    , legacyScoreFile(saveDirectory / "score.txt")
    , legacyScoresFile(saveDirectory / "scores.txt")
{
}

int SaveData::GetDinoBestScore()
{
    EnsureLoaded();
    return dinoBestScore;
}

int SaveData::GetAngryBestScore()
{
    EnsureLoaded();
    return angryBestScore;
}

int SaveData::GetAngryUnlockedLevelCount()
{
    EnsureLoaded();
    return angryUnlockedLevelCount;
}

int SaveData::GetAngryCurrentScene()
{
    EnsureLoaded();
    return angryCurrentScene;
}

void SaveData::SetDinoBestScore(int score)
{
    EnsureLoaded();

    if (score <= dinoBestScore)
        return;

    dinoBestScore = score;
    SaveToDisk();
}

void SaveData::SetAngryBestScore(int score)
{
    EnsureLoaded();

    if (score <= angryBestScore)
        return;

    angryBestScore = score;
    SaveToDisk();
}

void SaveData::SetAngryUnlockedLevelCount(int unlockedLevelCount)
{
    EnsureLoaded();

    const int clamped = std::clamp(unlockedLevelCount, 1, 2);
    if (clamped <= angryUnlockedLevelCount)
        return;

    angryUnlockedLevelCount = clamped;
    SaveToDisk();
}

void SaveData::SetAngryCurrentScene(int sceneId)
{
    EnsureLoaded();

    const int clamped = std::clamp(sceneId, 0, 2);
    if (clamped == angryCurrentScene)
        return;

    angryCurrentScene = clamped;
    SaveToDisk();
}

const std::filesystem::path& SaveData::GetSaveDirectoryPath()
{
    EnsureLoaded();
    return saveDirectory;
}

const std::filesystem::path& SaveData::GetSaveFilePath()
{
    EnsureLoaded();
    return saveFile;
}

void SaveData::EnsureLoaded()
{
    if (loaded)
        return;

    loaded = true;

    std::error_code ec;
    std::filesystem::create_directories(saveDirectory, ec);

    if (!LoadFromDisk())
    {
        SaveToDisk();
        return;
    }

    std::error_code ec2;
    if (!std::filesystem::exists(saveFile, ec2))
        SaveToDisk();
}

bool SaveData::LoadFromDisk()
{
    // Preferred format: JSON score file.
    {
        std::ifstream input(saveFile);
        if (input.is_open())
        {
            try
            {
                nlohmann::json json;
                input >> json;

                dinoBestScore = std::max(0, json.value("dino_best", dinoBestScore));
                angryBestScore = std::max(0, json.value("angry_best", angryBestScore));
                angryUnlockedLevelCount = std::clamp(json.value("angry_unlocked_levels", angryUnlockedLevelCount), 1, 2);
                angryCurrentScene = std::clamp(json.value("angry_current_scene", angryCurrentScene), 0, 2);

                return true;
            }
            catch (...)
            {
                // Fall through to legacy key-value readers.
            }
        }
    }

    auto loadLegacyKeyValue = [this](const std::filesystem::path& path) -> bool
    {
        std::ifstream input(path);
        if (!input.is_open())
            return false;

        std::string line;
        while (std::getline(input, line))
        {
            const std::string trimmed = Trim(line);
            if (trimmed.empty() || trimmed[0] == '#')
                continue;

            const size_t separator = trimmed.find('=');
            if (separator == std::string::npos)
                continue;

            const std::string key = Trim(trimmed.substr(0, separator));
            const std::string value = Trim(trimmed.substr(separator + 1));

            if (key == "dino_best")
            {
                dinoBestScore = std::max(0, ParseIntOrDefault(value, dinoBestScore));
            }
            else if (key == "angry_best")
            {
                angryBestScore = std::max(0, ParseIntOrDefault(value, angryBestScore));
            }
            else if (key == "angry_unlocked_levels")
            {
                angryUnlockedLevelCount = std::clamp(ParseIntOrDefault(value, angryUnlockedLevelCount), 1, 2);
            }
            else if (key == "angry_current_scene")
            {
                angryCurrentScene = std::clamp(ParseIntOrDefault(value, angryCurrentScene), 0, 2);
            }
        }

        return true;
    };

    if (!loadLegacyKeyValue(legacyScoreFile) && !loadLegacyKeyValue(legacyScoresFile))
        return false;

    angryUnlockedLevelCount = std::clamp(angryUnlockedLevelCount, 1, 2);
    angryCurrentScene = std::clamp(angryCurrentScene, 0, 2);

    return true;
}

bool SaveData::SaveToDisk() const
{
    std::error_code ec;
    std::filesystem::create_directories(saveDirectory, ec);

    std::ofstream output(saveFile, std::ios::trunc);
    if (!output.is_open())
        return false;

    nlohmann::json json;
    json["dino_best"] = dinoBestScore;
    json["angry_best"] = angryBestScore;
    json["angry_unlocked_levels"] = angryUnlockedLevelCount;
    json["angry_current_scene"] = angryCurrentScene;

    output << json.dump(4) << "\n";

    return true;
}

std::filesystem::path SaveData::ResolveSaveDirectory()
{
    const std::filesystem::path roots[] = {
        std::filesystem::path("."),
        std::filesystem::path(".."),
        std::filesystem::path("../.."),
        std::filesystem::path("../../..")
    };

    for (const std::filesystem::path& root : roots)
    {
        if (std::filesystem::exists(root / "src") && std::filesystem::exists(root / "CMakeLists.txt"))
            return std::filesystem::absolute(root / "save_data");
    }

    return std::filesystem::absolute("save_data");
}
