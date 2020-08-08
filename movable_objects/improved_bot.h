#ifndef MOVABLE_OBJECTS_IMPROVED_BOT_H_
#define MOVABLE_OBJECTS_IMPROVED_BOT_H_

#include <memory>
#include <utility>

#include "bot.h"

class ImprovedBot : public Bot {
 public:
  ImprovedBot(std::shared_ptr<const Map> map, const QString& texture_path,
              Coordinates initial_cell, TankParameters tank_parameters,
              BotParameters bot_parameters, Direction direction);
  ~ImprovedBot() override = default;

 protected:
  bool CheckDirection(
      int tank_coordinate, int bot_coordinate, int direction) override;
  bool IsRotationStartNeeded(const std::shared_ptr<const Tank>& tank) override;
  bool IsShotNeeded(const std::shared_ptr<const Tank>&) override;

 private:
  bool ChangeDirection();
  bool MaybeChangeDirection(int tank_coordinate,
                            int bot_coordinate,
                            int current_direction,
                            int first_direction,
                            int second_direction);
  void MaybeChangeRotationDirection(int direction, bool condition);
};

#endif  // MOVABLE_OBJECTS_IMPROVED_BOT_H_
