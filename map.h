#ifndef MAP_H
#define MAP_H

#include <QDebug>
#include <QFile>
#include <QImage>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPainter>
#include <QPair>
#include <QString>
#include <QTextStream>
#include <QVector>
#include <vector>

enum class CellType { Wall = 0, Grass = 1, Sand = 2, Water = 3 };

class Map {
 public:
  explicit Map(int map_number);
  void UpdateCoordinates(const int upper_left_x, const int upper_left_y,
                         const int width, const int height);
  void DrawMap(QPainter &painter);

  CellType GetField(const int cell_x, const int cell_y) const;
  int GetWallsPrecalc(const int x, const int y) const;
  int GetNumberOfCellsHorizontally() const;
  int GetNumberOfCellsVertically() const;
  int GetUpperLeftX() const;
  int GetUpperLeftY() const;
  int GetWidth() const;
  int GetHeight() const;
  int GetTankInitCellX() const;
  int GetTankInitCellY() const;
  QString GetTankStartDirection() const;

 private:
  void RescaleImages();
  void FormMapImage();
  void WallsPrecalc();

  QVector<QVector<CellType>> map_;
  QVector<QVector<int>> walls_precalc_;
  std::vector<QImage> images_;
  std::vector<QImage> scaled_images_;
  QImage map_scaled_image_;

  int cur_upper_left_x_{};
  int cur_upper_left_y_{};
  int cur_width_{};
  int cur_height_{};
  int cur_cell_width_{};
  int cur_cell_height_{};
  int tank_init_cell_x_;
  int tank_init_cell_y_;
  QString tank_start_direction_;

  const int number_of_cell_types_ = 4;
};

#endif  // MAP_H
