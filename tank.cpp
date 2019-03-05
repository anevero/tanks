#include "tank.h"

Tank::Tank(Map* map)
    : cell_x_(map->tank_init_cell_x_),
      cell_y_(map->tank_init_cell_y_),
      map_(map) {}

void Tank::UpdateCoordinates() {
  int cur_cell_width = static_cast<int>(map_->cur_width_ / map_->map_.size());
  int cur_cell_height =
      static_cast<int>(map_->cur_height_ / map_->map_[0].size());

  cur_width_ = static_cast<int>(0.5 * cur_cell_width);
  cur_height_ = static_cast<int>(0.7 * cur_cell_height);
  cur_upper_left_x_ = map_->cur_upper_left_x_ + (cur_cell_width * cell_x_) +
                      (static_cast<int>(0.25 * cur_cell_width));
  cur_upper_left_y_ = map_->cur_upper_left_y_ + (cur_cell_height * cell_y_) +
                      (static_cast<int>(0.3 * cur_cell_height));
}

void Tank::DrawTank(QPainter& painter) {
  painter.setBrush(Qt::black);
  painter.drawEllipse(cur_upper_left_x_, cur_upper_left_y_, cur_width_,
                      cur_height_);
}
