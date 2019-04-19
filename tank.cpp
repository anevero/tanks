#include "tank.h"

Tank::Tank(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y,
           TankQualities qualities)
    : Movable(map, init_cell_x, init_cell_y, qualities.direction,
              qualities.speed),
      rate_of_fire_(qualities.rate_of_fire),
      current_health_(qualities.max_health),
      max_health_(qualities.max_health) {
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

void Tank::UpdateCoordinates(int cell_x, int cell_y) {
    if (GetTimeToFinishMovement() == 0) {
      cell_x_ = cell_x;
      cell_y_ = cell_y;
      copy_existence_ = false;
    }

    int cur_cell_width =
        static_cast<int>(map_->GetWidth() / map_->GetNumberOfCellsHorizontally());
    int cur_cell_height =
        static_cast<int>(map_->GetHeight() / map_->GetNumberOfCellsVertically());

    cur_width_ = cur_cell_width;
    cur_height_ = cur_cell_height;

    double movement_proportion =
        static_cast<double>(time_to_finish_movement_) / current_speed_;
    opacity_ = 1 - movement_proportion;

    prev_upper_left_x_ = cur_upper_left_x_;
    prev_upper_left_y_ = cur_upper_left_y_;
    cur_upper_left_x_ =
        map_->GetUpperLeftX() + (cur_cell_width * cell_x) -
        reverse_ * static_cast<int>(
                       (directions_[1] * cur_cell_width * movement_proportion) -
                       (directions_[3] * cur_cell_width * movement_proportion));

    cur_upper_left_y_ =
        map_->GetUpperLeftY() + (cur_cell_height * cell_y) -
        reverse_ * static_cast<int>(
                       (directions_[2] * cur_cell_height * movement_proportion) -
                       (directions_[0] * cur_cell_height * movement_proportion));

    if (map_->GetField(cell_x, cell_y) == CellType::Forest) {
      if (movement_proportion <= 0.5) {
        opacity_ = 0.5;
      }
    } else if (copy_existence_) {
      opacity_ = 1;
    }

    double rotation_proportion =
        static_cast<double>(time_to_finish_rotation_) / current_speed_;
    current_rotate_degree_ =
        GetIntDirection() * 90 -
        rotate_reverse_ * static_cast<int>(90 * rotation_proportion);
    current_rotate_degree_ %= 360;

    RescaleImage();
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
  return time_since_last_shot_ >= rate_of_fire_;
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

bool Tank::IsDead() const { return current_health_ <= 0; }
