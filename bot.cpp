#include "bot.h"

Bot::Bot(const std::shared_ptr<Map>& map, const BotQualities& qualities)
    : Tank(map, qualities.init_cell_x, qualities.init_cell_y, qualities.tank),
      moving_length_(qualities.moving_length),
      amount_of_turns_(qualities.amount_of_turns),
      side_rotation_frequency_(qualities.side_rotation_frequency) {
  LoadImage();
}

void Bot::LoadImage() {
  image_.load(":/textures/bot.png");
  scaled_image_ = image_;
}

bool Bot::IsTurnNeeded() const { return time_to_finish_rotation_ > 0; }

bool Bot::IsRotationStartNeeded(const std::shared_ptr<Tank>&) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (number_of_turns_ > 0) {
      number_of_turns_--;
      return number_of_turns_ > 0;
    }
    if (number_of_cells_to_move_ == 0) {
      if (qrand() % side_rotation_frequency_ == 0) {
        TurnRotationReverseOn();
      } else {
        TurnRotationReverseOff();
      }
      number_of_turns_ = amount_of_turns_;
      number_of_turns_--;
      return true;
    }
  }
  return false;
}

bool Bot::IsMovingStartNeeded(
    const QList<std::shared_ptr<Movable>>&,
    const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>&) {
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
    return number_of_cells_to_move_ > 0;
  }
  return false;
}

bool Bot::IsShotNeeded(const std::shared_ptr<Map>& map,
                       const std::shared_ptr<Tank>& tank) {
  if (time_to_finish_rotation_ == 0 && time_to_finish_movement_ == 0) {
    int direction = GetIntDirection();
    size_t tank_x = tank->GetCellX();
    size_t tank_y = tank->GetCellY();
    size_t bot_x = GetCellX();
    size_t bot_y = GetCellY();
    if (map_->GetField(tank_x, tank_y) == CellType::Forest) {
      return false;
    }

    if (direction == 0 || direction == 2) {
      if (tank_x == bot_x) {
        if (IsWallBetweenObjectsX(map, tank_x, tank_y, bot_x, bot_y)) {
          return false;
        }

        if (CheckDirection(tank_y, bot_y, direction)) {
          return true;
        }
      }
    }
    if (direction == 1 || direction == 3) {
      if (tank_y == bot_y) {
        if (IsWallBetweenObjectsY(map, tank_x, tank_y, bot_x, bot_y)) {
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

bool Bot::CheckDirection(const int tank, const int bot, const int direction) {
  if (tank > bot) {
    if (direction == 0 || direction == 3) {
      return false;
    }
  } else if (direction == 2 || direction == 1) {
    return false;
  }
  return true;
}

bool Bot::IsWallBetweenObjectsX(const std::shared_ptr<Map>& map,
                                const size_t tank_x, const size_t tank_y,
                                const size_t bot_x, const size_t bot_y) {
  int walls_count = map->GetWallsPrecalc(bot_x, bot_y);
  walls_count += map->GetWallsPrecalc(tank_x - 1, tank_y - 1);
  walls_count -= map->GetWallsPrecalc(tank_x, tank_y - 1);
  walls_count -= map->GetWallsPrecalc(bot_x - 1, bot_y);
  return (walls_count != 0);
}

bool Bot::IsWallBetweenObjectsY(const std::shared_ptr<Map>& map,
                                const size_t tank_x, const size_t tank_y,
                                const size_t bot_x, const size_t bot_y) {
  int walls_count = map->GetWallsPrecalc(bot_x, bot_y);
  walls_count += map->GetWallsPrecalc(tank_x - 1, tank_y - 1);
  walls_count -= map->GetWallsPrecalc(tank_x - 1, tank_y);
  walls_count -= map->GetWallsPrecalc(bot_x, bot_y - 1);
  return (walls_count != 0);
}
