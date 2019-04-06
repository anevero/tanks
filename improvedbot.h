#ifndef IMPROVEDBOT_H
#define IMPROVEDBOT_H

#include "bot.h"
#include <memory>
#include <utility>

class ImprovedBot : public Bot {
 public:
  ImprovedBot(std::shared_ptr<Map>& map, BotQualities qualities,
              Direction direction);
  virtual void LoadImage() override;

 protected:
  bool CheckDirection(int& tank, int& bot, int direction) override;
  bool IsRotationStartNeeded(std::shared_ptr<Tank> tank) override;
  bool IsShotNeeded(std::shared_ptr<Map>, std::shared_ptr<Tank>) override;

 private:
  bool ChangeDirection();
};

#endif  // IMPROVEDBOT_H
