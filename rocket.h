#ifndef ROCKET_H
#define ROCKET_H

#include <QDebug>
#include <QPainter>
#include <memory>
#include "tank.h"

class Rocket : public Movable {
 public:
  Rocket(const std::shared_ptr<Map>& map, const std::shared_ptr<Tank>& tank,
         const int speed, const int power);
  void LoadImage() override;
  void Draw(QPainter& painter) override;

  std::shared_ptr<Tank> GetAttachedTank() const;
  int GetPower() const;

 private:
  const std::shared_ptr<Tank> tank_;
  const int power_;
};

#endif  // ROCKET_H
