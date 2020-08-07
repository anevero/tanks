#include "tank.h"

#include <algorithm>

std::mt19937 Tank::random_generator_ = std::mt19937(
    std::chrono::system_clock::now().time_since_epoch().count());

Tank::Tank(std::shared_ptr<const Map> map, const QString& path,
           Coordinates init_cell, TankParameters parameters,
           Direction direction)
    : Movable(std::move(map), path, init_cell, direction, parameters.speed),
      rate_of_fire_(parameters.rate_of_fire),
      time_since_last_shot_(0),
      current_type_of_charge_(0),
      current_charge_({parameters.max_light_charge,
                       parameters.max_medium_charge,
                       parameters.max_hard_charge}),
      current_health_(parameters.max_health),
      max_charge_({parameters.max_light_charge,
                   parameters.max_medium_charge,
                   parameters.max_hard_charge}),
      max_health_(parameters.max_health) {}

void Tank::Draw(QPainter* painter) {
  painter->save();
  painter->translate(
      upper_left_cell_coordinates_.x + width_ / 2,
      upper_left_cell_coordinates_.y + height_ / 2);
  painter->rotate(rotate_degree_);
  painter->setOpacity(opacity_);
  painter->drawPixmap(-width_ / 2, -height_ / 2,
                      scaled_pixmap_);
  painter->restore();
  DrawHealth(painter);
}

void Tank::DrawHealth(QPainter* painter) {
  painter->save();
  painter->translate(
      upper_left_cell_coordinates_.x + width_ / 2,
      upper_left_cell_coordinates_.y + height_ / 4);
  if (current_health_ > 0.3 * max_health_ * 0.3) {
    painter->setBrush(Qt::blue);
  } else {
    painter->setBrush(Qt::red);
  }
  painter->drawRect(-width_ / 2,
                    5 * height_ / 8,
                    current_health_ * width_ / max_health_,
                    height_ / 8);
  painter->setBrush(Qt::white);
  painter->drawRect(
      -width_ / 2 + current_health_ * width_ / max_health_,
      5 * height_ / 8,
      (max_health_ - current_health_) * width_ / max_health_,
      height_ / 8);
  painter->restore();
}

bool Tank::IsAbleToShoot() const {
  return (time_since_last_shot_ >= rate_of_fire_) &&
      ((current_type_of_charge_ == 0 && current_charge_[0] > 0) ||
          (current_type_of_charge_ == 1 && current_charge_[1] > 0) ||
          (current_type_of_charge_ == 2 && current_charge_[2] > 0));
}

void Tank::IncreaseTimeSinceLastShot(int delta) {
  time_since_last_shot_ =
      std::min(time_since_last_shot_ + delta, rate_of_fire_);
}

void Tank::SetZeroTimeFromLastShot() {
  time_since_last_shot_ = 0;
}

int Tank::GetCurrentHealth() const {
  return current_health_;
}

int Tank::GetMaxHealth() const {
  return max_health_;
}

void Tank::DecreaseHealth(int health) {
  current_health_ -= health;
}

void Tank::IncreaseHealth(int health) {
  current_health_ += health;
}

bool Tank::IsDead() const {
  return current_health_ <= 0;
}

int Tank::GetTimeSinceLastShot() const {
  return time_since_last_shot_;
}

int Tank::GetRateOfFire() const {
  return rate_of_fire_;
}

void Tank::ChangeTypeOfCharge(int type) {
  current_type_of_charge_ = type;
}

void Tank::DecreaseCharge(int type, int charge) {
  current_charge_[type] -= charge;
}

void Tank::IncreaseCharge() {
  int type = random_generator_() % 3;
  current_charge_[type] +=
      std::min(10 - 2 * type, max_charge_[type] - current_charge_[type]);
}

int Tank::GetTypeOfCharge() const {
  return current_type_of_charge_;
}

int Tank::GetCurrentCharge(int type) const {
  return current_charge_[type];
}

ChargeState Tank::GetChargeState() const {
  if (current_charge_[current_type_of_charge_] == 0) {
    return ChargeState::Empty;
  } else if (current_charge_[current_type_of_charge_] <=
      max_charge_[current_type_of_charge_] / 2) {
    return ChargeState::LessThanHalf;
  }
  return ChargeState::MoreThanHalf;
}
