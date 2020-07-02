#ifndef STATIC_OBJECTS_OBJECTONMAP_H_
#define STATIC_OBJECTS_OBJECTONMAP_H_

#include <QDebug>
#include <QImage>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QVector>
#include <memory>
#include <utility>
#include "../game_core/constants.h"
#include "../game_core/map.h"

class ObjectOnMap {
 public:
  ObjectOnMap(std::shared_ptr<const Map> map, int x, int y);
  virtual ~ObjectOnMap() = default;

  void LoadImage(const QString& path);
  void Draw(QPainter* painter);
  void RescaleImage();
  void UpdateCoordinates();
  int GetX() const;
  int GetY() const;

 protected:
  const std::shared_ptr<const Map> map_;
  int x_;
  int y_;
  int cur_width_{};
  int cur_height_{};
  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  QImage image_;
  QPixmap scaled_pixmap_;
};

class Obstacle : public ObjectOnMap {
 public:
  Obstacle(std::shared_ptr<const Map> map, int x, int y);
};

class MedicalKit : public ObjectOnMap {
 public:
  MedicalKit(std::shared_ptr<const Map> map, int x, int y);
};

class Charge : public ObjectOnMap {
 public:
  Charge(std::shared_ptr<const Map> map, int x, int y);
};

#endif  // STATIC_OBJECTS_OBJECTONMAP_H_
