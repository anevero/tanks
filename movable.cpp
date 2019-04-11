#include "movable.h"
#include "rocket.h"
#include "tank.h"

Movable::Movable(std::shared_ptr<Map>& map, int cell_x, int cell_y,
                 Direction direction, int speed)
    : cell_x_(cell_x),
      cell_y_(cell_y),
      map_(map),
      current_speed_(speed),
      basic_speed_(speed),
      current_rotate_degree_(90 * static_cast<int>(direction)) {
  directions_[static_cast<int>(direction)] = 1;
}

void Movable::StartMovement(
    int number_of_cells, QList<std::shared_ptr<Movable>>& tanks,
    std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& objects) {
  int new_cell_x = cell_x_ + reverse_ * (directions_[1] - directions_[3]);
  int new_cell_y = cell_y_ + reverse_ * (directions_[2] - directions_[0]);
  if (map_->GetField(new_cell_x, new_cell_y) == CellType::Wall) {
    cells_to_finish_movement_ = 0;
    return;
  }

  if (dynamic_cast<Tank*>(this) != nullptr) {
    for (const auto& object : tanks) {
      if (object->GetCellX() == new_cell_x &&
          object->GetCellY() == new_cell_y) {
        cells_to_finish_movement_ = 0;
        object->cells_to_finish_movement_ = 0;
        // зануление параметров движения танка
        return;
      }
    }
    current_speed_ = std ::max(
        static_cast<int>(map_->GetField(cell_x_, cell_y_)) * basic_speed_,
        static_cast<int>(map_->GetField(new_cell_x, new_cell_y)) *
            basic_speed_);
  }

  if (std::dynamic_pointer_cast<Obstacle>(objects[static_cast<unsigned>(
          new_cell_x)][static_cast<unsigned>(new_cell_y)]) != nullptr) {
    objects[static_cast<unsigned>(new_cell_x)]
           [static_cast<unsigned>(new_cell_y)] = nullptr;
    if (dynamic_cast<Rocket*>(this) != nullptr) {
      cells_to_finish_movement_ = 0;
      return;
    }
    current_speed_ = current_speed_ * 2;
  }

  cell_x_ = new_cell_x;
  cell_y_ = new_cell_y;
  time_to_finish_movement_ += current_speed_;
  cells_to_finish_movement_ = number_of_cells - 1;
}

void Movable::Move(int milliseconds_passed) {
  time_to_finish_movement_ -= milliseconds_passed;
}

void Movable::TurnReverseOn() { reverse_ = -1; }

void Movable::TurnReverseOff() { reverse_ = 1; }

void Movable::StartRotation() {
  current_speed_ =
      static_cast<int>(map_->GetField(cell_x_, cell_y_)) * basic_speed_;
  if (rotate_reverse_ == 1) {
    SwitchToNextDirection();
  } else {
    SwitchToPrevDirection();
  }
  time_to_finish_rotation_ = current_speed_;
}

void Movable::Rotate(int milliseconds_passed) {
  time_to_finish_rotation_ -= milliseconds_passed;
}

void Movable::TurnRotationReverseOn() { rotate_reverse_ = -1; }

void Movable::TurnRotationReverseOff() { rotate_reverse_ = 1; }

void Movable::UpdateCoordinates() {
  int cur_cell_width =
      static_cast<int>(map_->GetWidth() / map_->GetNumberOfCellsHorizontally());
  int cur_cell_height =
      static_cast<int>(map_->GetHeight() / map_->GetNumberOfCellsVertically());

  cur_width_ = cur_cell_width;
  cur_height_ = cur_cell_height;

  double movement_proportion =
      static_cast<double>(time_to_finish_movement_) / current_speed_;

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

  double rotation_proportion =
      static_cast<double>(time_to_finish_rotation_) / current_speed_;
  current_rotate_degree_ =
      GetIntDirection() * 90 -
      rotate_reverse_ * static_cast<int>(90 * rotation_proportion);
  current_rotate_degree_ %= 360;

  RescaleImage();
}

int Movable::GetSpeed() const { return current_speed_; }

int Movable::GetTimeToFinishMovement() const {
  return time_to_finish_movement_;
}

int Movable::GetCellsToFinishMovement() const {
  return cells_to_finish_movement_;
}

int Movable::GetTimeToFinishRotation() const {
  return time_to_finish_rotation_;
}

bool Movable::IsMovingOrRotating() const {
  return (GetTimeToFinishMovement() != 0 || GetTimeToFinishRotation() != 0 ||
          GetCellsToFinishMovement() != 0);
}

int Movable::GetReverseState() const { return reverse_; }

int Movable::GetRotationReverseState() const { return rotate_reverse_; }

int Movable::GetIntDirection() const {
  if (directions_[0] == 1) {
    return 0;
  }
  if (directions_[1] == 1) {
    return 1;
  }
  if (directions_[2] == 1) {
    return 2;
  }
  return 3;
}

Direction Movable::GetDirection() const {
  return static_cast<Direction>(GetIntDirection());
}

int Movable::GetUpperLeftX() const { return cur_upper_left_x_; }
int Movable::GetUpperLeftY() const { return cur_upper_left_y_; }
int Movable::GetWidth() const { return cur_width_; }
int Movable::GetHeight() const { return cur_height_; }
int Movable::GetCellX() const { return cell_x_; }
int Movable::GetCellY() const { return cell_y_; }

void Movable::SwitchToNextDirection() {
  int current_direction = GetIntDirection();
  directions_[current_direction] = 0;
  directions_[(current_direction + 1) % 4] = 1;
}

void Movable::SwitchToPrevDirection() {
  int current_direction = GetIntDirection();
  directions_[current_direction] = 0;
  directions_[(current_direction + 3) % 4] = 1;
}

void Movable::RescaleImage() {
  if (scaled_image_.width() == cur_width_ &&
      scaled_image_.height() == cur_height_) {
    return;
  }
  scaled_image_ = image_.scaled(cur_width_, cur_height_, Qt::KeepAspectRatio);
}
