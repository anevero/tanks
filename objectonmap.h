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
  ObjectOnMap(const std::shared_ptr<Map>& map, const int x, const int y);
  virtual void LoadImage() = 0;
  virtual ~ObjectOnMap() = default;
  void Draw(QPainter& painter);
  void RescaleImage();
  void UpdateCoordinates();
  int GetX() const;
  int GetY() const;

 protected:
  const std::shared_ptr<Map> map_;
  int x_;
  int y_;
  int cur_width_{};
  int cur_height_{};
  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  QImage image_;
  QImage scaled_image_;
};

class Obstacle : public ObjectOnMap {
 public:
  Obstacle(const std::shared_ptr<Map>& map, const int x, const int y);
  void LoadImage() override;
};

class MedicalKit : public ObjectOnMap {
 public:
  MedicalKit(const std::shared_ptr<Map>& map, const int x, const int y);
  void LoadImage() override;
};

class Charge : public ObjectOnMap {
 public:
  Charge(std::shared_ptr<Map>& map, int x, int y);
  void LoadImage() override;
};

#endif  // OBJECTONMAP_H
