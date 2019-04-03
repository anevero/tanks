#include "bot.h"

Bot::Bot(std::shared_ptr<Map>& map, BotQualities qualities, Direction direction)
    : Tank(map, qualities.init_cell_x, qualities.init_cell_y, qualities.tank,
           direction),
      moving_length_(qualities.moving_length),
      amount_of_turns_(qualities.amount_of_turns),
      side_rotation_frequency_(qualities.side_rotation_frequency) {
  LoadImage();
};

void Bot::LoadImage() {
  image_.load(":/textures/bot.png");
  scaled_image_ = image_;
}

bool Bot::IsTurnNeeded() const { return time_to_finish_rotation_ > 0; }

bool Bot::IsRotationStartNeeded(std::shared_ptr<Tank>) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (number_of_turns_ > 0) {
      number_of_turns_--;
      return number_of_turns_ > 0 ? true : false;
    }
    if (number_of_cells_to_move_ == 0) {
      if (qrand() % side_rotation_frequency_ == 0) {
        TurnRotationReverseOn();
      } else {
        TurnRotationReverseOff();
      }
      number_of_turns_ = amount_of_turns_;
      return true;
    }
  }
  return false;
}

bool Bot::IsMoveNeeded() const { return time_to_finish_movement_ > 0; }

bool Bot::IsMovingStartNeeded() {
  if (time_to_finish_movement_ <= 0 && time_to_finish_rotation_ <= 0) {
    if (number_of_cells_to_move_ == 0) {
      if (number_of_turns_ == 0) {
        number_of_cells_to_move_ = moving_length_;
      } else {
        return false;
      }
    } else {
      number_of_cells_to_move_--;
    }
    return number_of_cells_to_move_ > 0 ? true : false;
  }
  return false;
}

bool Bot::IsShotNeeded(std::shared_ptr<Map> map, std::shared_ptr<Tank> tank) {
  if (time_to_finish_rotation_ == 0 && time_to_finish_movement_ == 0) {
    int direction = GetIntDirection();
    int tank_x = tank->GetCellX();
    int tank_y = tank->GetCellY();
    int bot_x = GetCellX();
    int bot_y = GetCellY();

    if (direction == 0 || direction == 2) {
      if (tank_x == bot_x) {
        int walls_count = map->GetWallsPrecalc(bot_x, bot_y);
        walls_count += map->GetWallsPrecalc(tank_x - 1, tank_y - 1);
        walls_count -= map->GetWallsPrecalc(tank_x, tank_y - 1);
        walls_count -= map->GetWallsPrecalc(bot_x - 1, bot_y);
        if (walls_count != 0) {
          return false;
        }

        if (CheckDirection(tank_y, bot_y, direction)) {
          return true;
        }
      }
    }
    if (direction == 1 || direction == 3) {
      if (tank_y == bot_y) {
        int walls_count = map->GetWallsPrecalc(bot_x, bot_y);
        walls_count += map->GetWallsPrecalc(tank_x - 1, tank_y - 1);
        walls_count -= map->GetWallsPrecalc(tank_x - 1, tank_y);
        walls_count -= map->GetWallsPrecalc(bot_x, bot_y - 1);
        if (walls_count != 0) {
          return false;
        }

        if (CheckDirection(tank_x, bot_x, direction)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Bot::CheckDirection(int& tank, int& bot, int direction) {
  if (tank > bot) {
    if (direction == 0 || direction == 3) {
      return false;
    }
  } else if (direction == 2 || direction == 1) {
    return false;
  }
  return true;
}
