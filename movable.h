#ifndef MOVABLE_H
#define MOVABLE_H

#include "map.h"

class Movable {
 public:
  Movable(Map* map, int cell_x, int cell_y);
  virtual void UpdateCoordinates();
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
  Map* map_;

  // добавить параметры, связанные с движением,
  // можно в отдельную секцию protected, чтобы не путаться
};

#endif  // MOVABLE_H
