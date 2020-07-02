#include "objectonmap.h"

ObjectOnMap::ObjectOnMap(std::shared_ptr<const Map> map, int x, int y)
    : map_(std::move(map)), x_(x), y_(y) {}

void ObjectOnMap::LoadImage(const QString& path) {
  image_.load(path);
  scaled_pixmap_ = QPixmap::fromImage(image_);
}

void ObjectOnMap::Draw(QPainter* painter) {
  RescaleImage();
  painter->save();
  painter->translate(cur_upper_left_x_ + cur_width_ / 2,
                     cur_upper_left_y_ + cur_height_ / 2);
  if (map_->GetField(x_, y_) == CellType::Forest) {
    painter->setOpacity(constants::kOpacityLevel);
  }
  painter->drawPixmap(-cur_width_ / 2, -cur_height_ / 2, scaled_pixmap_);
  painter->restore();
}

void ObjectOnMap::RescaleImage() {
  if (scaled_pixmap_.width() == cur_width_ &&
      scaled_pixmap_.height() == cur_height_) {
    return;
  }
  scaled_pixmap_ = QPixmap::fromImage(
      image_.scaled(cur_width_, cur_height_, Qt::KeepAspectRatio));
}

void ObjectOnMap::UpdateCoordinates() {
  cur_width_ = map_->GetWidth() / map_->GetNumberOfCellsHorizontally();
  cur_height_ = map_->GetHeight() / map_->GetNumberOfCellsVertically();
  cur_upper_left_x_ = map_->GetUpperLeftX() + x_ * cur_width_;
  cur_upper_left_y_ = map_->GetUpperLeftY() + y_ * cur_height_;

  RescaleImage();
}

int ObjectOnMap::GetX() const {
  return x_;
}

int ObjectOnMap::GetY() const {
  return y_;
}

MedicalKit::MedicalKit(std::shared_ptr<const Map> map, int x, int y)
    : ObjectOnMap(std::move(map), x, y) {
  LoadImage(":/textures/medicalkit.png");
}

Obstacle::Obstacle(std::shared_ptr<const Map> map, int x, int y)
    : ObjectOnMap(std::move(map), x, y) {
  LoadImage(":/textures/log.png");
}

Charge::Charge(std::shared_ptr<const Map> map, int x, int y)
    : ObjectOnMap(std::move(map), x, y) {
  LoadImage(":/textures/charge.png");
}
