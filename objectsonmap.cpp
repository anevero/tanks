#include "objectsonmap.h"

ObjectsOnMap::ObjectsOnMap(std::shared_ptr<Map>& map, int x, int y)
    : map_(map), x_map_object_(x), y_map_object_(y) {}

void ObjectsOnMap::Draw(QPainter& painter) {
  painter.save();
  painter.translate(map_->GetUpperLeftX() + map_->GetWidth() / 2,
                    map_->GetUpperLeftY() + map_->GetHeight() / 2);
  painter.drawImage(-map_->GetWidth() / 2, -map_->GetHeight() / 2,
                    scaled_image_);
  painter.restore();
}

void ObjectsOnMap::RescaleImage() {
  if (scaled_image_.width() == map_->GetWidth() &&
      scaled_image_.height() == map_->GetHeight()) {
    return;
  }
  scaled_image_ =
      image_.scaled(map_->GetWidth(), map_->GetHeight(), Qt::KeepAspectRatio);
}

Bonus::Bonus(std::shared_ptr<Map>& map, int x, int y)
    : ObjectsOnMap(map, x, y) {
  LoadImage();
}

void Bonus::LoadImage() {
  image_.load(":/textures/aptechka.png");
  scaled_image_ = image_;
}

Obstacle::Obstacle(std::shared_ptr<Map>& map, int x, int y)
    : ObjectsOnMap(map, x, y) {
  LoadImage();
}

void Obstacle::LoadImage() {
  image_.load(":/textures/log.png");
  scaled_image_ = image_;
}
