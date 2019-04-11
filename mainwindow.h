#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFileInfo>
#include <QFormLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QToolTip>
#include <QTouchDevice>
#include <QTranslator>
#include <QtGlobal>
#include <algorithm>
#include <memory>
#include <vector>
#include "bot.h"
#include "cleverbot.h"
#include "improvedbot.h"
#include "map.h"
#include "objectonmap.h"
#include "rocket.h"
#include "tank.h"

struct GameOptions {
  int map_number;
  int tank_number;
  int difficulty_level_number;
};

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
  void NewGame();
  void Settings();
  void UpdateIndents();
  void RedrawButtons();
  void RedrawContent();
  void PauseOrContinue();
  void PressVirtualKey(Qt::Key key);

 private:
  void FindInteractingObjects();
  bool HaveObjectsCollided(const std::shared_ptr<Movable> &obj1,
                           const std::shared_ptr<Movable> &obj2) const;
  void CheckDeadObjects();
  void ShootRocket(std::shared_ptr<Tank> &object);
  bool IsRocketByThisTank(const std::shared_ptr<Movable> &rocket,
                          const std::shared_ptr<Movable> &tank) const;
  int GetTimerDuration() const;
  void ToggleVirtualKeys();
  void GameOver();
  void InitializeNewGameDialog();
  void InitializeSettingsDialog();

 private:
  bool paused_ = false;
  bool virtual_keys_shown_ = true;

  GameOptions current_game_options_{0, 0, 0};

  QDialog *new_game_dialog_;
  QDialogButtonBox *new_game_dialog_buttons_;
  QFormLayout *new_game_dialog_layout_;
  QComboBox *switch_map_menu_;
  QComboBox *switch_tank_menu_;
  QComboBox *switch_difficulty_menu_;
  QLabel *new_game_info_label_;
  QLabel *switch_map_label_;
  QLabel *switch_tank_label_;
  QLabel *switch_difficulty_label_;

  QDialog *settings_dialog_;
  QDialogButtonBox *settings_dialog_buttons_;
  QFormLayout *settings_dialog_layout_;
  QCheckBox *virtual_keys_checkbox_;
  QComboBox *language_menu_;
  QLabel *language_menu_label_;
  QLabel *settings_separator_label_;
  QLabel *version_label_;

  QPushButton *new_game_button_;
  QPushButton *pause_continue_button_;
  QPushButton *settings_button_;

  QVector<QPushButton *> virtual_keys_buttons_;
  QVector<Qt::Key> virtual_keys_encodings_;
  int number_of_virtual_keys_in_first_row_ = 2;
  int number_of_virtual_keys_in_second_row_ = 3;

  std::shared_ptr<Map> map_;
  QList<std::shared_ptr<Movable>> tanks_;
  QList<std::shared_ptr<Movable>> rockets_;
  std::vector<std::vector<std::shared_ptr<ObjectOnMap>>> obstacles_and_bonuses_;

  int number_of_player_tanks_ = 1;
  QVector<QString> difficulty_levels_names_ = {tr("Easy"), tr("Normal"),
                                               tr("Hard")};
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
