#include "movable.h"
#include "rocket.h"
#include "tank.h"

Movable::Movable(std::shared_ptr<const Map> map, int cell_x, int cell_y,
                 Direction direction, int speed)
    : cell_x_(cell_x), cell_y_(cell_y),
      prev_cell_x_(cell_x), prev_cell_y_(cell_y_),
      map_(std::move(map)), current_speed_(speed), basic_speed_(speed),
      current_rotate_degree_(90 * static_cast<int>(direction)) {
  directions_[static_cast<int>(direction)] = 1;
}

void Movable::LoadImage(const QString& path) {
  image_.load(path);
  scaled_pixmap_ = QPixmap::fromImage(image_);
}

void Movable::StartMovement(
    int number_of_cells,
    const std::list<std::shared_ptr<Tank>>& tanks,
    std::list<std::pair<std::shared_ptr<Tank>, Coordinates>>*
    objects_copies_,
    std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>* objects) {
  int new_cell_x = cell_x_ + reverse_ * (directions_[1] - directions_[3]);
  int new_cell_y = cell_y_ + reverse_ * (directions_[2] - directions_[0]);
  int old_cell_x = new_cell_x;
  int old_cell_y = new_cell_y;
  if (map_->GetField(new_cell_x, new_cell_y) == CellType::Wall) {
    cells_to_finish_movement_ = 0;
    return;
  }

  if (dynamic_cast<Tank*>(this) != nullptr) {
    for (auto& object : tanks) {
      if (object->GetCellX() == new_cell_x &&
          object->GetCellY() == new_cell_y) {
        cells_to_finish_movement_ = 0;
        object->cells_to_finish_movement_ = 0;
        return;
      }
    }
    for (auto& object : *objects_copies_) {
      if (object.second.x == new_cell_x && object.second.y == new_cell_y) {
        cells_to_finish_movement_ = 0;
        object.first->cells_to_finish_movement_ = 0;
        return;
      }
    }
    current_speed_ = std::max(
        static_cast<int>(map_->GetField(cell_x_, cell_y_)) * basic_speed_,
        static_cast<int>(map_->GetField(new_cell_x, new_cell_y)) *
            basic_speed_);
  }

  if (auto portal = std::dynamic_pointer_cast<Portal>(
        (*objects)[new_cell_x][new_cell_y]);
      portal != nullptr && dynamic_cast<Rocket*>(this) == nullptr) {
    Coordinates cells = GetNewPortalCells(
        portal->GetNewCellX(), portal->GetNewCellY(), new_cell_x, new_cell_y);

    if (map_->GetField(cells.x, cells.y) == CellType::Wall) {
      return;
    }

    for (auto& object : tanks) {
      if (object->GetCellX() == cells.x && object->GetCellY() == cells.y) {
        return;
      }
    }
    objects_copies_->emplace_back(
        std::static_pointer_cast<Tank>(shared_from_this()),
        Coordinates{cells.x, cells.y});
    copy_existence_ = true;

    new_cell_x = cells.x;
    new_cell_y = cells.y;
  }

  if ((*objects)[new_cell_x][new_cell_y] != nullptr &&
      std::dynamic_pointer_cast<Portal>(
          (*objects)[new_cell_x][new_cell_y]) == nullptr) {
    if (std::dynamic_pointer_cast<Obstacle>(
        (*objects)[new_cell_x][new_cell_y]) != nullptr) {
      current_speed_ *= 2;
    }
    if (auto tank = dynamic_cast<Tank*>(this); tank != nullptr) {
      if (std::dynamic_pointer_cast<MedicalKit>(
          (*objects)[new_cell_x][new_cell_y]) != nullptr) {
        tank->IncreaseHealth(
            std::min(35, tank->GetMaxHealth() - tank->GetCurrentHealth()));
      } else if (std::dynamic_pointer_cast<Charge>(
          (*objects)[new_cell_x][new_cell_y]) != nullptr) {
        tank->IncreaseCharge();
      }
    }
    if (auto rocket = dynamic_cast<Rocket*>(this); rocket != nullptr) {
      cells_to_finish_movement_ = 0;
      if (rocket->CanBreakObstacles()) {
        (*objects)[new_cell_x][new_cell_y] = nullptr;
      }
      return;
    }
    (*objects)[new_cell_x][new_cell_y] = nullptr;
  }

  new_cell_x = old_cell_x;
  new_cell_y = old_cell_y;
  prev_cell_x_ = cell_x_;
  prev_cell_y_ = cell_y_;
  cell_x_ = new_cell_x;
  cell_y_ = new_cell_y;
  time_to_finish_movement_ = current_speed_;
  cells_to_finish_movement_ = number_of_cells - 1;
}

void Movable::Move(const int milliseconds_passed) {
  time_to_finish_movement_ =
      std::max(time_to_finish_movement_ - milliseconds_passed, 0);
}

void Movable::TurnReverseOn() {
  reverse_ = -1;
}

void Movable::TurnReverseOff() {
  reverse_ = 1;
}

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
  time_to_finish_rotation_ = std::max(time_to_finish_rotation_, 0);
}

