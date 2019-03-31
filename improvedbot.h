#ifndef IMPROVEDBOT_H
#define IMPROVEDBOT_H

#include "bot.h"

class ImprovedBot : public Bot {
 public:
  ImprovedBot(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y, int speed,
                 int rate_of_fire, Direction direction, int moving_length, int amount_of_turns);

  // bool IsShotNeeded(std::shared_ptr<Map>, std::shared_ptr<Tank>) override;
};

#endif // IMPROVEDBOT_H
