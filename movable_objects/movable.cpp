#include "movable.h"

#include <algorithm>
#include <unordered_map>

#include "rocket.h"
#include "tank.h"
#include "../game_core/constants.h"
#include "../static_objects/portal.h"

Movable::Movable(std::shared_ptr<const Map> map, Coordinates cell,
                 Direction direction, int speed)
    : current_cell_(cell), previous_cell_(cell),
      map_(std::move(map)), current_speed_(speed), basic_speed_(speed),
      current_rotate_degree_(90 * static_cast<int>(direction)) {
  directions_[static_cast<int>(direction)] = 1;
}

void Movable::LoadImage(const QString& path) {
  image_.load(path);
  scaled_pixmap_ = QPixmap::fromImage(image_);
}

void Movable::StartMovement(
    const std::list<std::shared_ptr<Tank>>& tanks,
    std::list<std::pair<std::shared_ptr<Tank>,
                        Coordinates>>* objects_copies_,
    std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>* objects,
    int number_of_cells) {
  Coordinates new_cell = {
      current_cell_.x +
          linear_movement_direction_ * (directions_[1] - directions_[3]),
      current_cell_.y +
          linear_movement_direction_ * (directions_[2] - directions_[0])};
  Coordinates old_cell = new_cell;

  if (map_->GetField(new_cell) == CellType::Wall) {
    cells_to_finish_movement_ = 0;
    return;
  }

  if (dynamic_cast<Tank*>(this) != nullptr) {
    for (auto& object : tanks) {
      if (object->GetCoordinates() == new_cell) {
        cells_to_finish_movement_ = 0;
        object->cells_to_finish_movement_ = 0;
        return;
      }
    }
    for (auto& object : *objects_copies_) {
      if (object.second == new_cell) {
        cells_to_finish_movement_ = 0;
        object.first->cells_to_finish_movement_ = 0;
        return;
      }
    }
    current_speed_ = std::max(
        static_cast<int>(map_->GetField(current_cell_)) * basic_speed_,
        static_cast<int>(map_->GetField(new_cell)) *
            basic_speed_);
  }

  if (auto portal = std::dynamic_pointer_cast<Portal>(
        (*objects)[new_cell.x][new_cell.y]);
      portal != nullptr && dynamic_cast<Rocket*>(this) == nullptr) {
    Coordinates cells = GetNewPortalCells(portal->GetNewCell(), new_cell);

    if (map_->GetField(cells) == CellType::Wall) {
      return;
    }

    for (auto& object : tanks) {
      if (object->GetCoordinates() == cells) {
        return;
      }
    }
    objects_copies_->emplace_back(
        std::static_pointer_cast<Tank>(shared_from_this()),
        Coordinates{cells.x, cells.y});
    copy_existence_ = true;

    new_cell = cells;
  }

  if ((*objects)[new_cell.x][new_cell.y] != nullptr &&
      std::dynamic_pointer_cast<Portal>(
          (*objects)[new_cell.x][new_cell.y]) == nullptr) {
    if (std::dynamic_pointer_cast<Obstacle>(
        (*objects)[new_cell.x][new_cell.y]) != nullptr) {
      current_speed_ *= 2;
    }
    if (auto tank = dynamic_cast<Tank*>(this); tank != nullptr) {
      if (std::dynamic_pointer_cast<MedicalKit>(
          (*objects)[new_cell.x][new_cell.y]) != nullptr) {
        tank->IncreaseHealth(
            std::min(35, tank->GetMaxHealth() - tank->GetCurrentHealth()));
      } else if (std::dynamic_pointer_cast<Charge>(
          (*objects)[new_cell.x][new_cell.y]) != nullptr) {
        tank->IncreaseCharge();
      }
    }
    if (auto rocket = dynamic_cast<Rocket*>(this); rocket != nullptr) {
      cells_to_finish_movement_ = 0;
      if (rocket->CanBreakObstacles()) {
        (*objects)[new_cell.x][new_cell.y] = nullptr;
      }
      return;
    }
    (*objects)[new_cell.x][new_cell.y] = nullptr;
  }

  new_cell = old_cell;
  previous_cell_ = current_cell_;
  current_cell_ = new_cell;
  time_to_finish_movement_ = current_speed_;
  cells_to_finish_movement_ = number_of_cells - 1;
}

void Movable::Move(const int milliseconds_passed) {
  time_to_finish_movement_ =
      std::max(time_to_finish_movement_ - milliseconds_passed, 0);
}

void Movable::TurnReverseOn() {
  linear_movement_direction_ = -1;
}

void Movable::TurnReverseOff() {
  linear_movement_direction_ = 1;
}

void Movable::StartRotation() {
  current_speed_ =
      static_cast<int>(map_->GetField(current_cell_)) * basic_speed_;
  if (rotation_direction_ == 1) {
    SwitchToNextDirection();
  } else {
    SwitchToPreviousDirection();
  }
  time_to_finish_rotation_ = current_speed_;
}

