﻿#ifndef OBSTACLANDBONUS_H
#define OBSTACLANDBONUS_H

#include <QDebug>
#include <QImage>
#include <QList>
#include <QObject>
#include <QVector>
#include <memory>
#include "map.h"

class ObjectOnMap {
 public:
  ObjectOnMap(std::shared_ptr<Map>& map, int x, int y);
  virtual void LoadImage() = 0;
  virtual ~ObjectOnMap() = default;
  void Draw(QPainter& painter);
  void RescaleImage();
  void UpdateCoordinates();

 protected:
  std::shared_ptr<Map> map_;
  int x_, y_, cur_width, cur_height, cur_upper_left_x_, cur_upper_left_y_;
  QImage image_;
  QImage scaled_image_;
};

class Obstacle : public ObjectOnMap {
 public:
  Obstacle(std::shared_ptr<Map>& map, int x, int y);
  void LoadImage() override;
};

class Bonus : public ObjectOnMap {
 public:
  Bonus(std::shared_ptr<Map>& map, int x, int y);
  void LoadImage() override;
  void RandomCoordinates();
};

#endif  // OBSTACLANDBONUS_H
