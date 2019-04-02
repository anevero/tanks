#include "tank.h"

Tank::Tank(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y,
           TankQualities qualities, Direction direction)
    : Movable(map, init_cell_x, init_cell_y, direction, qualities.speed),
      rate_of_fire_(qualities.rate_of_fire),
      health_(qualities.health),
      max_health_(qualities.health) {
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
  if (health_ > 30) {
    painter.setBrush(Qt::blue);
  } else {
    painter.setBrush(Qt::red);
  }
  painter.drawRect(-cur_width_ / 2, 5 * cur_height_ / 8,
                   health_ * cur_width_ / max_health_, cur_height_ / 8);
  painter.setBrush(Qt::white);
  painter.drawRect(
      -cur_width_ / 2 + health_ * cur_width_ / max_health_, 5 * cur_height_ / 8,
      (max_health_ - health_) * cur_width_ / max_health_, cur_height_ / 8);
  painter.restore();
}

bool Tank::IsAbleToShoot() const {
  return time_since_last_shot_ >= rate_of_fire_;
}

void Tank::IncreaseTimeSinceLastShot(int delta) {
  if (time_since_last_shot_ <= rate_of_fire_) {
    time_since_last_shot_ += delta;
  }
}

void Tank::SetZeroTimeFromLastShot() { time_since_last_shot_ = 0; }

void Tank::MinusHealth(int health) { health_ -= health; }

void Tank::PlusHealth(int health) { health_ += health; }

bool Tank::IsDead() const { return health_ <= 0; }
