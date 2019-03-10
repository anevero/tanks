#include "tank.h"

Tank::Tank(Map* map)
    : Movable(map, map->GetTankInitCellX(), map->GetTankInitCellY()) {}

void Tank::Draw(QPainter& painter) {
  painter.setBrush(Qt::black);
  painter.drawRect(cur_upper_left_x_, cur_upper_left_y_, cur_width_,
                   cur_height_);
}
