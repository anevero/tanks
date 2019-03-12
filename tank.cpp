#include "tank.h"

Tank::Tank(Map* map)
    : Movable(map, map->GetTankInitCellX(), map->GetTankInitCellY(),
              Direction::Up, 750) {}

void Tank::Draw(QPainter& painter) {
  painter.setBrush(Qt::black);
  painter.save();
  painter.translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 2);
  painter.rotate(current_rotate_degree_);
  painter.drawRect(-cur_width_ / 4, -cur_height_ / 2, cur_width_ / 2,
                   cur_height_);
  painter.restore();
}
