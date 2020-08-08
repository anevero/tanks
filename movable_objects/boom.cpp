#include "boom.h"

Boom::Boom(std::shared_ptr<const Map> map,
           const std::shared_ptr<Movable>& tank, int speed)
    : Movable(std::move(map), ":/textures/boom.png", tank->GetCoordinates(),
              tank->GetDirection(), speed),
      boom_sound_() {
  opacity_ = 0.85;

  boom_sound_.setMedia(QUrl("qrc:/sounds/boom.mp3"));
  boom_sound_.play();
}

void Boom::Draw(QPainter* painter) {
  painter->save();
  painter->setOpacity(opacity_);
  painter->translate(
      upper_left_cell_coordinates_.x + width_ / 2,
      upper_left_cell_coordinates_.y + height_ / 2);
  painter->drawPixmap(-width_ / 2,
                      -height_ / 2,
                      scaled_pixmap_);
  painter->restore();
}

void Boom::StartMovement(
    const std::list<std::shared_ptr<Tank>>&,
    std::list<std::pair<std::shared_ptr<Tank>, Coordinates>>*,
    std::vector<std::vector<std::shared_ptr<StaticObject>>>*,
    int number_of_cells) {
  time_to_finish_movement_ = current_speed_;
  cells_to_finish_movement_ = number_of_cells - 1;
}

void Boom::UpdateCoordinates(Coordinates) {
  int current_cell_width =
      map_->GetWidth() / map_->GetNumberOfCellsHorizontally();
  int current_cell_height =
      map_->GetHeight() / map_->GetNumberOfCellsVertically();

  double movement_proportion = cells_to_finish_movement_ + 1
      - 1.0 * time_to_finish_movement_ / current_speed_;

  width_ =
      current_cell_width + 2 * current_cell_width * movement_proportion;
  height_ =
      current_cell_height + 2 * current_cell_height * movement_proportion;

  upper_left_cell_coordinates_.x = map_->GetUpperLeftCellCoordinates().x
      + current_cell_width * (cell_.x - movement_proportion);
  upper_left_cell_coordinates_.y = map_->GetUpperLeftCellCoordinates().y
      + current_cell_height * (cell_.y - movement_proportion);

  RescaleImage();
}
