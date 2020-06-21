#ifndef MOVABLE_OBJECTS_CLEVERBOT_H_
#define MOVABLE_OBJECTS_CLEVERBOT_H_

#include <QQueue>
#include <QVector>
#include <memory>
#include <vector>
#include "improvedbot.h"
#include "../static_objects/portal.h"

class CleverBot : public ImprovedBot {
 public:
  CleverBot(const std::shared_ptr<Map>& map, const BotQualities& qualities);

 protected:
  bool IsRotationStartNeeded(const std::shared_ptr<Tank>& tank) override;
  bool IsMovingStartNeeded(
      const QList<std::shared_ptr<Movable>>& objects,
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
  QVector<QVector<int>> distance_;

  void Bfs(
      const QList<std::shared_ptr<Movable>>& objects,
      const std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>& portals,
      int cell_x, int cell_y);

  int height_;
  int width_;
};

#endif  // MOVABLE_OBJECTS_CLEVERBOT_H_