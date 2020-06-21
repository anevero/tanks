#ifndef STATIC_OBJECTS_OBJECTONMAP_H_
#define STATIC_OBJECTS_OBJECTONMAP_H_

#include <QDebug>
#include <QImage>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QVector>
#include <memory>
#include "../game_core/map.h"

enum class Bonus { TypeMedicalKit, TypeCharge };

class ObjectOnMap {
 public:
  ObjectOnMap(const std::shared_ptr<Map>& map, size_t x, size_t y);
  virtual ~ObjectOnMap() = default;

  void LoadImage(const QString& path);
  void Draw(QPainter* painter);
  void RescaleImage();
  void UpdateCoordinates();
  size_t GetX() const;
  size_t GetY() const;

 protected:
  const std::shared_ptr<Map> map_;
  size_t x_;
  size_t y_;
  int cur_width_{};
  int cur_height_{};
  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  QImage image_;
  QPixmap scaled_pixmap_;
};

class Obstacle : public ObjectOnMap {
 public:
  Obstacle(const std::shared_ptr<Map>& map, size_t x, size_t y);
};

class MedicalKit : public ObjectOnMap {
 public:
  MedicalKit(const std::shared_ptr<Map>& map, size_t x, size_t y);
};

class Charge : public ObjectOnMap {
 public:
  Charge(const std::shared_ptr<Map>& map, size_t x, size_t y);
};

#endif  // STATIC_OBJECTS_OBJECTONMAP_H_
