#include "map.h"

Map::Map(int map_number) {
  std::fstream in("../tanks/maps/map" + std::to_string(map_number) + ".txt");

  int map_width_in_cells, map_height_in_cells;
  in >> map_width_in_cells >> map_height_in_cells;
  map_.resize(map_width_in_cells);

  for (int i = 0; i < map_height_in_cells; ++i) {
    for (int j = 0; j < map_width_in_cells; ++j) {
      char temp_value;
      in >> temp_value;
      switch (temp_value) {
      case ('G'):
        map_[j].push_back(CellType::Grass);
        break;
      case ('W'):
        map_[j].push_back(CellType::Wall);
        break;
      }
      in.ignore(1);
    }
  }
}

void Map::DrawMap(int upper_left_x, int upper_left_y, int lower_right_x,
                  int lower_right_y, QPainter &painter) {
  int cell_width = (lower_right_x - upper_left_x) / map_.size();
  int cell_height = (lower_right_y - upper_left_y) / map_[0].size();

  for (int i = 0; i < map_.size(); ++i) {
    for (int j = 0; j < map_[i].size(); ++j) {
      switch (map_[i][j]) {
      case CellType::Wall:
        painter.setBrush(Qt::red);
        break;
      case CellType::Grass:
        painter.setBrush(Qt::green);
        break;
      }
      painter.drawRect(upper_left_x + i * cell_width,
                       upper_left_y + j * cell_height, cell_width, cell_height);
    }
  }
}
