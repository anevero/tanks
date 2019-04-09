#ifndef OBSTACLANDBONUS_H
#define OBSTACLANDBONUS_H

#include <QDebug>
#include <QImage>
#include <QList>
#include <QObject>
#include <QVector>
#include <memory>
#include "map.h"

class ObjectsOnMap {
 public:
  ObjectsOnMap(std::shared_ptr<Map>& map, int x, int y);
  virtual void LoadImage() = 0;
  virtual ~ObjectsOnMap() = default;
  void Draw(QPainter& painter);
  void RescaleImage();

 protected:
  std::shared_ptr<Map> map_;
  int x_map_object_, y_map_object_;
  QImage image_;
  QImage scaled_image_;
};

class Obstacle : public ObjectsOnMap {
 public:
  Obstacle(std::shared_ptr<Map>& map, int x, int y);
  void LoadImage() override;
};

class Bonus : public ObjectsOnMap {
 public:
  Bonus(std::shared_ptr<Map>& map, int x, int y);
  void LoadImage() override;
  void RandomCoordinates();

 private:
};

#endif  // OBSTACLANDBONUS_H
