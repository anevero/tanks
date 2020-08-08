#include "clever_bot.h"

#include <queue>

CleverBot::CleverBot(
    std::shared_ptr<const Map> map, const QString& texture_path,
    Coordinates initial_cell, TankParameters tank_parameters,
    BotParameters bot_parameters, Direction direction)
    : ImprovedBot(std::move(map), texture_path, initial_cell,
                  tank_parameters, bot_parameters, direction) {
  height_ = map_->GetNumberOfCellsVertically();
  width_ = map_->GetNumberOfCellsHorizontally();
  distance_.resize(width_);
  for (int i = 0; i < height_; ++i) {
    for (int j = 0; j < width_; ++j) {
      distance_[j].push_back(height_ * width_);
    }
  }
}

bool CleverBot::IsRotationStartNeeded(
    const std::shared_ptr<const Tank>& tank) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (current_number_of_turns_ > 0) {
      current_number_of_turns_--;
      return current_number_of_turns_ > 0;
    }
    if (map_->GetField(tank->GetCoordinates()) == CellType::Forest) {
      return Bot::IsRotationStartNeeded(tank);
    }
  }
  return false;
}

bool CleverBot::IsMovingStartNeeded(
    const std::list<std::shared_ptr<Tank>>& objects,
    const std::vector<std::vector<std::shared_ptr<StaticObject>>>& portals) {
  auto tank = objects.front();
  if (time_to_finish_movement_ <= 0 && time_to_finish_rotation_ <= 0) {
    Bfs(objects, portals, tank->GetCoordinates());

    int direction = GetDirectionAsInt();
    int delta_x = 0;
    int delta_y = 0;
    if (direction == 0) {
      delta_y = 1;
    } else if (direction == 2) {
      delta_y = -1;
    } else if (direction == 3) {
      delta_x = 1;
    } else if (direction == 1) {
      delta_x = -1;
    }

    auto current_cell = GetCoordinates();
    if (map_->GetField(tank->GetCoordinates()) == CellType::Forest) {
      return Bot::IsMovingStartNeeded(objects, portals);
    } else if (distance_[current_cell.x - delta_x][current_cell.y - delta_y]
        == distance_[current_cell.x][current_cell.y] - 1) {
      if (distance_[current_cell.x][current_cell.y] - 1
          == distance_[tank->GetCoordinates().x][tank->GetCoordinates().y]) {
        return false;
      }
      number_of_cells_to_move_ = 1;
      return true;
    } else if (distance_[current_cell.x - delta_y][current_cell.y - delta_x]
        == distance_[current_cell.x][current_cell.y] - 1) {
      MaybeChangeRotationDirection(delta_x, false);
      current_number_of_turns_ = 2;
      return false;
    } else if (distance_[current_cell.x + delta_y][current_cell.y + delta_x]
        == distance_[current_cell.x][current_cell.y] - 1) {
      MaybeChangeRotationDirection(delta_x, true);
      current_number_of_turns_ = 2;
      return false;
    } else if (distance_[current_cell.x + delta_x][current_cell.y + delta_y]
        == distance_[current_cell.x][current_cell.y] - 1) {
      current_number_of_turns_ = 3;
      return false;
    }
  }
  return false;
}

void CleverBot::MaybeChangeRotationDirection(int delta_x, bool condition) {
  if ((delta_x == 0) ^ condition) {
    TurnRotationReverseOn();
  } else {
    TurnRotationReverseOff();
  }
}

void CleverBot::Bfs(
    const std::list<std::shared_ptr<Tank>>& objects,
    const std::vector<std::vector<std::shared_ptr<StaticObject>>>& portals,
    Coordinates cell) {
  std::queue<CellInfo> cells;
  cells.emplace(cell, cell, 0);

  for (int i = 0; i < height_; ++i) {
    for (int j = 0; j < width_; ++j) {
      distance_[j][i] = height_ * width_;
    }
  }

  while (!cells.empty()) {
    cell = cells.front().cell;
    auto previous_cell = cells.front().previous_cell;
    int current_distance = cells.front().distance;
    cells.pop();

    if (cell.x <= 0 || cell.x >= width_ || cell.y <= 0 || cell.y >= height_) {
      continue;
    }
    if (map_->GetField(cell) == CellType::Wall) {
      continue;
    }
    if (distance_[cell.x][cell.y] <= current_distance) {
      continue;
    }
    if (cell == GetCoordinates()) {
      distance_[cell.x][cell.y] = current_distance;
      break;
    }

    bool bad_cell = false;
    for (const auto& object : objects) {
      if (cell == GetCoordinates() && object != objects.front()) {
        bad_cell = true;
        break;
      }
    }
    if (bad_cell) {
      continue;
    }

    if (std::dynamic_pointer_cast<Portal>(portals[cell.x][cell.y]) != nullptr) {
      std::shared_ptr<Portal>
          portal = std::dynamic_pointer_cast<Portal>(portals[cell.x][cell.y]);
      auto portal_cell = portal->GetNewCell();
      distance_[portal_cell.x][portal_cell.y] = current_distance - 1;

      if (cell.x - previous_cell.x == 1) {
        portal_cell.x++;
      } else if (cell.x - previous_cell.x == -1) {
        portal_cell.x--;
      } else if (cell.y - previous_cell.y == 1) {
        portal_cell.y++;
      } else {
        portal_cell.y--;
      }

      cells.emplace(portal_cell, portal->GetNewCell(), current_distance);
      continue;
    }

    distance_[cell.x][cell.y] = current_distance;
    cells.emplace(Coordinates(cell.x + 1, cell.y), cell, current_distance + 1);
    cells.emplace(Coordinates(cell.x - 1, cell.y), cell, current_distance + 1);
    cells.emplace(Coordinates(cell.x, cell.y + 1), cell, current_distance + 1);
    cells.emplace(Coordinates(cell.x, cell.y - 1), cell, current_distance + 1);
  }
}
