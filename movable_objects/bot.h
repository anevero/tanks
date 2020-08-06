#ifndef MOVABLE_OBJECTS_BOT_H_
#define MOVABLE_OBJECTS_BOT_H_

#include <list>
#include <memory>
#include <utility>
#include <vector>

#include "../game_core/map.h"
#include "tank.h"

struct BotParameters {
  int moving_length;
  int number_of_turns;
  int side_rotation_frequency;
};

class Bot : public Tank {
 public:
  Bot(std::shared_ptr<const Map> map, Coordinates initial_cell,
      TankParameters tank_parameters, BotParameters bot_parameters,
      Direction direction);
  ~Bot() override = default;

  virtual bool IsTurnNeeded() const;

  virtual bool IsRotationStartNeeded(const std::shared_ptr<const Tank>&);
  virtual bool IsMovingStartNeeded(
      const std::list<std::shared_ptr<Tank>>&,
      const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>&);

  virtual bool IsShotNeeded(const std::shared_ptr<const Tank>&);

 protected:
  int number_of_cells_to_move_ = 0;
  int current_number_of_turns_ = 0;

  const int moving_length_;
  const int number_of_turns_;
  const int side_rotation_frequency_;

  virtual bool CheckDirection(int tank_coordinate, int bot_coordinate,
                              int direction);

  bool IsWallBetweenTankAndBotHorizontally(
      Coordinates tank_cell, Coordinates bot_cell) const;
  bool IsWallBetweenTankAndBotVertically(
      Coordinates tank_cell, Coordinates bot_cell) const;
};

#endif  // MOVABLE_OBJECTS_BOT_H_
