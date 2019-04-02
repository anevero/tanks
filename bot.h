#ifndef BOT_H
#define BOT_H

#include <QDebug>
#include <QImage>
#include <QPainter>
#include <cmath>
#include <memory>
#include "map.h"
#include "tank.h"

struct BotQualities {
  TankQualities tank;
  int init_cell_x;
  int init_cell_y;
  int moving_length;
  int amount_of_turns;
};

class Bot : public Tank {
 public:
  Bot(std::shared_ptr<Map>& map, BotQualities qualities, Direction direction);
  virtual void LoadImage() override;

  virtual bool IsTurnNeeded() const;
  virtual bool IsMoveNeeded() const;
  virtual bool IsRotationStartNeeded(std::shared_ptr<Tank>);
  virtual bool IsMovingStartNeeded();
  virtual bool IsShotNeeded(std::shared_ptr<Map>, std::shared_ptr<Tank>);

 protected:
  int number_of_cells_to_move_ = 0;
  int number_of_turns_ = 0;
  const int moving_length_;
  const int amount_of_turns_;

  virtual bool CheckDirection(int& tank, int& bot, int direction);
};

#endif  // BOT_H
