#include "improvedbot.h"

ImprovedBot::ImprovedBot(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y, int speed,
         int rate_of_fire, Direction direction, int moving_length, int amount_of_turns)
    : Bot(map, init_cell_x, init_cell_y, speed, rate_of_fire, direction,
          moving_length, amount_of_turns) {};

//bool ImprovedBot::IsShotNeeded(std::shared_ptr<Map>, std::shared_ptr<Tank>) {

//}
