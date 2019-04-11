#ifndef BOOM_H
#define BOOM_H

#include <QDebug>
#include <QPainter>
#include <memory>
#include "tank.h"

class Boom : public Movable {
 public:
  Boom(std::shared_ptr<Map>& map, std::shared_ptr<Movable>& tank, int speed);
  void LoadImage() override;
  void Draw(QPainter& painter) override;

  void UpdateCoordinates() override;
  void StartMovement(int number_of_cells,
                     QList<std::shared_ptr<Movable>>&) override;
};

#endif // BOOM_H
