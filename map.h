#ifndef MAP_H
#define MAP_H

#include <QDebug>
#include <QFile>
#include <QImage>
#include <QPainter>
#include <QPair>
#include <QString>
#include <QTextStream>
#include <QVector>
#include "objectsonmap.h"

enum class CellType {
  Wall = 0,
  Grass = 1,
  Sand = 2,
  Water = 3
  // to be continued
};

class Map {
 public:
  explicit Map(int map_number);
  void UpdateCoordinates(int upper_left_x, int upper_left_y, int width,
                         int height);
  void DrawMap(QPainter &painter);

  CellType GetField(int cell_x, int cell_y) const;
  int GetWallsPrecalc(int x, int y) const;
  int GetNumberOfCellsHorizontally() const;
  int GetNumberOfCellsVertically() const;
  int GetUpperLeftX() const;
  int GetUpperLeftY() const;
  int GetWidth() const;
  int GetHeight() const;
  int GetTankInitCellX() const;
  int GetTankInitCellY() const;

 private:
  void RescaleImages();
  void WallsPrecalc();

  QVector<QVector<CellType>> map_;
  QVector<QVector<ObjectsOnMap>> objects_on_map_;
  QVector<QVector<int>> walls_precalc_;
  QVector<QImage> images_;
  QVector<QImage> scaled_images_;

  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  int cur_width_{};
  int cur_height_{};
  int cur_cell_width_{};
  int cur_cell_height_{};
  int tank_init_cell_x_;
  int tank_init_cell_y_;

  const int number_of_cell_types_ = 4;
};

#endif  // MAP_H
