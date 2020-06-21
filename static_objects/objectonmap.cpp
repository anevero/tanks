#include "objectonmap.h"

ObjectOnMap::ObjectOnMap(const std::shared_ptr<Map>& map, const size_t x,
                         const size_t y)
    : map_(map), x_(x), y_(y) {}

void ObjectOnMap::LoadImage(const QString& path) {
  image_.load(path);
  scaled_pixmap_ = QPixmap::fromImage(image_);
}

void ObjectOnMap::Draw(QPainter* painter) {
  RescaleImage();
  painter->save();
  painter->translate(cur_upper_left_x_ + cur_width_ / 2,
                     cur_upper_left_y_ + cur_height_ / 2);
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
  cur_width_ =
      static_cast<int>(map_->GetWidth() / map_->GetNumberOfCellsHorizontally());
  cur_height_ =
      static_cast<int>(map_->GetHeight() / map_->GetNumberOfCellsVertically());
  cur_upper_left_x_ = map_->GetUpperLeftX() + static_cast<int>(x_) * cur_width_;
  cur_upper_left_y_ =
      map_->GetUpperLeftY() + static_cast<int>(y_) * cur_height_;

  RescaleImage();
}

size_t ObjectOnMap::GetX() const { return x_; }

size_t ObjectOnMap::GetY() const { return y_; }

MedicalKit::MedicalKit(const std::shared_ptr<Map>& map, const size_t x,
                       const size_t y)
    : ObjectOnMap(map, x, y) {
  LoadImage(":/textures/medicalkit.png");
}

Obstacle::Obstacle(const std::shared_ptr<Map>& map, const size_t x,
                   const size_t y)
    : ObjectOnMap(map, x, y) {
  LoadImage(":/textures/log.png");
}

Charge::Charge(const std::shared_ptr<Map>& map, size_t x, size_t y)
    : ObjectOnMap(map, x, y) {
  LoadImage(":/textures/charge.png");
}
