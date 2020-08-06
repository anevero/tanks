#ifndef STATIC_OBJECTS_OBJECTONMAP_H_
#define STATIC_OBJECTS_OBJECTONMAP_H_

#include <QImage>
#include <QString>
#include <QPainter>
#include <QPixmap>

#include <memory>
#include <utility>

#include "../game_core/map.h"

class ObjectOnMap {
 public:
  ObjectOnMap(std::shared_ptr<const Map> map, Coordinates coordinates);
  virtual ~ObjectOnMap() = default;

  void LoadImage(const QString& path);
  void Draw(QPainter* painter);
  void RescaleImage();

  void UpdateCoordinates();
  Coordinates GetCoordinates() const;

 protected:
  const std::shared_ptr<const Map> map_;
  Coordinates coordinates_;
  Coordinates current_upper_left_cell_coordinates_;

  int width_;
  int height_;

  QImage image_;
  QPixmap scaled_pixmap_;
};

class Obstacle : public ObjectOnMap {
 public:
  Obstacle(std::shared_ptr<const Map> map, Coordinates coordinates);
};

class MedicalKit : public ObjectOnMap {
 public:
  MedicalKit(std::shared_ptr<const Map> map, Coordinates coordinates);
};

class Charge : public ObjectOnMap {
 public:
  Charge(std::shared_ptr<const Map> map, Coordinates coordinates);
};

#endif  // STATIC_OBJECTS_OBJECTONMAP_H_
