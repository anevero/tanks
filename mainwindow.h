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
#include <QtGlobal>
#include <algorithm>
#include <memory>
#include "map.h"
#include "rocket.h"
#include "tank.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

 private:
  void keyReleaseEvent(QKeyEvent *) override;
  void paintEvent(QPaintEvent *) override;
  void resizeEvent(QResizeEvent *) override;
  void timerEvent(QTimerEvent *) override;

 private slots:
  void UpdateIndents();
  void RedrawButtons();
  void RedrawContent();

 private:
  void FindInteractingObjects();
  //  void InteractObjects(QList<std::shared_ptr<Movable>>::iterator &obj1,
  //                       QList<std::shared_ptr<Movable>>::iterator &obj2);
  bool HaveObjectsCollided(std::shared_ptr<Movable> &obj1,
                           std::shared_ptr<Movable> &obj2) const;
  void ShootRocket(std::shared_ptr<Tank> &object);
  bool IsRocketByThisTank(std::shared_ptr<Movable> &rocket,
                          std::shared_ptr<Movable> &tank) const;
  int GetTimerDuration() const;
  void GameOver();

 private:
  QPushButton *new_game_button_;
  QComboBox *swith_map_menu_;
  std::shared_ptr<Map> map_;
  QList<std::shared_ptr<Movable>> static_objects_;
  QList<std::shared_ptr<Movable>> moving_objects_;
  int number_of_player_tanks_ = 1;

  int timer_duration_ = 10;
  int timer_id_;
  int sq_width_;
  int sq_height_;
  int w_indent_;
  int h_indent_;

  QLabel *game_over_label_;
  QLabel *rotation_info_label_;
  QVector<QString> rotation_info_ = {"Up", "Right", "Down", "Left"};
};

#endif  // MAINWINDOW_H
