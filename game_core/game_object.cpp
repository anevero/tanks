#include "game_object.h"

GameObject::GameObject(
    std::shared_ptr<const Map> map, const QString& texture_path,
    Coordinates coordinates)
    : map_(std::move(map)), cell_(coordinates), width_(0), height_(0) {
  LoadImage(texture_path);
}

void GameObject::LoadImage(const QString& path) {
  image_.load(path);
  scaled_pixmap_ = QPixmap::fromImage(image_);
}

Coordinates GameObject::GetCoordinates() const {
  return cell_;
}

Coordinates GameObject::GetUpperLeftCellCoordinates() const {
  return upper_left_cell_coordinates_;
}

int GameObject::GetWidth() const {
  return width_;
}

int GameObject::GetHeight() const {
  return height_;
}

void GameObject::RescaleImage() {
  if (scaled_pixmap_.width() == width_ && scaled_pixmap_.height() == height_) {
    return;
  }
  scaled_pixmap_ = QPixmap::fromImage(
      image_.scaled(width_, height_, Qt::KeepAspectRatio));
}
