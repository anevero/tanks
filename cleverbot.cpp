#include "cleverbot.h"

CleverBot::CleverBot(std::shared_ptr<Map>& map, BotQualities qualities,
                     Direction direction)
    : ImprovedBot(map, qualities, direction) {
  LoadImage();
  int height = map_->GetNumberOfCellsVertically();
  int width = map_->GetNumberOfCellsHorizontally();
  distance_.resize(width);
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      distance_[j].push_back(height * width);
    }
  }
}

void CleverBot::LoadImage() {
  image_.load(":/textures/clever_bot.png");
  scaled_image_ = image_;
}

bool CleverBot::IsRotationStartNeeded(std::shared_ptr<Tank>) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (number_of_turns_ > 0) {
      number_of_turns_--;
      return number_of_turns_ > 0;
    }
  }
  return false;
}

bool CleverBot::IsMovingStartNeeded(const QList<std::shared_ptr<Movable>>& objects) {
  auto tank = objects[0];
  if (time_to_finish_movement_ <= 0 && time_to_finish_rotation_ <= 0) {
    Bfs(objects, tank->GetCellX(), tank->GetCellY());
    int direction = GetIntDirection();
    int cell_x = GetCellX();
    int cell_y = GetCellY();

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

    if (distance_[cell_x - delta_x][cell_y - delta_y]
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

void CleverBot::Bfs(const QList<std::shared_ptr<Movable>> objects,
                    int cell_x, int cell_y) {
  QQueue<CellInfo> cells;
  cells.push_back({cell_x, cell_y, 0});

  int height = map_->GetNumberOfCellsVertically();
  int width = map_->GetNumberOfCellsHorizontally();
  for (int i = 0; i < height; ++i) {
    for (int j = 0; j < width; ++j) {
      distance_[j][i] = height * width;
    }
  }

  while (!cells.empty()) {
    cell_x = cells.front().cell_x;
    cell_y = cells.front().cell_y;
    int current_distance = cells.front().distance;
    cells.pop_front();
    if (cell_x < 0 || cell_x >= width ||
        cell_y < 0 || cell_y >= height) {
      continue;
    }
    if (map_->GetField(cell_x, cell_y) == CellType::Wall) {
      continue;
    }
    if (distance_[cell_x][cell_y] <= current_distance) {
      continue;
    }
    if (cell_x == GetCellX() && cell_y == GetCellY()) {
      distance_[cell_x][cell_y] = current_distance;
      break;
    }

    bool bad_cell = false;
    for (const auto& object : objects) {
      if (object->GetCellX() == cell_x && object->GetCellY() == cell_y &&
          object != objects[0]) {
        bad_cell = true;
        break;
      }
    }
    if (bad_cell) { continue; }

    distance_[cell_x][cell_y] = current_distance;
    cells.push_back({cell_x + 1, cell_y, current_distance + 1});
    cells.push_back({cell_x - 1, cell_y, current_distance + 1});
    cells.push_back({cell_x, cell_y + 1, current_distance + 1});
    cells.push_back({cell_x, cell_y - 1, current_distance + 1});
  }
}
