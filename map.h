#ifndef MAP_H
#define MAP_H

#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QString>
#include <QTextStream>
#include <QVector>

enum class CellType {
  Wall,
  Grass
  // to be continued
};

class Map {
public:
  explicit Map(int map_number);

  void DrawMap(int upper_left_x, int upper_left_y, int lower_right_x,
               int lower_right_y, QPainter &painter);

private:
  QVector<QVector<CellType>> map_;
};

#endif // MAP_H
