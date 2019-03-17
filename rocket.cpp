#include "rocket.h"

Rocket::Rocket(std::shared_ptr<Map> map, std::shared_ptr<Movable> movable,
               int speed)
    : Movable(map, movable->GetCellX(), movable->GetCellY(),
              movable->GetDirection(), speed) {}

void Rocket::Draw(QPainter& painter) {
  painter.setBrush(Qt::blue);
  painter.save();
  painter.translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 2);
  painter.rotate(current_rotate_degree_);
  painter.drawEllipse(-cur_width_ / 8, -cur_height_ / 2, cur_width_ / 4,
                      cur_height_ / 3);
  painter.restore();
}

void Rocket::Explosion() {
  time_to_finish_movement_ = 0;
  cells_to_finish_movement_ = 0;
}
