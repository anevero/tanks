#ifndef BOT_H
#define BOT_H

#include <QDebug>
#include <QPainter>
#include <memory>
#include "map.h"
#include "tank.h"

class Bot : public Tank {
 public:
  explicit Bot(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y, int speed,
               int rate_of_fire, Direction direction);

  void Draw(QPainter& painter) override;

  bool IsTurnNeeded() const;
  bool IsRotationStartNeeded() const;
  bool IsShotNeeded(std::shared_ptr<Map>, std::shared_ptr<Tank>) const;
};

#endif // BOT_H
