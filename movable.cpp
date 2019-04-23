#include "movable.h"
#include "rocket.h"
#include "tank.h"

Movable::Movable(const std::shared_ptr<Map>& map, const int cell_x,
                 const int cell_y, const Direction direction, const int speed)
    : cell_x_(cell_x),
      cell_y_(cell_y),
      map_(map),
      current_speed_(speed),
      basic_speed_(speed),
      current_rotate_degree_(90 * static_cast<int>(direction)) {
  directions_[static_cast<int>(direction)] = 1;
}

void Movable::StartMovement(
    const int number_of_cells, const QList<std::shared_ptr<Movable>>& tanks,
    QList<QPair<std::shared_ptr<Movable>, Coordinates>>& objects_copies_,
    std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& objects) {
  int new_cell_x = cell_x_ + reverse_ * (directions_[1] - directions_[3]);
  int new_cell_y = cell_y_ + reverse_ * (directions_[2] - directions_[0]);
  int old_cell_x = new_cell_x;
  int old_cell_y = new_cell_y;
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
        return;
      }
    }
    for (const auto& object : objects_copies_) {
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

  if (std::dynamic_pointer_cast<Portal>(objects[static_cast<unsigned>(
          new_cell_x)][static_cast<unsigned>(new_cell_y)]) != nullptr) {
    if (dynamic_cast<Rocket*>(this) == nullptr) {
      std::shared_ptr<Portal> portal = std::dynamic_pointer_cast<Portal>(
          objects[static_cast<unsigned>(new_cell_x)]
                 [static_cast<unsigned>(new_cell_y)]);

      Coordinates cells = GetNewPortalCells(
          portal->GetNewCellX(), portal->GetNewCellY(), new_cell_x, new_cell_y);

      if (map_->GetField(cells.x, cells.y) == CellType::Wall) {
        return;
      }
      for (const auto& object : tanks) {
        if (object->GetCellX() == cells.x && object->GetCellY() == cells.y) {
          return;
        }
      }
      objects_copies_.append({shared_from_this(), {cells.x, cells.y}});
      copy_existence_ = true;

      new_cell_x = cells.x;
      new_cell_y = cells.y;
    }
  } else if (objects[static_cast<unsigned>(new_cell_x)]
                    [static_cast<unsigned>(new_cell_y)] != nullptr) {
    if (std::dynamic_pointer_cast<Obstacle>(objects[static_cast<unsigned>(
            new_cell_x)][static_cast<unsigned>(new_cell_y)]) != nullptr) {
      current_speed_ *= 2;
    }
    if (dynamic_cast<Tank*>(this) != nullptr) {
      Tank* tank = dynamic_cast<Tank*>(this);
      if (std::dynamic_pointer_cast<MedicalKit>(objects[static_cast<unsigned>(
              new_cell_x)][static_cast<unsigned>(new_cell_y)]) != nullptr) {
        tank->PlusHealth(
            std::min(35, tank->GetMaxHealth() - tank->GetCurrentHealth()));
      } else if (std::dynamic_pointer_cast<Charge>(
                     objects[static_cast<unsigned>(new_cell_x)]
                            [static_cast<unsigned>(new_cell_y)]) != nullptr) {
        tank->PlusCharge(
            0, std::min(10, tank->GetMaxCharge(0) - tank->GetCurrentCharge(0)));
      }
    }
    if (dynamic_cast<Rocket*>(this) != nullptr) {
      Rocket* rocket = dynamic_cast<Rocket*>(this);
      cells_to_finish_movement_ = 0;
      if (rocket->GetTypeOfRocket() != TypeOfRocket::HardRocket) {
        objects[static_cast<unsigned>(new_cell_x)]
               [static_cast<unsigned>(new_cell_y)] = nullptr;
      }
      return;
    }
    objects[static_cast<unsigned>(new_cell_x)]
           [static_cast<unsigned>(new_cell_y)] = nullptr;
  }

  new_cell_x = old_cell_x;
  new_cell_y = old_cell_y;
  cell_x_ = new_cell_x;
  cell_y_ = new_cell_y;
  time_to_finish_movement_ += current_speed_;
  cells_to_finish_movement_ = number_of_cells - 1;
}

void Movable::Move(const int milliseconds_passed) {
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

void Movable::Rotate(const int milliseconds_passed) {
  time_to_finish_rotation_ -= milliseconds_passed;
}

void Movable::TurnRotationReverseOn() { rotate_reverse_ = -1; }

void Movable::TurnRotationReverseOff() { rotate_reverse_ = 1; }

void Movable::UpdateCoordinates(const int cell_x, const int cell_y) {
  if ((cell_x_ != cell_x || cell_y_ != cell_y) &&
      GetTimeToFinishMovement() == 0) {
    cell_x_ = cell_x;
    cell_y_ = cell_y;
    copy_existence_ = false;
  }

  cur_width_ = map_->GetCellWidth();
  cur_height_ = map_->GetCellHeight();

  double movement_proportion =
      static_cast<double>(time_to_finish_movement_) / current_speed_;
  if (copy_existence_ && cell_x_ == cell_x && cell_y_ == cell_y) {
    opacity_ = movement_proportion;
    prev_opacity_ = opacity_;
  } else if (copy_existence_) {
    opacity_ = 1 - movement_proportion;
  }

  prev_upper_left_x_ = cur_upper_left_x_;
  prev_upper_left_y_ = cur_upper_left_y_;
  cur_upper_left_x_ =
      map_->GetUpperLeftX() + (cur_width_ * cell_x) -
      reverse_ *
          static_cast<int>((directions_[1] * cur_width_ * movement_proportion) -
                           (directions_[3] * cur_width_ * movement_proportion));

  cur_upper_left_y_ =
      map_->GetUpperLeftY() + (cur_height_ * cell_y) -
      reverse_ * static_cast<int>(
                     (directions_[2] * cur_height_ * movement_proportion) -
                     (directions_[0] * cur_height_ * movement_proportion));

  if (map_->GetField(cell_x_, cell_y_) == CellType::Forest) {
    if (movement_proportion <= 0.5) {
      opacity_ = 0.5;
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
  cur_upper_left_x_ = prev_upper_left_x_;
  cur_upper_left_y_ = prev_upper_left_y_;
  opacity_ = prev_opacity_;
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
  return (GetTimeToFinishMovement() > 0 || GetTimeToFinishRotation() != 0 ||
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

Coordinates Movable::GetNewPortalCells(int portal_cell_x, int portal_cell_y,
                                       int new_cell_x, int new_cell_y) {
  Coordinates cells = {portal_cell_x, portal_cell_y};
  if (new_cell_y - cell_y_ == -1) {
    cells.y--;
  } else if (new_cell_y - cell_y_ == 1) {
    cells.y++;
  } else if (new_cell_x - cell_x_ == -1) {
    cells.x--;
  } else {
    cells.x++;
  }
  return cells;
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
  if (scaled_image_.width() == cur_width_ &&
      scaled_image_.height() == cur_height_) {
    return;
  }
  scaled_image_ = image_.scaled(cur_width_, cur_height_, Qt::KeepAspectRatio);
}
