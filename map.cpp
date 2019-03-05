#include "map.h"

Map::Map(int map_number) {
  QFile input_file(":/maps/map" + QString::number(map_number) + ".txt");
  input_file.open(QIODevice::ReadOnly);
  QTextStream in(&input_file);

  int map_width_in_cells, map_height_in_cells;
  in >> map_width_in_cells >> map_height_in_cells;

  map_.resize(map_width_in_cells);

  for (int i = 0; i < map_height_in_cells; ++i) {
    for (int j = 0; j < map_width_in_cells; ++j) {
      in.skipWhiteSpace();
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
    }
  }

  in >> tank_init_cell_x_ >> tank_init_cell_y_;
  tank_init_cell_x_--;
  tank_init_cell_y_--;
  input_file.close();
}

void Map::UpdateCoordinates(int upper_left_x, int upper_left_y, int width,
                            int height) {
  cur_upper_left_x_ = upper_left_x;
  cur_upper_left_y_ = upper_left_y;
  cur_width_ = width;
  cur_height_ = height;
}

void Map::DrawMap(QPainter &painter) {
  int cell_width = cur_width_ / map_.size();
  int cell_height = cur_height_ / map_[0].size();

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
      painter.drawRect(cur_upper_left_x_ + i * cell_width,
                       cur_upper_left_y_ + j * cell_height, cell_width,
                       cell_height);
    }
  }
}
