#ifndef MOVABLE_OBJECTS_IMPROVEDBOT_H_
#define MOVABLE_OBJECTS_IMPROVEDBOT_H_

#include <memory>
#include <utility>
#include "bot.h"

class ImprovedBot : public Bot {
 public:
  ImprovedBot(std::shared_ptr<const Map> map, int init_cell_x, int init_cell_y,
              TankParameters tank_parameters, BotParameters bot_parameters,
              Direction direction);
  ~ImprovedBot() override = default;

 protected:
  bool CheckDirection(int tank, int bot, int direction) override;
  bool IsRotationStartNeeded(const std::shared_ptr<const Tank>& tank) override;
  bool IsShotNeeded(const std::shared_ptr<const Tank>&) override;

 private:
  bool ChangeDirection();
};

#endif  // MOVABLE_OBJECTS_IMPROVEDBOT_H_