void Movable::TurnRotationReverseOn() {
  rotate_reverse_ = -1;
}

void Movable::TurnRotationReverseOff() {
  rotate_reverse_ = 1;
}

void Movable::UpdateCoordinates(int cell_x, int cell_y) {
  if ((cell_x_ != cell_x || cell_y_ != cell_y) &&
      GetTimeToFinishMovement() == 0) {
    cell_x_ = cell_x;
    cell_y_ = cell_y;
    copy_existence_ = false;
  }

  current_width_ = map_->GetCellWidth();
  current_height_ = map_->GetCellHeight();

  double movement_proportion = 1.0 * time_to_finish_movement_ / current_speed_;
  if (copy_existence_ && cell_x_ == cell_x && cell_y_ == cell_y) {
    opacity_ = movement_proportion;
    prev_opacity_ = opacity_;
  } else if (copy_existence_) {
    opacity_ = 1 - movement_proportion;
  }

  prev_upper_left_x_ = current_upper_left_x_;
  prev_upper_left_y_ = current_upper_left_y_;
  current_upper_left_x_ =
      map_->GetUpperLeftX() + (current_width_ * cell_x) - reverse_ *
          ((directions_[1] * current_width_ * movement_proportion)
              - (directions_[3] * current_width_ * movement_proportion));

  current_upper_left_y_ =
      map_->GetUpperLeftY() + (current_height_ * cell_y) - reverse_ *
          ((directions_[2] * current_height_ * movement_proportion)
              - (directions_[0] * current_height_ * movement_proportion));

  if (map_->GetField(cell_x_, cell_y_) == CellType::Forest) {
    if (movement_proportion <= 0.5) {
      opacity_ = constants::kOpacityLevel;
    }
  } else if (map_->GetField(prev_cell_x_, prev_cell_y_) == CellType::Forest) {
    if (movement_proportion <= 0.5) {
      opacity_ = 1;
    }
  } else if (!copy_existence_) {
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

void Movable::ReturnToOriginal() {
  current_upper_left_x_ = prev_upper_left_x_;
  current_upper_left_y_ = prev_upper_left_y_;
  opacity_ = prev_opacity_;
}

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
  return (GetTimeToFinishMovement() > 0 ||
      GetTimeToFinishRotation() > 0 ||
      GetCellsToFinishMovement() > 0);
}

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

int Movable::GetUpperLeftX() const {
  return current_upper_left_x_;
}

int Movable::GetUpperLeftY() const {
  return current_upper_left_y_;
}

int Movable::GetWidth() const {
  return current_width_;
}

int Movable::GetHeight() const {
  return current_height_;
}

int Movable::GetCellX() const {
  return cell_x_;
}

int Movable::GetCellY() const {
  return cell_y_;
}

bool Movable::HaveObjectsCollided(const std::shared_ptr<const Movable>& obj1,
                                  const std::shared_ptr<const Movable>& obj2) {
  if (obj1 == obj2 || Movable::IsRocketByThisTank(obj1, obj2)) {
    return false;
  }

  if (obj1->GetUpperLeftX() >= obj2->GetUpperLeftX() + obj2->GetWidth()) {
    return false;
  }
  if (obj1->GetUpperLeftX() + obj1->GetWidth() <= obj2->GetUpperLeftX()) {
    return false;
  }
  if (obj1->GetUpperLeftY() >= obj2->GetUpperLeftY() + obj2->GetHeight()) {
    return false;
  }
  if (obj1->GetUpperLeftY() + obj1->GetHeight() <= obj2->GetUpperLeftY()) {
    return false;
  }

  return true;
}

bool Movable::IsRocketByThisTank(const std::shared_ptr<const Movable>& rocket,
                                 const std::shared_ptr<const Movable>& tank) {
  auto casted_rocket = std::dynamic_pointer_cast<const Rocket>(rocket);
  auto casted_tank = std::dynamic_pointer_cast<const Tank>(tank);
  if (casted_rocket != nullptr && casted_tank != nullptr) {
    return casted_tank == casted_rocket->GetAttachedTank();
  }
  return false;
}

Direction Movable::GetDirectionFromString(const std::string& direction) {
  static std::unordered_map<std::string, Direction> map = {
      {"up", Direction::Up}, {"down", Direction::Down},
      {"left", Direction::Left}, {"right", Direction::Right}};
  return map[direction];
}

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
  if (scaled_pixmap_.width() == current_width_ &&
      scaled_pixmap_.height() == current_height_) {
    return;
  }
  scaled_pixmap_ = QPixmap::fromImage(
      image_.scaled(current_width_, current_height_, Qt::KeepAspectRatio));
}

Coordinates Movable::GetNewPortalCells(int portal_cell_x, int portal_cell_y,
                                       int new_cell_x, int new_cell_y) const {
  Coordinates cells = {portal_cell_x, portal_cell_y};
  if (new_cell_y == cell_y_ - 1) {
    --cells.y;
  } else if (new_cell_y == cell_y_ + 1) {
    ++cells.y;
  } else if (new_cell_x == cell_x_ - 1) {
    --cells.x;
  } else {
    ++cells.x;
  }
  return cells;
}
