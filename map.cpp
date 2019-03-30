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
        case ('E'):
          map_[j].push_back(CellType::Ground);
          break;
        case ('Q'):
          map_[j].push_back(CellType::Water);
          break;
      }
    }
  }

  in >> tank_init_cell_x_ >> tank_init_cell_y_;
  tank_init_cell_x_--;
  tank_init_cell_y_--;

  // Пока считывание роботов будет так, потом можно придумать что-то другое
  int amount_of_robots;
  in >> amount_of_robots;
  int bot_cell_x, bot_cell_y;
  for (int i = 0; i < amount_of_robots; i++) {
    in >> bot_cell_x >> bot_cell_y;
    coordinates_.push_back({bot_cell_x, bot_cell_y});
  }

  input_file.close();

  QImage image;
  for (int i = 0; i < number_of_cell_types_; ++i) {
    image.load(":/textures/" + QString::number(i) + ".png");
    images_.push_back(image);
  }
  scaled_images_ = images_;
}

void Map::UpdateCoordinates(int upper_left_x, int upper_left_y, int width,
                            int height) {
  cur_upper_left_x_ = upper_left_x;
  cur_upper_left_y_ = upper_left_y;
  cur_width_ = width;
  cur_height_ = height;
  cur_cell_width_ = cur_width_ / map_.size();
  cur_cell_height_ = cur_height_ / map_[0].size();
  RescaleImages();
}

void Map::DrawMap(QPainter& painter) {
  QImage image;
  for (int i = 0; i < map_.size(); ++i) {
    for (int j = 0; j < map_[i].size(); ++j) {
      image = scaled_images_[static_cast<int>(map_[i][j])];
      painter.drawImage(cur_upper_left_x_ + i * cur_cell_width_ - 1,
                        cur_upper_left_y_ + j * cur_cell_height_ - 1, image);
    }
  }
}

CellType Map::GetField(int cell_x, int cell_y) const {
  return map_[cell_x][cell_y];
}
int Map::GetNumberOfCellsHorizontally() const { return map_.size(); }
int Map::GetNumberOfCellsVertically() const { return map_[0].size(); }
int Map::GetUpperLeftX() const { return cur_upper_left_x_; }
int Map::GetUpperLeftY() const { return cur_upper_left_y_; }
int Map::GetWidth() const { return cur_width_; }
int Map::GetHeight() const { return cur_height_; }
int Map::GetTankInitCellX() const { return tank_init_cell_x_; }
int Map::GetTankInitCellY() const { return tank_init_cell_y_; }

void Map::RescaleImages() {
  if (scaled_images_[0].width() == cur_cell_width_ + 2 &&
      scaled_images_[0].height() == cur_cell_height_ + 2) {
    return;
  }
  for (int i = 0; i < number_of_cell_types_; ++i) {
    scaled_images_[i] = images_[i].scaled(
        cur_cell_width_ + 2, cur_cell_height_ + 2, Qt::KeepAspectRatio);
  }
}
