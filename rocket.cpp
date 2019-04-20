#include "rocket.h"

Rocket::Rocket(const std::shared_ptr<Map>& map,
               const std::shared_ptr<Tank>& tank, const int speed,
               const int power)
    : Movable(map, tank->GetCellX(), tank->GetCellY(), tank->GetDirection(),
              speed),
      tank_(tank),
      power_(power) {
  LoadImage();
}

void Rocket::LoadImage() {
  image_.load(":/textures/rocket.png");
  scaled_image_ = image_;
}

void Rocket::Draw(QPainter& painter) {
  painter.save();
  painter.translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 2);
  painter.rotate(current_rotate_degree_);
  painter.drawImage(-cur_width_ / 2, -cur_height_ / 2, scaled_image_);
  painter.restore();
}

std::shared_ptr<Tank> Rocket::GetAttachedTank() const { return tank_; }

int Rocket::GetPower() const { return power_; }
