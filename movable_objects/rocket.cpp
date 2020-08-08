#include "rocket.h"

Rocket::Rocket(std::shared_ptr<const Map> map, std::shared_ptr<Tank> tank,
               RocketParameters parameters)
    : Movable(std::move(map), ":/textures/rocket.png", tank->GetCoordinates(),
              tank->GetDirection(), parameters.speed),
      tank_(std::move(tank)),
      power_(parameters.power),
      can_break_obstacle_(parameters.can_break_obstacle) {}

void Rocket::Draw(QPainter* painter) {
  painter->save();
  painter->translate(
      upper_left_cell_coordinates_.x + width_ / 2,
      upper_left_cell_coordinates_.y + height_ / 2);
  painter->setOpacity(opacity_);
  painter->rotate(rotate_degree_);
  painter->drawPixmap(-width_ / 2,
                      -height_ / 2,
                      scaled_pixmap_);
  painter->restore();
}

std::shared_ptr<const Tank> Rocket::GetAttachedTank() const {
  return tank_;
}

int Rocket::GetPower() const {
  return power_;
}

bool Rocket::CanBreakObstacles() const {
  return can_break_obstacle_;
}
