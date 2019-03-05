#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), map_(1) {
  new_game_button_ = new QPushButton("New game", this);
  swith_map_menu_ = new QComboBox(this);

  int map_number = 1;
  QFileInfo map_file(":/maps/map" + QString::number(map_number) + ".txt");
  while (map_file.exists() && map_file.isFile()) {
    swith_map_menu_->addItem("Map " + QString::number(map_number));
    map_number++;
    map_file = QFileInfo(":/maps/map" + QString::number(map_number) + ".txt");
  }

  setMinimumSize(600, 450);
  resize(600, 450);

  connect(new_game_button_, SIGNAL(clicked()), this, SLOT(RedrawMap()));
}

void MainWindow::paintEvent(QPaintEvent *) {
  // координаты правой верхней точки карты, ее длина и высота
  map_.UpdateCoordinates(w_indent + static_cast<int>(0.28 * sq_width),
                         h_indent + static_cast<int>(0.05 * sq_height),
                         static_cast<int>(0.68 * sq_width),
                         static_cast<int>(0.9 * sq_height));
  QPainter p;
  p.begin(this);
  map_.DrawMap(p);
  p.end();
}

void MainWindow::resizeEvent(QResizeEvent *) {
  UpdateIndents();
  RedrawButtons();
}

void MainWindow::UpdateIndents() {
  sq_width = 4 * std::min(width() / 4, height() / 3);
  sq_height = 3 * std::min(width() / 4, height() / 3);

  w_indent = (width() - sq_width) / 2;
  h_indent = (height() - sq_height) / 2;
}

void MainWindow::RedrawMap() {
  map_ = Map(swith_map_menu_->currentIndex() + 1);
  repaint();
}

void MainWindow::RedrawButtons() {
  new_game_button_->setGeometry(w_indent + static_cast<int>(0.04 * sq_width),
                                h_indent + static_cast<int>(0.05 * sq_height),
                                static_cast<int>(0.2 * sq_width),
                                static_cast<int>(0.05 * sq_height));
  swith_map_menu_->setGeometry(w_indent + static_cast<int>(0.04 * sq_width),
                               h_indent + static_cast<int>(0.15 * sq_height),
                               static_cast<int>(0.2 * sq_width),
                               static_cast<int>(0.05 * sq_height));

  // карта перерисуется автоматически, так как resizeEvent автоматически
  // вызывает paintEvent
}
