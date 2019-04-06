﻿#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QDebug>
#include <QEvent>
#include <QFileInfo>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QSignalMapper>
#include <QString>
#include <QTimer>
#include <QToolTip>
#include <QtGlobal>
#include <algorithm>
#include <memory>
#include "bot.h"
#include <cleverbot.h>
#include "improvedbot.h"
#include "map.h"
#include "rocket.h"
#include "tank.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override = default;

 private:
  void mouseReleaseEvent(QMouseEvent *event) override;
  void keyReleaseEvent(QKeyEvent *event) override;
  void paintEvent(QPaintEvent *) override;
  void resizeEvent(QResizeEvent *) override;
  void timerEvent(QTimerEvent *) override;

 private slots:
  void UpdateIndents();
  void RedrawButtons();
  void RedrawContent();
  void PauseOrContinue();
  void PressVirtualKey(int key);

 private:
  void FindInteractingObjects();
  bool HaveObjectsCollided(const std::shared_ptr<Movable> &obj1,
                           const std::shared_ptr<Movable> &obj2) const;
  void CheckDeadObjects();
  void ShootRocket(std::shared_ptr<Tank> &object);
  bool IsRocketByThisTank(const std::shared_ptr<Movable> &rocket,
                          const std::shared_ptr<Movable> &tank) const;
  int GetTimerDuration() const;
  void GameOver();

 private:
  bool paused_ = false;

  QPushButton *new_game_button_;
  QPushButton *pause_continue_button_;
  QComboBox *switch_map_menu_;
  QComboBox *switch_tank_menu_;
  QComboBox *switch_difficulty_menu_;
  QLabel *switch_map_label_;
  QLabel *switch_tank_label_;
  QLabel *switch_difficulty_label_;

  QVector<QPushButton *> virtual_keys_buttons_;
  QVector<int> virtual_keys_encodings_;
  QSignalMapper *virtual_buttons_mapper_;
  int number_of_virtual_keys_in_first_row_ = 2;
  int number_of_virtual_keys_in_second_row_ = 3;

  std::shared_ptr<Map> map_;
  QList<std::shared_ptr<Movable>> tanks_;
  QList<std::shared_ptr<Movable>> rockets_;

  int number_of_player_tanks_ = 1;
  int number_of_difficulty_levels_ = 2;
  QVector<QString> difficulty_levels_names_ = {"Easy", "Normal", "Hard"};
  QVector<TankQualities> available_tank_types_;

  int timer_duration_ = 10;
  int timer_id_ = 0;
  int time_since_tooltip_appearing_ = 0;
  int time_for_showing_tooltip_ = 1200;
  int sq_width_;
  int sq_height_;
  int w_indent_;
  int h_indent_;
};

#endif  // MAINWINDOW_H
