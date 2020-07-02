#ifndef MOVABLE_OBJECTS_CLEVERBOT_H_
#define MOVABLE_OBJECTS_CLEVERBOT_H_

#include <QVector>
#include <list>
#include <memory>
#include <queue>
#include <utility>
#include <vector>
#include "improvedbot.h"
#include "../static_objects/portal.h"

class CleverBot : public ImprovedBot {
 public:
  CleverBot(std::shared_ptr<const Map> map, int init_cell_x, int init_cell_y,
            TankParameters tank_parameters, BotParameters bot_parameters,
            Direction direction);
  ~CleverBot() override = default;

 protected:
  bool IsRotationStartNeeded(const std::shared_ptr<const Tank>& tank) override;
  bool IsMovingStartNeeded(
      const std::list<std::shared_ptr<Tank>>& objects,
      const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& portals)
  override;

 private:
  struct CellInfo {
    int cell_x;
    int cell_y;
    int prev_cell_x;
    int prev_cell_y;
    int distance;

    CellInfo(int cell_x, int cell_y,
             int prev_cell_x, int prev_cell_y,
             int distance)
        : cell_x(cell_x), cell_y(cell_y),
          prev_cell_x(prev_cell_x), prev_cell_y(prev_cell_y),
          distance(distance) {}
  };
  std::vector<std::vector<int>> distance_;

  void Bfs(
      const std::list<std::shared_ptr<Tank>>& objects,
      const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& portals,
      int cell_x, int cell_y);

  int height_;
  int width_;
};

#endif  // MOVABLE_OBJECTS_CLEVERBOT_H_
