#ifndef GAME_CORE_MAP_H_
#define GAME_CORE_MAP_H_

#include <QImage>
#include <QPainter>
#include <QPixmap>

#include <string>
#include <vector>

enum class CellType {
  Wall = 0,
  Grass = 1,
  Sand = 2,
  Water = 3,
  Forest = 4,
  Last = 5
};

struct Coordinates {
  Coordinates() : x(0), y(0) {}
  Coordinates(int x, int y) : x(x), y(y) {}

  bool operator==(Coordinates coordinates);
  bool operator!=(Coordinates coordinates);

  int x;
  int y;
};

class Map {
 public:
  // The constructor creates necessary object by reading corresponding to
  // map_number file.
  explicit Map(int map_number);
  void DrawMap(QPainter* painter);

  void UpdateCoordinates(Coordinates upper_left_cell_coordinates,
                         int width, int height);

  CellType GetField(Coordinates cell) const;
  int GetWallsPrecalculation(int cell_x, int cell_y) const;

  int GetNumberOfCellsHorizontally() const;
  int GetNumberOfCellsVertically() const;

  Coordinates GetUpperLeftCellCoordinates() const;

  int GetWidth() const;
  int GetHeight() const;
  int GetCellWidth() const;
  int GetCellHeight() const;

  Coordinates GetTankInitialCoordinates() const;
  std::string GetTankStartDirection() const;

 private:
  std::vector<std::vector<CellType>> map_;

  void RescaleImages();
  void FormMapImage();

  std::vector<QImage> images_;
  std::vector<QPixmap> scaled_pixmaps_;
  QPixmap map_scaled_pixmap_;

  void WallsPrecalculation();
  std::vector<std::vector<int>> walls_precalculation_;

  int current_width_;
  int current_height_;
  int current_cell_width_;
  int current_cell_height_;

  Coordinates current_upper_left_cell_coordinates_;
  Coordinates tank_initial_cell_;
  std::string tank_start_direction_;
};

#endif  // GAME_CORE_MAP_H_
