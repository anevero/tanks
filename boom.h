#ifndef BOOM_H_
#define BOOM_H_

#include <QDebug>
#include <QMediaPlayer>
#include <QPainter>
#include <memory>
#include <vector>
#include "tank.h"

class Boom : public Movable {
 public:
  Boom(const std::shared_ptr<Map>& map, const std::shared_ptr<Movable>& tank,
       int speed);
  void LoadImage() override;
  void Draw(QPainter& painter) override;

  void UpdateCoordinates(size_t cell_x, size_t cell_y) override;
  void StartMovement(
      int number_of_cells, const QList<std::shared_ptr<Movable>>&,
      QList<QPair<std::shared_ptr<Movable>, Coordinates>>*,
      std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>*) override;

 private:
  QMediaPlayer boom_sound_;
};

#endif  // BOOM_H_
