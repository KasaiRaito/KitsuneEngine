#include "SaveData.h"

#include "nlohmann/json.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>

namespace
{
    static constexpr int kAngryLevelCount = 4;
    static constexpr int kWaterLevelCount = 4;

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

    static int ToAngryLevelIndex(int levelNumber)
    {
        return std::clamp(levelNumber, 1, kAngryLevelCount) - 1;
    }

    static int ToWaterLevelIndex(int levelNumber)
    {
        return std::clamp(levelNumber, 1, kWaterLevelCount) - 1;
    }
}

SaveData& SaveData::Instance()
{
    static SaveData saveData;
    return saveData;
}

SaveData::SaveData()
    : saveDirectory(ResolveSaveDirectory())
    , saveFile(saveDirectory / "save_data.json")
    , legacyScoreFile(saveDirectory / "score.txt")
    , legacyScoresFile(saveDirectory / "scores.txt")
{
}

int SaveData::GetDinoBestScore()
{
    EnsureLoaded();
    return dinoBestScore;
}

int SaveData::GetAngryBestScore(int levelNumber)
{
    EnsureLoaded();
    return angryBestScoresByLevel[ToAngryLevelIndex(levelNumber)];
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

int SaveData::GetWaterBestScore(int levelNumber)
{
    EnsureLoaded();
    return waterBestScoresByLevel[ToWaterLevelIndex(levelNumber)];
}

int SaveData::GetWaterUnlockedLevelCount()
{
    EnsureLoaded();
    return waterUnlockedLevelCount;
}

int SaveData::GetWaterCurrentLevel()
{
    EnsureLoaded();
    return waterCurrentLevel;
}

void SaveData::SetDinoBestScore(int score)
{
    EnsureLoaded();

    if (score <= dinoBestScore)
        return;

    dinoBestScore = score;
    SaveToDisk();
}

void SaveData::SetAngryBestScore(int levelNumber, int score)
{
    EnsureLoaded();

    if (score < 0)
        return;

    const int levelIndex = ToAngryLevelIndex(levelNumber);
    if (score <= angryBestScoresByLevel[levelIndex])
        return;

    angryBestScoresByLevel[levelIndex] = score;
    SaveToDisk();
}

void SaveData::SetAngryUnlockedLevelCount(int unlockedLevelCount)
{
    EnsureLoaded();

    const int clamped = std::clamp(unlockedLevelCount, 1, kAngryLevelCount);
    if (clamped <= angryUnlockedLevelCount)
        return;

    angryUnlockedLevelCount = clamped;
    SaveToDisk();
}

void SaveData::SetAngryCurrentScene(int sceneId)
{
    EnsureLoaded();

    const int clamped = std::clamp(sceneId, 0, kAngryLevelCount);
    if (clamped == angryCurrentScene)
        return;

    angryCurrentScene = clamped;
    SaveToDisk();
}

void SaveData::SetWaterBestScore(int levelNumber, int score)
{
    EnsureLoaded();

    if (score < 0)
        return;

    const int levelIndex = ToWaterLevelIndex(levelNumber);
    if (score <= waterBestScoresByLevel[levelIndex])
        return;

    waterBestScoresByLevel[levelIndex] = score;
    SaveToDisk();
}

void SaveData::SetWaterUnlockedLevelCount(int unlockedLevelCount)
{
    EnsureLoaded();

    const int clamped = std::clamp(unlockedLevelCount, 1, kWaterLevelCount);
    if (clamped <= waterUnlockedLevelCount)
        return;

    waterUnlockedLevelCount = clamped;
    SaveToDisk();
}

void SaveData::SetWaterCurrentLevel(int levelNumber)
{
    EnsureLoaded();

    const int clamped = std::clamp(levelNumber, 1, kWaterLevelCount);
    if (clamped == waterCurrentLevel)
        return;

    waterCurrentLevel = clamped;
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
                if (json.contains("angry_best_by_level") && json["angry_best_by_level"].is_array())
                {
                    const auto& levelScores = json["angry_best_by_level"];
                    for (int i = 0; i < kAngryLevelCount; ++i)
                    {
                        if (i >= (int)levelScores.size() || !levelScores[(size_t)i].is_number_integer())
                            continue;

                        angryBestScoresByLevel[(size_t)i] = std::max(0, levelScores[(size_t)i].get<int>());
                    }
                }
                else
                {
                    // Backward compatibility with single angry_best key from older save versions.
                    angryBestScoresByLevel[0] = std::max(0, json.value("angry_best", angryBestScoresByLevel[0]));
                }
                angryUnlockedLevelCount = std::clamp(json.value("angry_unlocked_levels", angryUnlockedLevelCount), 1, kAngryLevelCount);
                angryCurrentScene = std::clamp(json.value("angry_current_scene", angryCurrentScene), 0, kAngryLevelCount);

                if (json.contains("water_best_by_level") && json["water_best_by_level"].is_array())
                {
                    const auto& levelScores = json["water_best_by_level"];
                    for (int i = 0; i < kWaterLevelCount; ++i)
                    {
                        if (i >= (int)levelScores.size() || !levelScores[(size_t)i].is_number_integer())
                            continue;

                        waterBestScoresByLevel[(size_t)i] = std::max(0, levelScores[(size_t)i].get<int>());
                    }
                }
                else
                {
                    // Backward compatibility with single water_best key from older save versions.
                    waterBestScoresByLevel[0] = std::max(0, json.value("water_best", waterBestScoresByLevel[0]));
                }
                waterUnlockedLevelCount = std::clamp(json.value("water_unlocked_levels", waterUnlockedLevelCount), 1, kWaterLevelCount);
                waterCurrentLevel = std::clamp(json.value("water_current_level", waterCurrentLevel), 1, kWaterLevelCount);

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
                angryBestScoresByLevel[0] = std::max(0, ParseIntOrDefault(value, angryBestScoresByLevel[0]));
            }
            else if (key.rfind("angry_best_level", 0) == 0)
            {
                const std::string suffix = key.substr(std::string("angry_best_level").size());
                const int levelNumber = ParseIntOrDefault(suffix, 1);
                const int levelIndex = ToAngryLevelIndex(levelNumber);
                angryBestScoresByLevel[(size_t)levelIndex] = std::max(
                    0,
                    ParseIntOrDefault(value, angryBestScoresByLevel[(size_t)levelIndex])
                );
            }
            else if (key == "angry_unlocked_levels")
            {
                angryUnlockedLevelCount = std::clamp(ParseIntOrDefault(value, angryUnlockedLevelCount), 1, kAngryLevelCount);
            }
            else if (key == "angry_current_scene")
            {
                angryCurrentScene = std::clamp(ParseIntOrDefault(value, angryCurrentScene), 0, kAngryLevelCount);
            }
            else if (key == "water_best")
            {
                waterBestScoresByLevel[0] = std::max(0, ParseIntOrDefault(value, waterBestScoresByLevel[0]));
            }
            else if (key.rfind("water_best_level", 0) == 0)
            {
                const std::string suffix = key.substr(std::string("water_best_level").size());
                const int levelNumber = ParseIntOrDefault(suffix, 1);
                const int levelIndex = ToWaterLevelIndex(levelNumber);
                waterBestScoresByLevel[(size_t)levelIndex] = std::max(
                    0,
                    ParseIntOrDefault(value, waterBestScoresByLevel[(size_t)levelIndex])
                );
            }
            else if (key == "water_unlocked_levels")
            {
                waterUnlockedLevelCount = std::clamp(ParseIntOrDefault(value, waterUnlockedLevelCount), 1, kWaterLevelCount);
            }
            else if (key == "water_current_level")
            {
                waterCurrentLevel = std::clamp(ParseIntOrDefault(value, waterCurrentLevel), 1, kWaterLevelCount);
            }
        }

        return true;
    };

    if (!loadLegacyKeyValue(legacyScoreFile) && !loadLegacyKeyValue(legacyScoresFile))
        return false;

    angryUnlockedLevelCount = std::clamp(angryUnlockedLevelCount, 1, kAngryLevelCount);
    angryCurrentScene = std::clamp(angryCurrentScene, 0, kAngryLevelCount);
    waterUnlockedLevelCount = std::clamp(waterUnlockedLevelCount, 1, kWaterLevelCount);
    waterCurrentLevel = std::clamp(waterCurrentLevel, 1, kWaterLevelCount);

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
    json["angry_best_by_level"] = angryBestScoresByLevel;
    json["angry_best"] = *std::max_element(angryBestScoresByLevel.begin(), angryBestScoresByLevel.end());
    json["angry_unlocked_levels"] = angryUnlockedLevelCount;
    json["angry_current_scene"] = angryCurrentScene;
    json["water_best_by_level"] = waterBestScoresByLevel;
    json["water_best"] = *std::max_element(waterBestScoresByLevel.begin(), waterBestScoresByLevel.end());
    json["water_unlocked_levels"] = waterUnlockedLevelCount;
    json["water_current_level"] = waterCurrentLevel;

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
