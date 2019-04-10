#include "objectonmap.h"

ObjectOnMap::ObjectOnMap(std::shared_ptr<Map>& map, int x, int y)
    : map_(map), x_(x), y_(y) {}

void ObjectOnMap::Draw(QPainter& painter) {
  RescaleImage();
  painter.save();
  int cur_width =
      static_cast<int>(map_->GetWidth() / map_->GetNumberOfCellsHorizontally());
  int cur_height =
      static_cast<int>(map_->GetHeight() / map_->GetNumberOfCellsVertically());

  int cur_upper_left_x_ = map_->GetUpperLeftX() + x_ * cur_width;

  int cur_upper_left_y_ = map_->GetUpperLeftY() + y_ * cur_height;
  painter.translate(cur_upper_left_x_ + cur_width / 2,
                    cur_upper_left_y_ + cur_height / 2);
  painter.drawImage(-cur_width / 2, -cur_height / 2, scaled_image_);
  painter.restore();
}

void ObjectOnMap::RescaleImage() {
  if (scaled_image_.width() == map_->GetWidth() &&
      scaled_image_.height() == map_->GetHeight()) {
    return;
  }
  int cur_width =
      static_cast<int>(map_->GetWidth() / map_->GetNumberOfCellsHorizontally());
  int cur_height =
      static_cast<int>(map_->GetHeight() / map_->GetNumberOfCellsVertically());

  scaled_image_ = image_.scaled(cur_width, cur_height, Qt::KeepAspectRatio);
}

Bonus::Bonus(std::shared_ptr<Map>& map, int x, int y) : ObjectOnMap(map, x, y) {
  LoadImage();
}

void Bonus::LoadImage() {
  image_.load(":/textures/aptechka.png");
  scaled_image_ = image_;
}

Obstacle::Obstacle(std::shared_ptr<Map>& map, int x, int y)
    : ObjectOnMap(map, x, y) {
  LoadImage();
}

void Obstacle::LoadImage() {
  image_.load(":/textures/log.png");
  scaled_image_ = image_;
}
