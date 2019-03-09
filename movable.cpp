#include "movable.h"

Movable::Movable(Map* map, int cell_x, int cell_y)
    : cell_x_(cell_x), cell_y_(cell_y), map_(map) {}

void Movable::UpdateCoordinates() {
  // получаем текущие размеры ячеек из размеров карты
  int cur_cell_width = static_cast<int>(map_->cur_width_ / map_->map_.size());
  int cur_cell_height =
      static_cast<int>(map_->cur_height_ / map_->map_[0].size());

  // считаем координаты для отрисовки объекта
  // тут надо все заменить с учетом движения
  // пока можно считать, что любой наш объект - это квадрат с размерами ячейки
  // карты :)
  // кажется, что проще будет потом реализовать уменьшение размеров
  // непосредственно при отрисовке объекта по вот этим координатам,
  // которая будет происходить в дочерних классах, в соответствующих
  // функциях Draw
  cur_width_ = static_cast<int>(0.5 * cur_cell_width);
  cur_height_ = static_cast<int>(0.7 * cur_cell_height);
  cur_upper_left_x_ = map_->cur_upper_left_x_ + (cur_cell_width * cell_x_) +
                      (static_cast<int>(0.25 * cur_cell_width));
  cur_upper_left_y_ = map_->cur_upper_left_y_ + (cur_cell_height * cell_y_) +
                      (static_cast<int>(0.3 * cur_cell_height));
}
