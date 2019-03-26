#include "bot.h"

Bot::Bot(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y, int speed,
         int rate_of_fire, Direction direction) :
         Tank(map, init_cell_x, init_cell_y, speed, rate_of_fire, direction) {};

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
}

bool Bot::DoesNeedToTurn() const {
  if (time_to_finish_rotation_ > 0) {
    return true;
  } else {
    return false;
  }
}

bool Bot::DoesNeedToStartRotation() const {
  if (time_to_finish_rotation_ <= 0) {
    return true;
  } else {
    return false;
  }
}

bool Bot::DoesNeedToShoot(std::shared_ptr<Map> map, std::shared_ptr<Tank> tank) const {
  if (time_to_finish_rotation_ <= 0) {
    if (GetIntDirection() == 0) {
      if (tank->GetCellX() == GetCellX()) {
        for (int cell = GetCellY() - 1; cell >= 0; cell--) {
          if (map->WallCheck(GetCellX(), cell)) { break; }
          if (tank->GetCellY() == cell) { return true; }
        }
      }
    } else if (GetIntDirection() == 1) {
      if (tank->GetCellY() == GetCellY()) {
        for (int cell = GetCellX() + 1; cell < map->GetNumberOfCellsHorizontally(); cell++) {
          if (map->WallCheck(cell, GetCellY())) { break; }
          if (tank->GetCellX() == cell) { return true; }
        }
      }
    } else if (GetIntDirection() == 2) {
      if (tank->GetCellX() == GetCellX()) {
        for (int cell = GetCellY() + 1; cell < map->GetNumberOfCellsVertically(); cell++) {
          if (map->WallCheck(GetCellX(), cell)) { break; }
          if (tank->GetCellY() == cell) { return true; }
        }
      }
    } else if (GetIntDirection() == 3) {
      if (tank->GetCellY() == GetCellY()) {
        for (int cell = GetCellX() - 1; cell >= 0; cell--) {
          if (map->WallCheck(cell, GetCellY())) { break; }
          if (tank->GetCellX() == cell) { return true; }
        }
      }
    }
    return false;
  } else {
    return false;
  }
}
