#ifndef MOVABLE_OBJECTS_BOT_H_
#define MOVABLE_OBJECTS_BOT_H_

#include <QPainter>
#include <chrono>
#include <cmath>
#include <list>
#include <memory>
#include <random>
#include <utility>
#include <vector>
#include "../game_core/map.h"
#include "tank.h"

struct BotParameters {
  int moving_length;
  int amount_of_turns;
  int side_rotation_frequency;
};

class Bot : public Tank {
 public:
  Bot(std::shared_ptr<Map> map, int init_cell_x, int init_cell_y,
      TankParameters tank_parameters, BotParameters bot_parameters,
      Direction direction);
  ~Bot() override = default;

  virtual bool IsTurnNeeded() const;
  virtual bool IsRotationStartNeeded(const std::shared_ptr<Tank>&);
  virtual bool IsMovingStartNeeded(
      const std::list<std::shared_ptr<Movable>>&,
      const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>&);
  virtual bool IsShotNeeded(const std::shared_ptr<Map>&,
                            const std::shared_ptr<Tank>&);

 protected:
  int number_of_cells_to_move_ = 0;
  int number_of_turns_ = 0;
  const int moving_length_;
  const int amount_of_turns_;
  const int side_rotation_frequency_;

  virtual bool CheckDirection(int tank, int bot, int direction);
  static bool IsWallBetweenObjectsX(
      const std::shared_ptr<Map>& map,
      int tank_x, int tank_y, int bot_x, int bot_y);
  static bool IsWallBetweenObjectsY(
      const std::shared_ptr<Map>& map,
      int tank_x, int tank_y, int bot_x, int bot_y);

  static std::mt19937 random_generator_;
};

#endif  // MOVABLE_OBJECTS_BOT_H_
