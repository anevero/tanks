#ifndef BOT_H
#define BOT_H

#include <QDebug>
#include <QPainter>
#include <cmath>
#include <memory>
#include "map.h"
#include "tank.h"

class Bot : public Tank {
 public:
  Bot(std::shared_ptr<Map>& map, int init_cell_x, int init_cell_y, int speed,
               int rate_of_fire, Direction direction, int moving_length, int amount_of_turns);

  void Draw(QPainter& painter) override;

  bool IsTurnNeeded() const;
  bool IsMoveNeeded() const;
  bool IsRotationStartNeeded();
  bool IsMovingStartNeeded();
  virtual bool IsShotNeeded(std::shared_ptr<Map>, std::shared_ptr<Tank>);

 protected:
  int number_of_cells_to_move_ = 0;
  int number_of_turns_ = 0;
  const int moving_length_;
  const int amount_of_turns_;

  bool CheckDirection(int& first, int& second, int direction);
};

#endif // BOT_H
