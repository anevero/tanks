#ifndef GAME_CORE_GAME_OBJECT_H_
#define GAME_CORE_GAME_OBJECT_H_

#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QString>

#include <memory>
#include <utility>

#include "map.h"

class GameObject {
 public:
  GameObject(std::shared_ptr<const Map> map, const QString& path,
             Coordinates coordinates);
  virtual ~GameObject() = default;

  void LoadImage(const QString& path);
  virtual void Draw(QPainter* painter) = 0;

  Coordinates GetCoordinates() const;
  Coordinates GetUpperLeftCellCoordinates() const;

  int GetWidth() const;
  int GetHeight() const;

 protected:
  void RescaleImage();

 protected:
  const std::shared_ptr<const Map> map_;

  Coordinates cell_;
  Coordinates upper_left_cell_coordinates_;

  int width_;
  int height_;

  QImage image_;
  QPixmap scaled_pixmap_;
};

#endif  // GAME_CORE_GAME_OBJECT_H_