void Movable::Rotate(int milliseconds_passed) {
  time_to_finish_rotation_ -= milliseconds_passed;
  time_to_finish_rotation_ = std::max(time_to_finish_rotation_, 0);
}

void Movable::TurnRotationReverseOn() {
  rotation_direction_ = -1;
}

void Movable::TurnRotationReverseOff() {
  rotation_direction_ = 1;
}

void Movable::UpdateCoordinates(Coordinates cell) {
  if (current_cell_ != cell && GetTimeToFinishMovement() == 0) {
    current_cell_ = cell;
    copy_existence_ = false;
  }

  current_width_ = map_->GetCellWidth();
  current_height_ = map_->GetCellHeight();

  double movement_proportion = 1.0 * time_to_finish_movement_ / current_speed_;
  if (copy_existence_ && current_cell_ == cell) {
    opacity_ = movement_proportion;
    previous_opacity_ = opacity_;
  } else if (copy_existence_) {
    opacity_ = 1 - movement_proportion;
  }

  previous_upper_left_cell_coordinates_ = current_upper_left_cell_coordinates_;
  current_upper_left_cell_coordinates_.x =
      map_->GetUpperLeftCellCoordinates().x + (current_width_ * cell.x)
      - linear_movement_direction_ * movement_proportion
      * (directions_[1] * current_width_ - directions_[3] * current_width_);

  current_upper_left_cell_coordinates_.y =
      map_->GetUpperLeftCellCoordinates().y + (current_height_ * cell.y)
      - linear_movement_direction_ * movement_proportion
      * (directions_[2] * current_height_ - directions_[0] * current_height_);

  if (map_->GetField(current_cell_) == CellType::Forest) {
    if (movement_proportion <= 0.5) {
      opacity_ = constants::kOpacityLevel;
    }
  } else if (map_->GetField(previous_cell_) == CellType::Forest) {
    if (movement_proportion <= 0.5) {
      opacity_ = 1;
    }
  } else if (!copy_existence_) {
    opacity_ = 1;
  }

  double rotation_proportion =
      static_cast<double>(time_to_finish_rotation_) / current_speed_;
  current_rotate_degree_ =
      GetDirectionAsInt() * 90 -
          rotation_direction_ * static_cast<int>(90 * rotation_proportion);
  current_rotate_degree_ %= 360;

  RescaleImage();
}

void Movable::ReturnToOriginal() {
  current_upper_left_cell_coordinates_ = previous_upper_left_cell_coordinates_;
  opacity_ = previous_opacity_;
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

int Movable::GetDirectionAsInt() const {
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
  return static_cast<Direction>(GetDirectionAsInt());
}

Coordinates Movable::GetUpperLeftCellCoordinates() const {
  return current_upper_left_cell_coordinates_;
}

int Movable::GetWidth() const {
  return current_width_;
}

int Movable::GetHeight() const {
  return current_height_;
}

Coordinates Movable::GetCoordinates() const {
  return current_cell_;
}

bool Movable::HaveObjectsCollided(const std::shared_ptr<const Movable>& obj1,
                                  const std::shared_ptr<const Movable>& obj2) {
  if (obj1 == obj2 || Movable::CanRocketKillTank(obj1, obj2)) {
    return false;
  }

  if (obj1->GetUpperLeftCellCoordinates().x >=
      obj2->GetUpperLeftCellCoordinates().x + obj2->GetWidth()) {
    return false;
  }
  if (obj1->GetUpperLeftCellCoordinates().x + obj1->GetWidth() <=
      obj2->GetUpperLeftCellCoordinates().x) {
    return false;
  }
  if (obj1->GetUpperLeftCellCoordinates().y >=
      obj2->GetUpperLeftCellCoordinates().y + obj2->GetHeight()) {
    return false;
  }
  return obj1->GetUpperLeftCellCoordinates().y + obj1->GetHeight() >
      obj2->GetUpperLeftCellCoordinates().y;
}

bool Movable::CanRocketKillTank(const std::shared_ptr<const Movable>& rocket,
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
  int current_direction = GetDirectionAsInt();
  directions_[current_direction] = 0;
  directions_[(current_direction + 1) % 4] = 1;
}

void Movable::SwitchToPreviousDirection() {
  int current_direction = GetDirectionAsInt();
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

Coordinates Movable::GetNewPortalCells(Coordinates portal_coordinates,
                                       Coordinates new_cells) const {
  if (new_cells.y == current_cell_.y - 1) {
    --portal_coordinates.y;
  } else if (new_cells.y == current_cell_.y + 1) {
    ++portal_coordinates.y;
  } else if (new_cells.x == current_cell_.x - 1) {
    --portal_coordinates.x;
  } else {
    ++portal_coordinates.x;
  }
  return portal_coordinates;
}
