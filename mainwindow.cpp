#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  // пока фиксированные размеры, потом будем смотреть
  setFixedSize(800, 600);

  new_game_button_ = new QPushButton("New game", this);
  new_game_button_->setGeometry(30, 30, 170, 100);

  swith_map_menu_ = new QComboBox(this);
  swith_map_menu_->setGeometry(30, 160, 170, 25);

  int map_number = 1;
  QFileInfo map_file("../tanks/maps/map" + QString::number(map_number) +
                     ".txt");
  while (map_file.exists() && map_file.isFile()) {
    swith_map_menu_->addItem("Map " + QString::number(map_number));
    map_number++;
    map_file =
        QFileInfo("../tanks/maps/map" + QString::number(map_number) + ".txt");
  }

  connect(new_game_button_, SIGNAL(clicked()), this, SLOT(RedrawMap()));
}

MainWindow::~MainWindow() {}

void MainWindow::RedrawMap() {
  map_ = Map(swith_map_menu_->currentIndex() + 1);
  repaint();
}

void MainWindow::paintEvent(QPaintEvent *) {
  QPainter p;
  p.begin(this);
  // координаты правой верхней точки карты и левой нижней точки карты
  map_.DrawMap(230, 30, 760, 570, p);
  p.end();
}
