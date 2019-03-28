#include "bot.h"

Bot::Bot(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y, int speed,
         int rate_of_fire, Direction direction)
    : Tank(map, init_cell_x, init_cell_y, speed, rate_of_fire, direction){};

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

bool Bot::IsRotationStartNeeded() const {
  return time_to_finish_rotation_ <= 0;
}

bool Bot::IsShotNeeded(std::shared_ptr<Map> map,
                          std::shared_ptr<Tank> tank) const {
  if (time_to_finish_rotation_ <= 0) {
    int direction = GetIntDirection();
    if (direction == 0 || direction == 2) {
      if (tank->GetCellX() == GetCellX()) {
        int delta = (direction == 0) ? -1 : 1;
        int finish_cell =
            (direction == 0) ? 0 : map->GetNumberOfCellsVertically();
        int curr_cell = GetCellY() + delta;
        while (curr_cell != finish_cell
               && map_->GetField(GetCellX(), curr_cell) != CellType::Wall) {
          if (tank->GetCellY() == curr_cell) {
            return true;
          }
          curr_cell += delta;
        }
      }
    }
    if (direction == 1 || direction == 3) {
      if (tank->GetCellY() == GetCellY()) {
        int delta = (direction == 3) ? -1 : 1;
        int finish_cell =
            (direction == 3) ? 0 : map->GetNumberOfCellsHorizontally();
        int curr_cell = GetCellX() + delta;
        while (curr_cell != finish_cell
               && map_->GetField(curr_cell, GetCellY()) != CellType::Wall) {
          if (tank->GetCellX() == curr_cell) {
            return true;
          }
          curr_cell += delta;
        }
      }
    }
  }
  return false;
}
