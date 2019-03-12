#include "rocket.h"

Rocket::Rocket(Map* map, Movable* movable, int speed)
    : Movable(map, movable->GetCellX(), movable->GetCellY(),
              movable->GetDirection(), speed) {}

void Rocket::Draw(QPainter& painter) {
  painter.setBrush(Qt::blue);
  painter.drawEllipse(cur_upper_left_x_, cur_upper_left_y_, cur_width_,
                      cur_height_);
}

void Rocket::Explosion() {
  time_to_finish_movement_ = 0;
  cells_to_finish_movement_ = 0;
}
