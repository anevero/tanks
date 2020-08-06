#include "objectonmap.h"

#include "../game_core/constants.h"

ObjectOnMap::ObjectOnMap(
    std::shared_ptr<const Map> map, Coordinates coordinates)
    : map_(std::move(map)), coordinates_(coordinates), width_(0), height_(0) {}

void ObjectOnMap::LoadImage(const QString& path) {
  image_.load(path);
  scaled_pixmap_ = QPixmap::fromImage(image_);
}

void ObjectOnMap::Draw(QPainter* painter) {
  RescaleImage();
  painter->save();
  painter->translate(
      current_upper_left_cell_coordinates_.x + width_ / 2,
      current_upper_left_cell_coordinates_.y + height_ / 2);
  if (map_->GetField(coordinates_) == CellType::Forest) {
    painter->setOpacity(constants::kOpacityLevel);
  }
  painter->drawPixmap(-width_ / 2, -height_ / 2, scaled_pixmap_);
  painter->restore();
}

void ObjectOnMap::RescaleImage() {
  if (scaled_pixmap_.width() == width_ && scaled_pixmap_.height() == height_) {
    return;
  }
  scaled_pixmap_ = QPixmap::fromImage(
      image_.scaled(width_, height_, Qt::KeepAspectRatio));
}

void ObjectOnMap::UpdateCoordinates() {
  width_ = map_->GetWidth() / map_->GetNumberOfCellsHorizontally();
  height_ = map_->GetHeight() / map_->GetNumberOfCellsVertically();
  current_upper_left_cell_coordinates_.x =
      map_->GetUpperLeftCellCoordinates().x + coordinates_.x * width_;
  current_upper_left_cell_coordinates_.y =
      map_->GetUpperLeftCellCoordinates().y + coordinates_.y * height_;

  RescaleImage();
}

Coordinates ObjectOnMap::GetCoordinates() const {
  return coordinates_;
}

MedicalKit::MedicalKit(std::shared_ptr<const Map> map, Coordinates coordinates)
    : ObjectOnMap(std::move(map), coordinates) {
  LoadImage(":/textures/medicalkit.png");
}

Obstacle::Obstacle(std::shared_ptr<const Map> map, Coordinates coordinates)
    : ObjectOnMap(std::move(map), coordinates) {
  LoadImage(":/textures/log.png");
}

Charge::Charge(std::shared_ptr<const Map> map, Coordinates coordinates)
    : ObjectOnMap(std::move(map), coordinates) {
  LoadImage(":/textures/charge.png");
}
