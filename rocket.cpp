#include "rocket.h"

Rocket::Rocket(Map* map, Movable* movable, int speed)
    : Movable(map, movable->GetUpperLeftX(), movable->GetUpperLeftY(),
              movable->GetDirection(), speed) {
  StartMovement(10);
  qDebug() << "Create";
}

void Rocket::Draw(QPainter& painter) {
  painter.setBrush(Qt::blue);
  painter.drawEllipse(cur_upper_left_x_, cur_upper_left_y_, cur_width_,
                   cur_height_);
}
