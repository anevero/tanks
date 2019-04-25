#include "tank.h"

Tank::Tank(const std::shared_ptr<Map>& map, const int init_cell_x,
           const int init_cell_y, const TankQualities& qualities)
    : Movable(map, init_cell_x, init_cell_y, qualities.direction,
              qualities.speed),
      rate_of_fire_(qualities.rate_of_fire),
      type_of_charge_(0),
      current_charge_({qualities.max_light_charge, qualities.max_medium_charge,
                       qualities.max_hard_charge}),
      max_charge_({qualities.max_light_charge, qualities.max_medium_charge,
                   qualities.max_hard_charge}),
      max_health_(qualities.max_health),
      current_health_(qualities.max_health) {
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
  painter.setOpacity(opacity_);
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
  return (time_since_last_shot_ >= rate_of_fire_) &&
         ((type_of_charge_ == 0 && current_charge_[0] > 0) ||
          (type_of_charge_ == 1 && current_charge_[1] > 0) ||
          (type_of_charge_ == 2 && current_charge_[2] > 0));
}

void Tank::IncreaseTimeSinceLastShot(const int delta) {
  if (time_since_last_shot_ <= rate_of_fire_) {
    time_since_last_shot_ += delta;
  }
}

void Tank::SetZeroTimeFromLastShot() { time_since_last_shot_ = 0; }
int Tank::GetCurrentHealth() const { return current_health_; }
int Tank::GetMaxHealth() const { return max_health_; }
void Tank::MinusHealth(const int health) { current_health_ -= health; }
void Tank::PlusHealth(const int health) { current_health_ += health; }
void Tank::ChangeTypeOfCharge(int type) { type_of_charge_ = type; }
int Tank::GetTypeOfCharge() const { return type_of_charge_; }
int Tank::GetCurrentCharge(int type) const { return current_charge_[type]; }
int Tank::GetMaxCharge(int type) const { return max_charge_[type]; }

void Tank::MinusCharge(int type, int charge) {
  current_charge_[type] -= charge;
}
void Tank::PlusCharge() {
  int type = rand() % 2;
  current_charge_[type] +=
      std::min(10 - 2 * type, max_charge_[type] - current_charge_[type]);
}

bool Tank::IsDead() const { return current_health_ <= 0; }
int Tank::GetTimeSinceLastShot() const { return time_since_last_shot_; }
int Tank::GetRateOfFire() const { return rate_of_fire_; }
