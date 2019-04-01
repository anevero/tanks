#include "bot.h"

Bot::Bot(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y, int speed,
         int rate_of_fire, Direction direction, int moving_length, int amount_of_turns)
    : Tank(map, init_cell_x, init_cell_y, speed, rate_of_fire, direction),
      moving_length_(moving_length),
      amount_of_turns_(amount_of_turns) {};

void Bot::Draw(QPainter& painter) {
  painter.setBrush(Qt::yellow);
  painter.save();
  painter.translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 2);
  painter.rotate(current_rotate_degree_);
  painter.drawEllipse(-cur_width_ / 4, -cur_height_ / 4, cur_width_ / 2,
                      cur_height_ / 4 * 3);
  painter.setBrush(Qt::blue);
  painter.drawEllipse(-cur_width_ / 2, -cur_height_ / 2, cur_width_,
                      cur_height_ / 4);
  painter.restore();
  DrawHealth(painter);
}

bool Bot::IsTurnNeeded() const {
  return time_to_finish_rotation_ > 0;
}

bool Bot::IsRotationStartNeeded(std::shared_ptr<Tank>) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (number_of_turns_ > 0) {
      number_of_turns_--;
      return number_of_turns_ > 0 ? true : false;
    }
    if (number_of_cells_to_move_ == 0) {
      if (qrand() % 2 == 0) {
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

bool Bot::IsMoveNeeded() const {
  return time_to_finish_movement_ > 0;
}

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

bool Bot::IsShotNeeded(std::shared_ptr<Map> map,
                       std::shared_ptr<Tank> tank) {
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
    std::swap(tank, bot);
    if (direction == 0 || direction == 3) {
      return false;
    }
  } else if (direction == 2 || direction == 1) {
    return false;
  }
  return true;
}
