#ifndef GAME_CORE_CONSTANTS_H_
#define GAME_CORE_CONSTANTS_H_

#include <vector>
#include <QObject>
#include <QString>

namespace constants {

const int number_of_maps = 4;
const int number_of_tanks = 3;
const int number_of_difficulty_levels = 3;
std::vector<QString> difficulty_levels_names =
    {QObject::tr("Easy"), QObject::tr("Normal"), QObject::tr("Hard")};

}  // namespace constants

#endif  // GAME_CORE_CONSTANTS_H_
