﻿#ifndef BOOM_H
#define BOOM_H

#include <QDebug>
#include <QMediaPlayer>
#include <QPainter>
#include <memory>
#include <vector>
#include "tank.h"

class Boom : public Movable {
 public:
  Boom(const std::shared_ptr<Map>& map, const std::shared_ptr<Movable>& tank,
       const int speed);
  void LoadImage() override;
  void Draw(QPainter& painter) override;

  void UpdateCoordinates(int cell_x, int cell_y) override;
  void StartMovement(
      const int number_of_cells, const QList<std::shared_ptr<Movable>>&,
      QList<QPair<std::shared_ptr<Movable>, Coordinates>>*,
      std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>*) override;

 private:
  QMediaPlayer boom_sound_;
};

#endif  // BOOM_H
