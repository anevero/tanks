#include "static_object.h"

#include "../game_core/constants.h"

StaticObject::StaticObject(
    std::shared_ptr<const Map> map, const QString& path,
    Coordinates coordinates)
    : GameObject(std::move(map), path, coordinates) {}

void StaticObject::Draw(QPainter* painter) {
  RescaleImage();
  painter->save();
  painter->translate(
      upper_left_cell_coordinates_.x + width_ / 2,
      upper_left_cell_coordinates_.y + height_ / 2);
  if (map_->GetField(cell_) == CellType::Forest) {
    painter->setOpacity(constants::kOpacityLevel);
  }
  painter->drawPixmap(-width_ / 2, -height_ / 2, scaled_pixmap_);
  painter->restore();
}

void StaticObject::UpdateCoordinates() {
  width_ = map_->GetWidth() / map_->GetNumberOfCellsHorizontally();
  height_ = map_->GetHeight() / map_->GetNumberOfCellsVertically();
  upper_left_cell_coordinates_.x =
      map_->GetUpperLeftCellCoordinates().x + cell_.x * width_;
  upper_left_cell_coordinates_.y =
      map_->GetUpperLeftCellCoordinates().y + cell_.y * height_;

  RescaleImage();
}

MedicalKit::MedicalKit(std::shared_ptr<const Map> map, Coordinates coordinates)
    : StaticObject(std::move(map), ":/textures/medicalkit.png", coordinates) {}

Obstacle::Obstacle(std::shared_ptr<const Map> map, Coordinates coordinates)
    : StaticObject(std::move(map), ":/textures/log.png", coordinates) {}

Charge::Charge(std::shared_ptr<const Map> map, Coordinates coordinates)
    : StaticObject(std::move(map), ":/textures/charge.png", coordinates) {}
