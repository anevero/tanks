#include "bot.h"

Bot::Bot(std::shared_ptr<const Map> map, Coordinates initial_cell,
         TankParameters tank_parameters, BotParameters bot_parameters,
         Direction direction)
    : Tank(std::move(map), initial_cell, tank_parameters, direction),
      moving_length_(bot_parameters.moving_length),
      number_of_turns_(bot_parameters.number_of_turns),
      side_rotation_frequency_(bot_parameters.side_rotation_frequency) {
  LoadImage(":/textures/bot.png");
}

bool Bot::IsTurnNeeded() const {
  return time_to_finish_rotation_ > 0;
}

bool Bot::IsRotationStartNeeded(const std::shared_ptr<const Tank>&) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (current_number_of_turns_ > 0) {
      current_number_of_turns_--;
      return current_number_of_turns_ > 0;
    }
    if (number_of_cells_to_move_ == 0) {
      TryToChangeRotationDirectionAndRotate();
      return true;
    }
  }
  return false;
}

bool Bot::IsMovingStartNeeded(
    const std::list<std::shared_ptr<Tank>>&,
    const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>&) {
  if (time_to_finish_movement_ <= 0 && time_to_finish_rotation_ <= 0) {
    if (number_of_cells_to_move_ == 0) {
      if (current_number_of_turns_ == 0) {
        number_of_cells_to_move_ = moving_length_;
      } else {
        return false;
      }
    } else {
      number_of_cells_to_move_--;
    }
    return number_of_cells_to_move_ > 0;
  }
  return false;
}

bool Bot::IsShotNeeded(const std::shared_ptr<const Tank>& tank) {
  if (time_to_finish_rotation_ == 0 && time_to_finish_movement_ == 0) {
    int direction = GetDirectionAsInt();
    Coordinates tank_cell = tank->GetCoordinates();
    Coordinates bot_cell = GetCoordinates();
    if (map_->GetField(tank_cell) == CellType::Forest) {
      return false;
    }

    if (direction == 0 || direction == 2) {
      if (tank_cell.x == bot_cell.x) {
        if (IsWallBetweenTankAndBotHorizontally(tank_cell, bot_cell)) {
          return false;
        }

        if (CheckDirection(tank_cell.y, bot_cell.y, direction)) {
          return true;
        }
      }
    }
    if (direction == 1 || direction == 3) {
      if (tank_cell.y == bot_cell.y) {
        if (IsWallBetweenTankAndBotVertically(tank_cell, bot_cell)) {
          return false;
        }

        if (CheckDirection(tank_cell.x, bot_cell.x, direction)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Bot::CheckDirection(int tank_coordinate, int bot_coordinate,
                         int direction) {
  if (tank_coordinate > bot_coordinate) {
    if (direction == 0 || direction == 3) {
      return false;
    }
  } else if (direction == 2 || direction == 1) {
    return false;
  }
  return true;
}

void Bot::TryToChangeRotationDirectionAndRotate() {
  if (random_generator_() % side_rotation_frequency_ == 0) {
    TurnRotationReverseOn();
  } else {
    TurnRotationReverseOff();
  }
  current_number_of_turns_ = number_of_turns_;
  current_number_of_turns_--;
}

bool Bot::IsWallBetweenTankAndBotHorizontally(
    Coordinates tank_cell, Coordinates bot_cell) const {
  return (map_->GetWallsPrecalculation(bot_cell.x, bot_cell.y) +
      map_->GetWallsPrecalculation(tank_cell.x - 1, tank_cell.y - 1)) -
      map_->GetWallsPrecalculation(tank_cell.x, tank_cell.y - 1) -
      map_->GetWallsPrecalculation(bot_cell.x - 1, bot_cell.y) != 0;
}

bool Bot::IsWallBetweenTankAndBotVertically(
    Coordinates tank_cell, Coordinates bot_cell) const {
  return (map_->GetWallsPrecalculation(bot_cell.x, bot_cell.y) +
      map_->GetWallsPrecalculation(tank_cell.x - 1, tank_cell.y - 1)) -
      map_->GetWallsPrecalculation(tank_cell.x - 1, tank_cell.y) -
      map_->GetWallsPrecalculation(bot_cell.x, bot_cell.y - 1) != 0;
}
