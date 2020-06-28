#include "boom.h"

Boom::Boom(std::shared_ptr<Map> map, const std::shared_ptr<Movable>& tank,
           int speed)
    : Movable(std::move(map), tank->GetCellX(), tank->GetCellY(),
              tank->GetDirection(), speed),
      boom_sound_() {
  LoadImage(":/textures/boom.png");
  opacity_ = 0.85;

  boom_sound_.setMedia(QUrl("qrc:/sounds/boom.mp3"));
  boom_sound_.play();
}

void Boom::Draw(QPainter* painter) {
  painter->save();
  painter->setOpacity(opacity_);
  painter->translate(current_upper_left_x_ + current_width_ / 2,
                     current_upper_left_y_ + current_height_ / 2);
  painter->drawPixmap(-current_width_ / 2,
                      -current_height_ / 2,
                      scaled_pixmap_);
  painter->restore();
}

void Boom::StartMovement(
    const int number_of_cells, const std::list<std::shared_ptr<Movable>>&,
    std::list<std::pair<std::shared_ptr<Movable>, Coordinates>>*,
    std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>*) {
  time_to_finish_movement_ = current_speed_;
  cells_to_finish_movement_ = number_of_cells - 1;
}

void Boom::UpdateCoordinates(int, int) {
  int cur_cell_width = map_->GetWidth() / map_->GetNumberOfCellsHorizontally();
  int cur_cell_height = map_->GetHeight() / map_->GetNumberOfCellsVertically();

  double movement_proportion = cells_to_finish_movement_ + 1
      - 1.0 * time_to_finish_movement_ / current_speed_;

  current_width_ = cur_cell_width + 2 * cur_cell_width * movement_proportion;
  current_height_ = cur_cell_height + 2 * cur_cell_height * movement_proportion;

  current_upper_left_x_ = map_->GetUpperLeftX() + (cur_cell_width * cell_x_) -
      cur_cell_width * movement_proportion;
  current_upper_left_y_ = map_->GetUpperLeftY() + (cur_cell_height * cell_y_) -
      cur_cell_height * movement_proportion;

  RescaleImage();
}
