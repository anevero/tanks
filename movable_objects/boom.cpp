#include "boom.h"

Boom::Boom(const std::shared_ptr<Map>& map,
           const std::shared_ptr<Movable>& tank, const int speed)
    : Movable(map, tank->GetCellX(), tank->GetCellY(), tank->GetDirection(),
              speed) {
  LoadImage(":/textures/boom.png");
  opacity_ = 0.85;

  boom_sound_.setMedia(QUrl("qrc:/sounds/boom.mp3"));
  boom_sound_.play();
}

void Boom::Draw(QPainter* painter) {
  painter->save();
  painter->setOpacity(opacity_);
  painter->translate(cur_upper_left_x_ + cur_width_ / 2,
                    cur_upper_left_y_ + cur_height_ / 2);
  painter->drawPixmap(-cur_width_ / 2, -cur_height_ / 2, scaled_pixmap_);
  painter->restore();
}

void Boom::StartMovement(
    const int number_of_cells, const QList<std::shared_ptr<Movable>>&,
    QList<QPair<std::shared_ptr<Movable>, Coordinates>>*,
    std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>*) {
  time_to_finish_movement_ = current_speed_;
  cells_to_finish_movement_ = number_of_cells - 1;
}

void Boom::UpdateCoordinates(size_t, size_t) {
  int cur_cell_width =
      static_cast<int>(map_->GetWidth() / map_->GetNumberOfCellsHorizontally());
  int cur_cell_height =
      static_cast<int>(map_->GetHeight() / map_->GetNumberOfCellsVertically());

  double movement_proportion = cells_to_finish_movement_ + 1
      - static_cast<double>(time_to_finish_movement_) / current_speed_;

  cur_width_ = cur_cell_width
      + 2 * static_cast<int>(cur_cell_width * movement_proportion);
  cur_height_ = cur_cell_height
      + 2 * static_cast<int>(cur_cell_height * movement_proportion);

  cur_upper_left_x_ =
      map_->GetUpperLeftX() + (cur_cell_width * static_cast<int>(cell_x_)) -
          static_cast<int>(cur_cell_width * movement_proportion);

  cur_upper_left_y_ =
      map_->GetUpperLeftY() + (cur_cell_height * static_cast<int>(cell_y_)) -
          static_cast<int>(cur_cell_height * movement_proportion);

  RescaleImage();
}