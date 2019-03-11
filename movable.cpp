#include "movable.h"

Movable::Movable(Map* map, int cell_x, int cell_y, Direction direction,
                 int speed)
    : cell_x_(cell_x),
      cell_y_(cell_y),
      map_(map),
      directions_({0, 0, 0, 0}),
      speed_(speed) {
  directions_[static_cast<int>(direction)] = 1;
}

void Movable::StartMovement() {
  cell_x_ = cell_x_ + directions_[1] - directions_[3];
  cell_y_ = cell_y_ + directions_[2] - directions_[0];
  time_to_finish_movement_ += speed_;
}

void Movable::SwitchToNextDirection() {
  // todo
}

void Movable::SwitchToPrevDirection() {
  // todo
}

void Movable::Move(int milliseconds_passed) {
  time_to_finish_movement_ -= milliseconds_passed;
}

void Movable::UpdateCoordinates() {
  // получаем текущие размеры ячеек из размеров карты
  int cur_cell_width =
      static_cast<int>(map_->GetWidth() / map_->GetNumberOfCellsHorizontally());
  int cur_cell_height =
      static_cast<int>(map_->GetHeight() / map_->GetNumberOfCellsVertically());

  cur_width_ = cur_cell_width;
  cur_height_ = cur_cell_height;

  double movement_proportion =
      static_cast<double>(time_to_finish_movement_) / speed_;

  cur_upper_left_x_ =
      map_->GetUpperLeftX() + (cur_cell_width * cell_x_) -
      directions_[1] * static_cast<int>(cur_cell_width * movement_proportion) +
      directions_[3] * static_cast<int>(cur_cell_width * movement_proportion);

  cur_upper_left_y_ =
      map_->GetUpperLeftY() + (cur_cell_height * cell_y_) -
      directions_[2] * static_cast<int>(cur_cell_height * movement_proportion) +
      directions_[0] * static_cast<int>(cur_cell_height * movement_proportion);

  qDebug() << movement_proportion << " " << cur_upper_left_x_ << " "
           << cur_upper_left_y_;
}

int Movable::GetSpeed() const { return speed_; }

int Movable::GetTimeToFinishMovement() const {
  return time_to_finish_movement_;
}
Direction Movable::GetDirection() const {
  if (directions_[0] == 1) {
    return Direction::Up;
  }
  if (directions_[1] == 1) {
    return Direction::Right;
  }
  if (directions_[2] == 1) {
    return Direction::Down;
  }
  return Direction::Left;
}
