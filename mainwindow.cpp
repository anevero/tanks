#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), map_(1), tank_(&map_) {
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

  startTimer(timer_duration_);

  connect(new_game_button_, SIGNAL(clicked()), this, SLOT(RedrawContent()));
}

void MainWindow::paintEvent(QPaintEvent *) {
  qDebug() << "Paint Event Called";

  // координаты правой верхней точки карты, ее длина и высота
  map_.UpdateCoordinates(w_indent_ + static_cast<int>(0.28 * sq_width_),
                         h_indent_ + static_cast<int>(0.05 * sq_height_),
                         static_cast<int>(0.68 * sq_width_),
                         static_cast<int>(0.9 * sq_height_));
  tank_.UpdateCoordinates();

  // +UpdateCoordinates для всех объектов в нашем QSet/QVector

  QPainter p;
  p.begin(this);
  map_.DrawMap(p);
  tank_.Draw(p);

  // +Draw для всех объектов в нашем QSet/QVector

  p.end();
}

void MainWindow::timerEvent(QTimerEvent *event) {
  qDebug() << "Timer ID: " << event->timerId();

  // Move для танка и для всех объектов в нашем QSet/QVector

  // вызов paintevent
  repaint();
}

// этой функции место в классе Movable

// void MainWindow::Move() {
//  for (const auto object : objects) {
//    object->UpdateCoordinates(std::min(speed, object->GetLeftTime()));
//    object->EditTimeToMove(speed);

//    // потом еще дописать проверку на столкновение
//    if (object->GetLeftTime() <= 0) {
//      // нужно выкинуть элемент из вектора,
//      // правда пока не знаю, как сделать это аккуратно
//    }
//  }
//}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
  // здесь по идее надо не только задать направление, но еще и обнулить
  // внутренние параметры движения танка
  // и после этого при следующем срабатывании таймера он пойдет двигаться

  // и не забыть просто пропускать эти ивэнты, пока танк в движении, то
  // есть пока timeleft > 0

  //  switch (event->key()) {
  //    case Qt::Key_W | Qt::ShiftModifier:
  //      tank_.SetMoveDirection(Direction::Up);
  //      break;
  //    case Qt::Key_S | Qt::ShiftModifier:
  //      tank_.SetMoveDirection(Direction::Down);
  //      break;
  //    case Qt::Key_A | Qt::ShiftModifier:
  //      tank_.SetMoveDirection(Direction::Left);
  //      break;
  //    case Qt::Key_D | Qt::ShiftModifier:
  //      tank_.SetMoveDirection(Direction::Right);
  //      break;

  // с rotate пока не паримся

  //    case Qt::Key_W: tank_.SetRotateDirection(Direction::Up); break;
  //    case Qt::Key_S: tank_.SetRotateDirection(Direction::Down); break;
  //    case Qt::Key_A: tank_.SetRotateDirection(Direction::Left); break;
  //    case Qt::Key_D: tank_.SetRotateDirection(Direction::Right); break;
  //    default:
  //      return;

  //}
}

void MainWindow::resizeEvent(QResizeEvent *) {
  UpdateIndents();
  RedrawButtons();
}

void MainWindow::UpdateIndents() {
  sq_width_ = 4 * std::min(width() / 4, height() / 3);
  sq_height_ = 3 * std::min(width() / 4, height() / 3);

  w_indent_ = (width() - sq_width_) / 2;
  h_indent_ = (height() - sq_height_) / 2;
}

void MainWindow::RedrawButtons() {
  new_game_button_->setGeometry(w_indent_ + static_cast<int>(0.04 * sq_width_),
                                h_indent_ + static_cast<int>(0.05 * sq_height_),
                                static_cast<int>(0.2 * sq_width_),
                                static_cast<int>(0.05 * sq_height_));
  swith_map_menu_->setGeometry(w_indent_ + static_cast<int>(0.04 * sq_width_),
                               h_indent_ + static_cast<int>(0.15 * sq_height_),
                               static_cast<int>(0.2 * sq_width_),
                               static_cast<int>(0.05 * sq_height_));

  // карта перерисуется автоматически, так как resizeEvent автоматически
  // вызывает paintEvent
}

// функция вызывается при смене карты
void MainWindow::RedrawContent() {
  map_ = Map(swith_map_menu_->currentIndex() + 1);
  tank_ = Tank(&map_);
  repaint();
}
