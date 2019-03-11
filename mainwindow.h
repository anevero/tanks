#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QDebug>
#include <QEvent>
#include <QFileInfo>
#include <QKeyEvent>
#include <QList>
#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <algorithm>
#include <cmath>
#include "map.h"
#include "tank.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

  int GetSpeed();

 private:
  void paintEvent(QPaintEvent *) override;
  void timerEvent(QTimerEvent *event) override;
  void keyReleaseEvent(QKeyEvent *) override;
  void resizeEvent(QResizeEvent *) override;
  void Move();

 private slots:
  void UpdateIndents();
  void RedrawButtons();
  void RedrawContent();

 private:
  QPushButton *new_game_button_;
  QComboBox *swith_map_menu_;
  Map map_;
  Tank tank_;

  // все движущиеся объекты
  QList<Movable *> moving_objects_;

  int timer_duration_ = 10;

  // размеры и отступы прямоугольника 4*3 внутри окна, в котором
  // отрисовываются все элементы (для того, чтобы клетки были квадратными)
  int sq_width_;
  int sq_height_;
  int w_indent_;
  int h_indent_;
};

#endif  // MAINWINDOW_H
