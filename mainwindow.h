#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QEvent>
#include <QFileInfo>
#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <algorithm>
#include <vector>
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
  void timerEvent(QTimerEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
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
  int timerId;

  // все движущиеся объекты
  std::vector<Movable*> objects;

  // скорость обновления таймера, мб потом изменим
  int speed = 10;

  // размеры и отступы прямоугольника 4*3 внутри окна, в котором
  // отрисовываются все элементы (для того, чтобы клетки были квадратными)
  int sq_width;
  int sq_height;
  int w_indent;
  int h_indent;
};

#endif  // MAINWINDOW_H
