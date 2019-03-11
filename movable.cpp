#include "movable.h"

Movable::Movable(Map* map, int cell_x, int cell_y, Direction direction,
                 int speed)
    : cell_x_(cell_x), cell_y_(cell_y), map_(map), speed_(speed) {
  directions_[static_cast<int>(direction)] = 1;
}

void Movable::StartMovement(int number_of_cells) {
  int new_cell_x = cell_x_ + reverse_ * (directions_[1] - directions_[3]);
  int new_cell_y = cell_y_ + reverse_ * (directions_[2] - directions_[0]);
  if (map_->GetField(new_cell_x, new_cell_y) == CellType::Wall) {
    return;
  }
  cell_x_ = new_cell_x;
  cell_y_ = new_cell_y;
  time_to_finish_movement_ += speed_;
  cells_to_finish_movement_ = number_of_cells - 1;
}

void Movable::SwitchToNextDirection() {
  for (int i = 0; i < 4; ++i) {
    if (directions_[i] == 1) {
      directions_[i] = 0;
      directions_[(i + 1) % 4] = 1;
      break;
    }
  }
}

void Movable::SwitchToPrevDirection() {
  for (int i = 0; i < 4; ++i) {
    if (directions_[i] == 1) {
      directions_[i] = 0;
      directions_[(i + 3) % 4] = 1;
      break;
    }
  }
}

void Movable::Move(int milliseconds_passed) {
  time_to_finish_movement_ -= milliseconds_passed;
}

void Movable::TurnReverseOn() { reverse_ = -1; }

void Movable::TurnReverseOff() { reverse_ = 1; }

void Movable::UpdateCoordinates() {
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
      reverse_ * static_cast<int>(
                     (directions_[1] * cur_cell_width * movement_proportion) -
                     (directions_[3] * cur_cell_width * movement_proportion));

  cur_upper_left_y_ =
      map_->GetUpperLeftY() + (cur_cell_height * cell_y_) -
      reverse_ * static_cast<int>(
                     (directions_[2] * cur_cell_height * movement_proportion) -
                     (directions_[0] * cur_cell_height * movement_proportion));
}

int Movable::GetSpeed() const { return speed_; }

int Movable::GetTimeToFinishMovement() const {
  return time_to_finish_movement_;
}

int Movable::GetCellsToFinishMovement() const {
  return cells_to_finish_movement_;
}

int Movable::GetReverseState() const { return reverse_; }

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
