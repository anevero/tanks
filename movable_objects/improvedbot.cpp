#include "improvedbot.h"

ImprovedBot::ImprovedBot(std::shared_ptr<const Map> map,
                         Coordinates initial_cell,
                         TankParameters tank_parameters,
                         BotParameters bot_parameters,
                         Direction direction)
    : Bot(std::move(map), initial_cell,
          tank_parameters, bot_parameters, direction) {
  LoadImage(":/textures/improved_bot.png");
}

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
      if (random_generator_() % side_rotation_frequency_ == 0) {
        TurnRotationReverseOn();
      } else {
        TurnRotationReverseOff();
      }
      current_number_of_turns_ = number_of_turns_;
      current_number_of_turns_--;
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

      if (tank_cell.y > bot_cell.y) {
        if (direction == 2) {
          return true;
        } else if (direction == 0) {
          return ChangeDirection();
        }
      } else if (direction == 0) {
        return true;
      } else if (direction == 2) {
        return ChangeDirection();
      }
      return CheckDirection(tank_cell.y, bot_cell.y, direction);
    }

    if (tank_cell.y == bot_cell.y) {
      if (IsWallBetweenTankAndBotVertically(tank_cell, bot_cell)) {
        return false;
      }

      if (tank_cell.x > bot_cell.x) {
        if (direction == 1) {
          return true;
        } else if (direction == 3) {
          return ChangeDirection();
        }
      } else if (direction == 3) {
        return true;
      } else if (direction == 1) {
        return ChangeDirection();
      }
      return CheckDirection(tank_cell.x, bot_cell.x, direction);
    }
  }
  return false;
}

bool ImprovedBot::CheckDirection(
    int tank_coordinate, int bot_coordinate, int direction) {
  number_of_cells_to_move_ = 0;
  current_number_of_turns_ = 1;
  if (tank_coordinate > bot_coordinate) {
    if (direction == 0 || direction == 1) {
      TurnRotationReverseOff();
    } else {
      TurnRotationReverseOn();
    }
  } else {
    if (direction == 0 || direction == 1) {
      TurnRotationReverseOn();
    } else {
      TurnRotationReverseOff();
    }
  }
  return false;
}

bool ImprovedBot::ChangeDirection() {
  number_of_cells_to_move_ = 0;
  current_number_of_turns_ = 2;
  return false;
}
