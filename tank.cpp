#include "tank.h"

Tank::Tank(std::shared_ptr<Map>& map, int speed, int rate_of_fire,
           Direction direction)
    : Movable(map, map->GetTankInitCellX(), map->GetTankInitCellY(), direction,
              speed),
      rate_of_fire_(rate_of_fire) {}

void Tank::Draw(QPainter& painter) {
  painter.setBrush(Qt::black);
  painter.save();
  painter.translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 2);
  painter.rotate(current_rotate_degree_);
  painter.drawRect(-cur_width_ / 4, -cur_height_ / 2, cur_width_ / 2,
                   cur_height_);
  painter.restore();
  DrawHealth(painter);
}

void Tank::DrawHealth(QPainter& painter) {
  painter.save();
  painter.translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 2);
  if (health_ > 30) {
    painter.setBrush(Qt::blue);
  } else {
    painter.setBrush(Qt::red);
  }
  painter.drawRect(-cur_width_ / 2, 5 * cur_height_ / 8,
                   health_ * cur_width_ / 100, cur_height_ / 8);
  painter.setBrush(Qt::white);
  painter.drawRect(-cur_width_ / 2 + health_ * cur_width_ / 100,
                   5 * cur_height_ / 8, (100 - health_) * cur_width_ / 100,
                   cur_height_ / 8);
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
