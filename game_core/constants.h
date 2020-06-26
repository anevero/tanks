#ifndef GAME_CORE_CONSTANTS_H_
#define GAME_CORE_CONSTANTS_H_

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <QObject>
#include <QString>

namespace constants {

const QString kOrganizationName = "Tanks";
const QString kApplicationName = "Tanks";
const int kApplicationRestartCode = 438254263;

const int kMinutesPerRound = 10;

const int kMapsNumber = 4;
const int kTanksNumber = 3;
const int kDifficultyLevelsNumber = 3;

const QString kVirtualKeysEnabledKey = "virtual_keys_enabled";
const QString kMobileVirtualKeysStyleEnabledKey =
    "mobile_virtual_keys_style_enabled";
const QString kChargeLineEnabledKey = "charge_line_enabled";
const QString kMusicEnabledKey = "music_enabled";
const QString kCurrentFpsOptionKey = "current_fps_option";
const QString kLanguageKey = "language";

const std::vector<int> kFpsOptions = {40, 50, 60, 90, 120, 240};
const std::vector<QString> kLanguages = {"en_US", "ru_RU", "be_BY"};

}  // namespace constants

#endif  // GAME_CORE_CONSTANTS_H_
