#include "improvedbot.h"

ImprovedBot::ImprovedBot(std::shared_ptr<Map>& map, BotQualities qualities,
                         Direction direction)
    : Bot(map, qualities, direction) {
  LoadImage();
};

void ImprovedBot::LoadImage() {
  image_.load(":/textures/improved_bot.png");
  scaled_image_ = image_;
}

bool ImprovedBot::IsRotationStartNeeded(std::shared_ptr<Tank> tank) {
  if (time_to_finish_rotation_ <= 0 && time_to_finish_movement_ <= 0) {
    if (number_of_turns_ > 0) {
      number_of_turns_--;
      return number_of_turns_ > 0 ? true : false;
    }
    if (number_of_cells_to_move_ == 0) {
      if (IsShotNeeded(map_, tank)) {
        return false;
      }
      if (qrand() % 2 == 0) {
        TurnRotationReverseOn();
      } else {
        TurnRotationReverseOff();
      }
      number_of_turns_ = amount_of_turns_;
      return true;
    }
  }
  return false;
}

bool ImprovedBot::CheckDirection(int& tank, int& bot, int direction) {
  if (tank > bot) {
    std::swap(tank, bot);
    if (direction == 0 || direction == 3) {
      number_of_cells_to_move_ = 0;
      number_of_turns_ = 3;
      return false;
    }
  } else if (direction == 2 || direction == 1) {
    number_of_cells_to_move_ = 0;
    number_of_turns_ = 3;
    return false;
  }
  return true;
}
