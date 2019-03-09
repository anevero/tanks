#include "tank.h"

Tank::Tank(Map* map)
    : Movable(map, map->tank_init_cell_x_, map->tank_init_cell_y_) {}

void Tank::Draw(QPainter& painter) {
  painter.setBrush(Qt::black);
  painter.drawEllipse(cur_upper_left_x_, cur_upper_left_y_, cur_width_,
                      cur_height_);
}
