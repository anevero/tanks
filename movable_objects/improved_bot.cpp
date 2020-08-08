#include "improved_bot.h"

ImprovedBot::ImprovedBot(
    std::shared_ptr<const Map> map, const QString& texture_path,
    Coordinates initial_cell, TankParameters tank_parameters,
    BotParameters bot_parameters, Direction direction)
    : Bot(std::move(map), texture_path, initial_cell, tank_parameters,
          bot_parameters, direction) {}

bool ImprovedBot::IsRotationStartNeeded(
    const std::shared_ptr<const Tank>& tank) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (current_number_of_turns_ > 0) {
      current_number_of_turns_--;
      return true;
    }
    if (number_of_cells_to_move_ == 0) {
      if (IsShotNeeded(tank)) {
        return false;
      }
      TryToChangeRotationDirectionAndRotate();
      return true;
    }
  }
  return false;
}

bool ImprovedBot::IsShotNeeded(const std::shared_ptr<const Tank>& tank) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    int direction = GetDirectionAsInt();
    Coordinates tank_cell = tank->GetCoordinates();
    Coordinates bot_cell = GetCoordinates();
    if (map_->GetField(tank_cell) == CellType::Forest) {
      return false;
    }

    if (tank_cell.x == bot_cell.x) {
      if (IsWallBetweenTankAndBotHorizontally(tank_cell, bot_cell)) {
        return false;
      }
      return MaybeChangeDirection(tank_cell.y, bot_cell.y, direction, 0, 2);
    }

    if (tank_cell.y == bot_cell.y) {
      if (IsWallBetweenTankAndBotVertically(tank_cell, bot_cell)) {
        return false;
      }
      return MaybeChangeDirection(tank_cell.x, bot_cell.x, direction, 3, 1);
    }
  }
  return false;
}

bool ImprovedBot::CheckDirection(
    int tank_coordinate, int bot_coordinate, int direction) {
  number_of_cells_to_move_ = 0;
  current_number_of_turns_ = 1;
  MaybeChangeRotationDirection(direction, tank_coordinate <= bot_coordinate);
  return false;
}

bool ImprovedBot::MaybeChangeDirection(int tank_coordinate,
                                       int bot_coordinate,
                                       int current_direction,
                                       int first_direction,
                                       int second_direction) {
  if (tank_coordinate > bot_coordinate) {
    if (current_direction == second_direction) {
      return true;
    } else if (current_direction == first_direction) {
      return ChangeDirection();
    }
  } else if (current_direction == first_direction) {
    return true;
  } else if (current_direction == second_direction) {
    return ChangeDirection();
  }
  return CheckDirection(tank_coordinate, bot_coordinate, current_direction);
}

void ImprovedBot::MaybeChangeRotationDirection(int direction, bool condition) {
  if ((direction == 0 || direction == 1) ^ condition) {
    TurnRotationReverseOff();
  } else {
    TurnRotationReverseOn();
  }
}

bool ImprovedBot::ChangeDirection() {
  number_of_cells_to_move_ = 0;
  current_number_of_turns_ = 2;
  return false;
}
