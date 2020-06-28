#ifndef MOVABLE_OBJECTS_MOVABLE_H_
#define MOVABLE_OBJECTS_MOVABLE_H_

#include <QDebug>
#include <QImage>
#include <QList>
#include <QPixmap>
#include <QVector>
#include <algorithm>
#include <cmath>
#include <list>
#include <memory>
#include <utility>
#include <vector>
#include "../game_core/map.h"
#include "../static_objects/objectonmap.h"
#include "../static_objects/portal.h"

enum class Direction {
  Up = 0,
  Right = 1,
  Down = 2,
  Left = 3
};

struct Coordinates {
  int x;
  int y;
};

class Movable : public std::enable_shared_from_this<Movable> {
 public:
  Movable(std::shared_ptr<Map> map, int cell_x, int cell_y,
          Direction direction, int speed);
  virtual ~Movable() = default;

  void LoadImage(const QString& path);

  // Defines current move characteristics by initializing several variables,
  // including current speed, number of cells to finish movement, time to
  // finish movement, copy cell coordinates in case of moving through portal.
  // These variables are initialized accordingly to the current state of the
  // object, e.g. obstacles or walls in front of him.
  virtual void StartMovement(
      int number_of_cells, const std::list<std::shared_ptr<Movable>>& tanks,
      std::list<std::pair<std::shared_ptr<Movable>, Coordinates>>*
      objects_copies_,
      std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>* objects);
  // Updates state of current move by updating time to finish movement variable.
  virtual void Move(int milliseconds_passed);
  // Changes current move direction to the reverse.
  virtual void TurnReverseOn();
  // Changes current move direction from the reverse to normal.
  virtual void TurnReverseOff();

  // Defines current rotation characteristics by initializing several variables,
  // including current rotation speed, time to finish rotation.
  // These variables are initialized accordingly to the current state of the
  // object, e.g. obstacles or walls in front of him.
  virtual void StartRotation();
  // Updates state of current rotation by updating time to finish rotation
  // variable.
  virtual void Rotate(int milliseconds_passed);
  // Changes current rotation direction to the reverse.
  virtual void TurnRotationReverseOn();
  // Changes current rotation direction from the reverse to normal.
  virtual void TurnRotationReverseOff();

  // Updates object's coordinates on the map according to the cell numbers and
  // the state of movement or rotation.
  virtual void UpdateCoordinates(int cell_x, int cell_y);
  virtual void Draw(QPainter* painter) = 0;

  // Changes object's coordinates from current to previous. It's used in
  // portals implementation. To draw one tank in two places at the same time
  // we firstly draw it in the first place, then change its coordinates to copy
  // coordinates and draw it once again in the second place, then return to
  // original coordinates.
  virtual void ReturnToOriginal();

  int GetTimeToFinishMovement() const;
  int GetCellsToFinishMovement() const;
  int GetTimeToFinishRotation() const;
  bool IsMovingOrRotating() const;

  int GetIntDirection() const;
  Direction GetDirection() const;

  int GetUpperLeftX() const;
  int GetUpperLeftY() const;
  int GetWidth() const;
  int GetHeight() const;
  int GetCellX() const;
  int GetCellY() const;

  // Checks if given objects have been collided, i.e. they have common points
  // on the screen. Compares their coordinates for that.
  static bool HaveObjectsCollided(const std::shared_ptr<Movable>& obj1,
                                  const std::shared_ptr<Movable>& obj2);

  // Checks if given rocket has been released by the given tank. The result of
  // this function is used when we decide if we need to reduce tank's health
  // (rockets released by the tank can't reduce its own health).
  static bool IsRocketByThisTank(const std::shared_ptr<Movable>& rocket,
                                 const std::shared_ptr<Movable>& tank);

  // Returns direction object corresponding to the string.
  static Direction GetDirectionFromString(const std::string& direction);

 protected:
  void SwitchToNextDirection();
  void SwitchToPrevDirection();
  void RescaleImage();
  Coordinates GetNewPortalCells(int portal_cell_x, int portal_cell_y,
                                int new_cell_x, int new_cell_y) const;

  int cell_x_;
  int cell_y_;
  int prev_cell_x_;
  int prev_cell_y_;

  int current_upper_left_x_{};
  int current_upper_left_y_{};
  int prev_upper_left_x_{};
  int prev_upper_left_y_{};
  int current_width_{};
  int current_height_{};

  const std::shared_ptr<Map> map_;
  std::vector<int> directions_ = {0, 0, 0, 0};

  int current_speed_;
  const int basic_speed_;

  int reverse_ = 1;
  int time_to_finish_movement_ = 0;
  int cells_to_finish_movement_ = 0;
  int current_rotate_degree_;
  int time_to_finish_rotation_ = 0;
  int rotate_reverse_ = 1;

  double opacity_ = 1;
  double prev_opacity_ = 1;
  bool copy_existence_ = false;

  QImage image_;
  QPixmap scaled_pixmap_;
};

#endif  // MOVABLE_OBJECTS_MOVABLE_H_
