#include "improvedbot.h"

ImprovedBot::ImprovedBot(const std::shared_ptr<Map>& map,
                         const BotQualities& qualities)
    : Bot(map, qualities) {
  LoadImage(":/textures/improved_bot.png");
}

bool ImprovedBot::IsRotationStartNeeded(const std::shared_ptr<Tank>& tank) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (number_of_turns_ > 0) {
      number_of_turns_--;
      return true;
    }
    if (number_of_cells_to_move_ == 0) {
      if (IsShotNeeded(map_, tank)) {
        return false;
      }
      if (qrand() % side_rotation_frequency_ == 0) {
        TurnRotationReverseOn();
      } else {
        TurnRotationReverseOff();
      }
      number_of_turns_ = amount_of_turns_;
      number_of_turns_--;
      return true;
    }
  }
  return false;
}

bool ImprovedBot::IsShotNeeded(const std::shared_ptr<Map>& map,
                               const std::shared_ptr<Tank>& tank) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    int direction = GetIntDirection();
    int tank_x = tank->GetCellX();
    int tank_y = tank->GetCellY();
    int bot_x = GetCellX();
    int bot_y = GetCellY();
    if (map_->GetField(tank_x, tank_y) == CellType::Forest) {
      return false;
    }

    if (tank_x == bot_x) {
      if (IsWallBetweenObjectsX(map, tank_x, tank_y, bot_x, bot_y)) {
        return false;
      }

      if (tank_y > bot_y) {
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
      return CheckDirection(tank_y, bot_y, direction);
    }

    if (tank_y == bot_y) {
      if (IsWallBetweenObjectsY(map, tank_x, tank_y, bot_x, bot_y)) {
        return false;
      }

      if (tank_x > bot_x) {
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
      return CheckDirection(tank_x, bot_x, direction);
    }
  }
  return false;
}

bool ImprovedBot::CheckDirection(const int tank, const int bot,
                                 const int direction) {
  number_of_cells_to_move_ = 0;
  number_of_turns_ = 1;
  if (tank > bot) {
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
  number_of_turns_ = 2;
  return false;
}
