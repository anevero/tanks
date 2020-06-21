#ifndef MOVABLE_OBJECTS_IMPROVEDBOT_H_
#define MOVABLE_OBJECTS_IMPROVEDBOT_H_

#include <memory>
#include <utility>
#include "bot.h"

class ImprovedBot : public Bot {
 public:
  ImprovedBot(const std::shared_ptr<Map>& map, const BotQualities& qualities);

 protected:
  bool CheckDirection(const int tank, const int bot, int direction) override;
  bool IsRotationStartNeeded(const std::shared_ptr<Tank>& tank) override;
  bool IsShotNeeded(const std::shared_ptr<Map>&,
                    const std::shared_ptr<Tank>&) override;

 private:
  bool ChangeDirection();
};

#endif  // MOVABLE_OBJECTS_IMPROVEDBOT_H_
