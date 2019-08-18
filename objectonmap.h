#ifndef OBJECTONMAP_H
#define OBJECTONMAP_H

#include <QDebug>
#include <QImage>
#include <QList>
#include <QObject>
#include <QVector>
#include <memory>
#include "map.h"

enum class Bonus { TypeMedicalKit, TypeCharge };

class ObjectOnMap {
 public:
  ObjectOnMap(const std::shared_ptr<Map>& map, size_t x, size_t y);
  virtual void LoadImage() = 0;
  virtual ~ObjectOnMap() = default;
  void Draw(QPainter& painter);
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
  QImage scaled_image_;
};

class Obstacle : public ObjectOnMap {
 public:
  Obstacle(const std::shared_ptr<Map>& map, size_t x, size_t y);
  void LoadImage() override;
};

class MedicalKit : public ObjectOnMap {
 public:
  MedicalKit(const std::shared_ptr<Map>& map, size_t x, size_t y);
  void LoadImage() override;
};

class Charge : public ObjectOnMap {
 public:
  Charge(const std::shared_ptr<Map>& map, size_t x, size_t y);
  void LoadImage() override;
};

#endif  // OBJECTONMAP_H
