#ifndef IMPROVEDBOT_H
#define IMPROVEDBOT_H

#include "bot.h"
#include <memory>
#include <utility>

class ImprovedBot : public Bot {
 public:
  ImprovedBot(std::shared_ptr<Map>& map, BotQualities qualities);
  virtual void LoadImage() override;

 protected:
  bool CheckDirection(const int& tank, const int& bot, const int direction) override;
  bool IsRotationStartNeeded(const std::shared_ptr<Tank> tank) override;
  bool IsShotNeeded(const std::shared_ptr<Map>, const std::shared_ptr<Tank>) override;

 private:
  bool ChangeDirection();
};

#endif  // IMPROVEDBOT_H
