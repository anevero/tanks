#ifndef MOVABLE_OBJECTS_CLEVERBOT_H_
#define MOVABLE_OBJECTS_CLEVERBOT_H_

#include <QQueue>
#include <QVector>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include "improvedbot.h"
#include "../static_objects/portal.h"

class CleverBot : public ImprovedBot {
 public:
  CleverBot(std::shared_ptr<Map> map, int init_cell_x, int init_cell_y,
            TankParameters tank_parameters, BotParameters bot_parameters,
            Direction direction);
  ~CleverBot() override = default;

 protected:
  bool IsRotationStartNeeded(const std::shared_ptr<Tank>& tank) override;
  bool IsMovingStartNeeded(
      const std::list<std::shared_ptr<Movable>>& objects,
      const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& portals)
  override;

 private:
  struct CellInfo {
    int cell_x;
    int cell_y;
    int prev_cell_x;
    int prev_cell_y;
    int distance;
  };
  std::vector<std::vector<int>> distance_;

  void Bfs(
      const std::list<std::shared_ptr<Movable>>& objects,
      const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& portals,
      int cell_x, int cell_y);

  int height_;
  int width_;
};

#endif  // MOVABLE_OBJECTS_CLEVERBOT_H_
