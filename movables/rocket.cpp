#include "rocket.h"

Rocket::Rocket(const std::shared_ptr<Map>& map,
               const std::shared_ptr<Tank>& tank, const int speed,
               const int power, const TypeOfRocket& type)
    : Movable(map, tank->GetCellX(), tank->GetCellY(), tank->GetDirection(),
              speed),
      tank_(tank),
      power_(power),
      type_(type) {
  LoadImage(":/textures/rocket.png");
}

void Rocket::Draw(QPainter* painter) {
  painter->save();
  painter->translate(cur_upper_left_x_ + cur_width_ / 2,
                     cur_upper_left_y_ + cur_height_ / 2);
  painter->setOpacity(opacity_);
  painter->rotate(current_rotate_degree_);
  painter->drawPixmap(-cur_width_ / 2, -cur_height_ / 2, scaled_pixmap_);
  painter->restore();
}

std::shared_ptr<Tank> Rocket::GetAttachedTank() const { return tank_; }

int Rocket::GetPower() const { return power_; }

TypeOfRocket Rocket::GetTypeOfRocket() const { return type_; }
