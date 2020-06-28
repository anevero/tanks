#include "cleverbot.h"

CleverBot::CleverBot(
    std::shared_ptr<Map> map, int init_cell_x, int init_cell_y,
    TankParameters tank_parameters, BotParameters bot_parameters,
    Direction direction)
    : ImprovedBot(std::move(map), init_cell_x, init_cell_y,
                  tank_parameters, bot_parameters, direction) {
  LoadImage(":/textures/clever_bot.png");
  height_ = map_->GetNumberOfCellsVertically();
  width_ = map_->GetNumberOfCellsHorizontally();
  distance_.resize(width_);
  for (int i = 0; i < height_; ++i) {
    for (int j = 0; j < width_; ++j) {
      distance_[j].push_back(height_ * width_);
    }
  }
}

bool CleverBot::IsRotationStartNeeded(const std::shared_ptr<Tank>& tank) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (number_of_turns_ > 0) {
      number_of_turns_--;
      return number_of_turns_ > 0;
    }
    if (map_->GetField(tank->GetCellX(), tank->GetCellY()) ==
        CellType::Forest) {
      return Bot::IsRotationStartNeeded(tank);
    }
  }
  return false;
}

bool CleverBot::IsMovingStartNeeded(
    const std::list<std::shared_ptr<Movable>>& objects,
    const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& portals) {
  auto tank = objects.front();
  if (time_to_finish_movement_ <= 0 && time_to_finish_rotation_ <= 0) {
    Bfs(objects, portals, tank->GetCellX(), tank->GetCellY());
    int direction = GetIntDirection();
    auto cell_x = static_cast<int>(GetCellX());
    auto cell_y = static_cast<int>(GetCellY());

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

    if (map_->GetField(tank->GetCellX(), tank->GetCellY())
        == CellType::Forest) {
      return Bot::IsMovingStartNeeded(objects, portals);
    } else if (distance_[cell_x - delta_x][cell_y - delta_y]
        == distance_[cell_x][cell_y] - 1) {
      if (distance_[cell_x][cell_y] - 1
          == distance_[tank->GetCellX()][tank->GetCellY()]) {
        return false;
      }
      number_of_cells_to_move_ = 1;
      return true;
    } else if (distance_[cell_x - delta_y][cell_y - delta_x]
        == distance_[cell_x][cell_y] - 1) {
      if (delta_x == 0) {
        TurnRotationReverseOn();
      } else {
        TurnRotationReverseOff();
      }
      number_of_turns_ = 2;
      return false;
    } else if (distance_[cell_x + delta_y][cell_y + delta_x]
        == distance_[cell_x][cell_y] - 1) {
      if (delta_x == 0) {
        TurnRotationReverseOff();
      } else {
        TurnRotationReverseOn();
      }
      number_of_turns_ = 2;
      return false;
    } else if (distance_[cell_x + delta_x][cell_y + delta_y]
        == distance_[cell_x][cell_y] - 1) {
      number_of_turns_ = 3;
      return false;
    }
  }
  return false;
}

void CleverBot::Bfs(
    const std::list<std::shared_ptr<Movable>>& objects,
    const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& portals,
    int cell_x, int cell_y) {
  QQueue<CellInfo> cells;
  cells.push_back({cell_x, cell_y, cell_x, cell_y, 0});

  for (int i = 0; i < height_; ++i) {
    for (int j = 0; j < width_; ++j) {
      distance_[j][i] = height_ * width_;
    }
  }

  while (!cells.empty()) {
    cell_x = cells.front().cell_x;
    cell_y = cells.front().cell_y;
    int prev_cell_x = cells.front().prev_cell_x;
    int prev_cell_y = cells.front().prev_cell_y;
    int current_distance = cells.front().distance;
    cells.pop_front();
    if (cell_x <= 0 || cell_x >= width_ || cell_y <= 0 || cell_y >= height_) {
      continue;
    }
    if (map_->GetField(cell_x, cell_y) == CellType::Wall) {
      continue;
    }
    if (distance_[cell_x][cell_y] <= current_distance) {
      continue;
    }
    if (cell_x == static_cast<int>(GetCellX()) &&
        cell_y == static_cast<int>(GetCellY())) {
      distance_[cell_x][cell_y] = current_distance;
      break;
    }

    bool bad_cell = false;
    for (const auto& object : objects) {
      if (cell_x == static_cast<int>(object->GetCellX()) &&
          cell_y == static_cast<int>(object->GetCellY()) &&
          object != objects.front()) {
        bad_cell = true;
        break;
      }
    }
    if (bad_cell) {
      continue;
    }

    if (std::dynamic_pointer_cast<Portal>(portals[cell_x][cell_y]) != nullptr) {
      std::shared_ptr<Portal>
          portal = std::dynamic_pointer_cast<Portal>(portals[cell_x][cell_y]);
      distance_[portal->GetNewCellX()][portal->GetNewCellY()] =
          current_distance - 1;
      int portal_cell_x = portal->GetNewCellX();
      int portal_cell_y = portal->GetNewCellY();
      if (cell_x - prev_cell_x == 1) {
        portal_cell_x++;
      } else if (cell_x - prev_cell_x == -1) {
        portal_cell_x--;
      } else if (cell_y - prev_cell_y == 1) {
        portal_cell_y++;
      } else {
        portal_cell_y--;
      }

      cells.push_back({portal_cell_x,
                       portal_cell_y,
                       static_cast<int>(portal->GetX()),
                       static_cast<int>(portal->GetY()),
                       current_distance});
      continue;
    }

    distance_[cell_x][cell_y] = current_distance;
    cells.push_back({cell_x + 1, cell_y, cell_x, cell_y, current_distance + 1});
    cells.push_back({cell_x - 1, cell_y, cell_x, cell_y, current_distance + 1});
    cells.push_back({cell_x, cell_y + 1, cell_x, cell_y, current_distance + 1});
    cells.push_back({cell_x, cell_y - 1, cell_x, cell_y, current_distance + 1});
  }
}
