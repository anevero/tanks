﻿#include "tank.h"

Tank::Tank(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y,
           TankQualities qualities)
    : Movable(map, init_cell_x, init_cell_y, qualities.direction,
              qualities.speed),
      rate_of_fire_(qualities.rate_of_fire),
      current_health_(qualities.max_health),
      type_of_charge_(0),
      light_current_charge_(qualities.max_light_charge),
      medium_current_charge_(qualities.max_medium_charge),
      hard_current_charge_(qualities.max_hard_charge),
      max_health_(qualities.max_health),
      max_light_charge_(qualities.max_light_charge),
      max_medium_charge_(qualities.max_medium_charge),
      max_hard_charge_(qualities.max_hard_charge) {
  LoadImage();
}

void Tank::LoadImage() {
  image_.load(":/textures/tank.png");
  scaled_image_ = image_;
}

void Tank::Draw(QPainter& painter) {
  painter.save();
  painter.translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 2);
  painter.rotate(current_rotate_degree_);
  painter.drawImage(-cur_width_ / 2, -cur_height_ / 2, scaled_image_);
  painter.restore();
  DrawHealth(painter);
}

void Tank::DrawHealth(QPainter& painter) {
  painter.save();
  painter.translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 4);
  if (current_health_ > static_cast<int>(max_health_ * 0.3)) {
    painter.setBrush(Qt::blue);
  } else {
    painter.setBrush(Qt::red);
  }
  painter.drawRect(-cur_width_ / 2, 5 * cur_height_ / 8,
                   current_health_ * cur_width_ / max_health_, cur_height_ / 8);
  painter.setBrush(Qt::white);
  painter.drawRect(-cur_width_ / 2 + current_health_ * cur_width_ / max_health_,
                   5 * cur_height_ / 8,
                   (max_health_ - current_health_) * cur_width_ / max_health_,
                   cur_height_ / 8);
  painter.restore();
}

bool Tank::IsAbleToShoot() const {
  return ((light_current_charge_ > 0 || medium_current_charge_ > 0 ||
           hard_current_charge_ > 0) &&
          time_since_last_shot_ >= rate_of_fire_);
}

void Tank::IncreaseTimeSinceLastShot(int delta) {
  if (time_since_last_shot_ <= rate_of_fire_) {
    time_since_last_shot_ += delta;
  }
}

void Tank::SetZeroTimeFromLastShot() { time_since_last_shot_ = 0; }
int Tank::GetCurrentHealth() const { return current_health_; }
int Tank::GetMaxHealth() const { return max_health_; }
void Tank::MinusHealth(int health) { current_health_ -= health; }
void Tank::PlusHealth(int health) { current_health_ += health; }
void Tank::ChangeTypeOfCharge(int type) { type_of_charge_ = type; }
int Tank::GetTypeOfCharge() const { return type_of_charge_; }
int Tank::GetLightCurrentCharge() const { return light_current_charge_; }
int Tank::GetMaxLightCharge() const { return max_light_charge_; }
void Tank::MinusLightCharge(int charge) { light_current_charge_ -= charge; }
void Tank::PlusLightCharge(int charge) { light_current_charge_ += charge; }
int Tank::GetMediumCurrentCharge() const { return medium_current_charge_; }
int Tank::GetMaxMediumCharge() const { return max_medium_charge_; }
void Tank::MinusMediumCharge(int charge) { medium_current_charge_ -= charge; }
void Tank::PlusMediumCharge(int charge) { medium_current_charge_ += charge; }
int Tank::GetHardCurrentCharge() const { return hard_current_charge_; }
int Tank::GetMaxHardCharge() const { return max_hard_charge_; }
void Tank::MinusHardCharge(int charge) { hard_current_charge_ -= charge; }
void Tank::PlusHardCharge(int charge) { hard_current_charge_ += charge; }
bool Tank::IsDead() const { return current_health_ <= 0; }
int Tank::GetTimeSinceLastShot() const { return time_since_last_shot_; }
int Tank::GetRateOfFire() const { return rate_of_fire_; }
