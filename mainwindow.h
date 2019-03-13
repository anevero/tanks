#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QDebug>
#include <QEvent>
#include <QFileInfo>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QVector>
#include "map.h"
#include "tank.h"
#include "rocket.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

  int GetTimerDuration() const;

 private:
  void keyReleaseEvent(QKeyEvent *) override;
  void keyPressEvent(QKeyEvent *) override;
  void paintEvent(QPaintEvent *) override;
  void resizeEvent(QResizeEvent *) override;
  void timerEvent(QTimerEvent *) override;

 private slots:
  void UpdateIndents();
  void RedrawButtons();
  void RedrawContent();

 private:
  QPushButton *new_game_button_;
  QComboBox *swith_map_menu_;
  Map map_;
  Tank tank_;
  QList<Movable *> moving_objects_;
  QVector<bool> keys = {0, 0, 0, 0, 0, 0, 0, 0};

  int timer_duration_ = 10;

  // размеры и отступы прямоугольника 4*3 внутри окна, в котором
  // отрисовываются все элементы (для того, чтобы клетки были квадратными)
  int sq_width_;
  int sq_height_;
  int w_indent_;
  int h_indent_;

  // временная штука для вывода направления движения
  QLabel *rotation_info_label_;
  QVector<QString> rotation_info_ = {"Up", "Right", "Down", "Left"};
};

#endif  // MAINWINDOW_H
