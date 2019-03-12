#include "tank.h"

Tank::Tank(Map* map, int speed, int rate_of_fire)
    : Movable(map, map->GetTankInitCellX(), map->GetTankInitCellY(),
              Direction::Up, speed), rate_of_fire_(rate_of_fire) {}

void Tank::Draw(QPainter& painter) {
  painter.setBrush(Qt::black);
  painter.drawRect(cur_upper_left_x_, cur_upper_left_y_, cur_width_,
                   cur_height_);
}

int Tank::GetRateOfFire() const { return rate_of_fire_; }
int Tank::GetTimeSinceLastShot() const { return time_since_last_shot_; }

void Tank::SetTimeSinceLastShot(int delta) {
  time_since_last_shot_ += delta;
}
