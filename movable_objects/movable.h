#ifndef MOVABLE_OBJECTS_MOVABLE_H_
#define MOVABLE_OBJECTS_MOVABLE_H_

#include <QString>
#include <QPixmap>
#include <QImage>

#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../game_core/map.h"
#include "../static_objects/objectonmap.h"

enum class Direction {
  Up = 0,
  Right = 1,
  Down = 2,
  Left = 3
};

// Forward declaration to use this class in StartMovement function.
class Tank;

class Movable : public std::enable_shared_from_this<Movable> {
 public:
  Movable(std::shared_ptr<const Map> map, Coordinates cell,
          Direction direction, int speed);
  virtual ~Movable() = default;

  void LoadImage(const QString& path);

  // Defines current move characteristics by initializing several variables,
  // including current speed, number of cells to finish movement, time to
  // finish movement, copy cell coordinates in case of moving through portal.
  // These variables are initialized accordingly to the current state of the
  // object, e.g. obstacles or walls in front of him.
  virtual void StartMovement(const std::list<std::shared_ptr<Tank>>& tanks,
                             std::list<std::pair<std::shared_ptr<Tank>,
                                                 Coordinates>>* objects_copies_,
                             std::vector<std::vector<std::shared_ptr<
                                 ObjectOnMap>>>* objects,
                             int number_of_cells);
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
  virtual void UpdateCoordinates(Coordinates cell);
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

  int GetDirectionAsInt() const;
  Direction GetDirection() const;

  Coordinates GetUpperLeftCellCoordinates() const;
  int GetWidth() const;
  int GetHeight() const;
  Coordinates GetCoordinates() const;

  // Checks if given objects have been collided, i.e. they have common points
  // on the screen. Compares their coordinates for that.
  static bool HaveObjectsCollided(const std::shared_ptr<const Movable>& obj1,
                                  const std::shared_ptr<const Movable>& obj2);

  // Checks if given rocket has been released by the given tank. The result of
  // this function is used when we decide if we need to reduce tank's health
  // (rockets released by the tank can't reduce its own health).
  static bool CanRocketKillTank(const std::shared_ptr<const Movable>& rocket,
                                const std::shared_ptr<const Movable>& tank);

  // Returns direction object corresponding to the string.
  static Direction GetDirectionFromString(const std::string& direction);

 protected:
  void SwitchToNextDirection();
  void SwitchToPreviousDirection();
  void RescaleImage();
  Coordinates GetNewPortalCells(Coordinates portal_coordinates,
                                Coordinates new_cells) const;

  Coordinates current_cell_;
  Coordinates previous_cell_;

  Coordinates current_upper_left_cell_coordinates_;
  Coordinates previous_upper_left_cell_coordinates_;

  int current_width_;
  int current_height_;

  const std::shared_ptr<const Map> map_;

  // Item at index i is responsible for the i-th direction in Direction enum
  // class.
  std::vector<int> directions_ = {0, 0, 0, 0};

  int current_speed_;
  const int basic_speed_;

  int time_to_finish_movement_ = 0;
  int cells_to_finish_movement_ = 0;
  int current_rotate_degree_;
  int time_to_finish_rotation_ = 0;

  // Responsible for the direction of movement along the line
  // (vertically or horizontally).
  // Equals to 1 when the movable object going forward, and -1 otherwise.
  int linear_movement_direction_ = 1;

  // Equals to 1 when the movable object rotates clockwise, and -1 otherwise.
  int rotation_direction_ = 1;

  double opacity_ = 1;
  double previous_opacity_ = 1;

  bool copy_existence_ = false;

  QImage image_;
  QPixmap scaled_pixmap_;
};

#endif  // MOVABLE_OBJECTS_MOVABLE_H_
