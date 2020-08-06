#include "rocket.h"

Rocket::Rocket(std::shared_ptr<const Map> map, std::shared_ptr<Tank> tank,
               RocketParameters parameters)
    : Movable(std::move(map), tank->GetCoordinates(),
              tank->GetDirection(), parameters.speed),
      tank_(std::move(tank)),
      power_(parameters.power),
      can_break_obstacle_(parameters.can_break_obstacle) {
  LoadImage(":/textures/rocket.png");
}

void Rocket::Draw(QPainter* painter) {
  painter->save();
  painter->translate(
      current_upper_left_cell_coordinates_.x + current_width_ / 2,
      current_upper_left_cell_coordinates_.y + current_height_ / 2);
  painter->setOpacity(opacity_);
  painter->rotate(current_rotate_degree_);
  painter->drawPixmap(-current_width_ / 2,
                      -current_height_ / 2,
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
