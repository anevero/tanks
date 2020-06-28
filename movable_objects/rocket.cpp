#include "rocket.h"

Rocket::Rocket(std::shared_ptr<Map> map, std::shared_ptr<Tank> tank,
               RocketParameters parameters)
    : Movable(std::move(map), tank->GetCellX(), tank->GetCellY(),
              tank->GetDirection(), parameters.speed),
      tank_(std::move(tank)),
      power_(parameters.power),
      obstacle_break_(parameters.obstacle_break) {
  LoadImage(":/textures/rocket.png");
}

void Rocket::Draw(QPainter* painter) {
  painter->save();
  painter->translate(current_upper_left_x_ + current_width_ / 2,
                     current_upper_left_y_ + current_height_ / 2);
  painter->setOpacity(opacity_);
  painter->rotate(current_rotate_degree_);
  painter->drawPixmap(-current_width_ / 2,
                      -current_height_ / 2,
                      scaled_pixmap_);
  painter->restore();
}

std::shared_ptr<Tank> Rocket::GetAttachedTank() const {
  return tank_;
}

int Rocket::GetPower() const {
  return power_;
}

bool Rocket::CanBreakObstacles() const {
  return obstacle_break_;
}
