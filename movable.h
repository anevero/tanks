#ifndef MOVABLE_H
#define MOVABLE_H

#include "map.h"

enum class Direction {
    Up,
    Down,
    Left,
    Right
};

class Movable {
 public:
  Movable(Map* map, int cell_x, int cell_y);
  virtual void UpdateCoordinates(int time_of_movement);
  virtual void Draw(QPainter& painter) = 0;
  virtual ~Movable() = default;

 protected:
  // координаты ячейки объекта
  int cell_x_;
  int cell_y_;

  // координаты объекта в окне
  int cur_upper_left_x_;
  int cur_upper_left_y_;
  int cur_width_;
  int cur_height_;

  // указатель на карту, к которой привязан танк
  // с его помощью можно получить текущие размеры карты, например
  // хотя потом можно подумать про инкапсуляцию и другие умные слова и,
  // может, написать геттеры, а не так, как сейчас сделано, делать :D
  // какие-то геттеры есть, если что, дописать
  Map* map_;

  // добавить параметры, связанные с движением,
  // можно в отдельную секцию protected, чтобы не путаться
  // вот и они
protected:
  // эти параметры задаются каждому объекту
  int speed_of_movement;
  int left_time;

  Direction move_direction;
  Direction rotate_direction;

public:

  int GetSpeed();
  int GetLeftTime();
  Direction GetMoveDirection();
  Direction GetRotateDirection();
  void EditTimeToMove(int time);
};

#endif  // MOVABLE_H
