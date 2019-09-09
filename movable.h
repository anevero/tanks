#ifndef MOVABLE_H
#define MOVABLE_H

#include <QDebug>
#include <QImage>
#include <QList>
#include <QVector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <vector>
#include "map.h"
#include "objectonmap.h"
#include "portal.h"

enum class Direction { Up = 0, Right = 1, Down = 2, Left = 3 };

struct Coordinates {
  size_t x;
  size_t y;
};

class Movable : public std::enable_shared_from_this<Movable> {
 public:
  Movable(const std::shared_ptr<Map>& map, size_t cell_x, size_t cell_y,
          Direction direction, int speed);
  virtual ~Movable() = default;
  virtual void LoadImage() = 0;

  // Defines current move characteristics by initializing several variables,
  // including current speed, number of cells to finish movement, time to
  // finish movement, copy cell coordinates in case of moving through portal.
  // These variables are initialized accordingly to the current state of movable
  // object, e.g. obstacles or walls in front of him.
  virtual void StartMovement(
      int number_of_cells, const QList<std::shared_ptr<Movable>>& tanks,
      QList<QPair<std::shared_ptr<Movable>, Coordinates>>* objects_copies_,
      std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>* objects);
  // Updates state of current move by updating time to finish movement variable.
  virtual void Move(int milliseconds_passed);
  // Changes current move direction to the reverse.
  virtual void TurnReverseOn();
  // Changes current move direction from the reverse to normal.
  virtual void TurnReverseOff();

  // Defines current rotation characteristics by initializing several variables,
  // including current rotation speed, time to finish rotation.
  // These variables are initialized accordingly to the current state of movable
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
  virtual void UpdateCoordinates(size_t cell_x, size_t cell_y);
  virtual void Draw(QPainter& painter) = 0;
  // Changes object's coordinates from current to previous. It's used in
  // portals implementation. To draw one tank in two places at the same time
  // we firstly draw it in the first place, then change its coordinates to copy
  // coordinates and draw it once again in the second place, then return to
  // original coordinates.
  virtual void ReturnToOriginal();

  virtual int GetTimeToFinishMovement() const;
  virtual int GetCellsToFinishMovement() const;
  virtual int GetTimeToFinishRotation() const;
  virtual bool IsMovingOrRotating() const;

  [[maybe_unused]] virtual int GetReverseState() const;
  [[maybe_unused]] virtual int GetRotationReverseState() const;
  virtual int GetIntDirection() const;
  virtual Direction GetDirection() const;

  virtual int GetUpperLeftX() const;
  virtual int GetUpperLeftY() const;
  virtual int GetWidth() const;
  virtual int GetHeight() const;
  virtual size_t GetCellX() const;
  virtual size_t GetCellY() const;

 protected:
  virtual void SwitchToNextDirection();
  virtual void SwitchToPrevDirection();
  virtual void RescaleImage();
  Coordinates GetNewPortalCells(size_t portal_cell_x, size_t portal_cell_y,
                                size_t new_cell_x, size_t new_cell_y);

  size_t cell_x_;
  size_t cell_y_;
  size_t prev_cell_x_;
  size_t prev_cell_y_;

  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  int prev_upper_left_x_{};
  int prev_upper_left_y_{};
  int cur_width_{};
  int cur_height_{};

  const std::shared_ptr<Map> map_;
  QVector<int> directions_ = {0, 0, 0, 0};

  int current_speed_{};
  const int basic_speed_{};
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
  QImage scaled_image_;
};

#endif  // MOVABLE_H
