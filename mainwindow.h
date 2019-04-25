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
#include <QSize>
#include <QString>
#include <QTextBrowser>
#include <QTimer>
#include <QToolTip>
#include <QTouchDevice>
#include <QTranslator>
#include <QVector>
#include <QtGlobal>
#include <algorithm>
#include <memory>
#include <utility>
#include <vector>
#include "boom.h"
#include "bot.h"
#include "cleverbot.h"
#include "improvedbot.h"
#include "map.h"
#include "objectonmap.h"
#include "portal.h"
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
  void About();
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
  void FormObjectsOnMapImage();
  void ShootRocket(std::shared_ptr<Tank> &tank);
  void MakeBoom(std::shared_ptr<Movable> &object);
  bool IsRocketByThisTank(const std::shared_ptr<Movable> &rocket,
                          const std::shared_ptr<Movable> &tank) const;
  int GetTimerDuration() const;
  void ToggleVirtualKeys();
  void GameOver();
  void InitializeNewGameDialog();
  void InitializeSettingsDialog();
  void InitializeAboutDialog();
  void DetermineCurrentLanguageSettings();
  void ChangeCurrentLanguageSettings();
  QJsonObject GetJsonObjectFromFile(const QString &filepath);
  Direction DetermineDirection(const QString &start_direction) const;
  void RandomMedicalKit();

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
  QLabel *language_menu_restart_label_;
  QLabel *settings_separator_label_;
  QLabel *version_label_;

  QDialog *about_dialog_;
  QDialogButtonBox *about_dialog_buttons_;
  QFormLayout *about_dialog_layout_;
  QTextBrowser *html_widget_;

  QPushButton *new_game_button_;
  QPushButton *pause_continue_button_;
  QPushButton *settings_button_;
  QPushButton *about_button_;

  QVector<QPushButton *> virtual_keys_buttons_;
  QVector<Qt::Key> virtual_keys_encodings_;
  const int number_of_virtual_keys_in_first_row_ = 2;
  const int number_of_virtual_keys_in_second_row_ = 3;

  std::shared_ptr<Map> map_;
  QList<std::shared_ptr<Movable>> tanks_;
  QList<std::shared_ptr<Movable>> rockets_;
  std::vector<std::vector<std::shared_ptr<ObjectOnMap>>> obstacles_and_bonuses_;
  QList<QPair<std::shared_ptr<Movable>, Coordinates>> objects_copies_;

  const int number_of_player_tanks_ = 1;
  const QVector<QString> difficulty_levels_names_ = {tr("Easy"), tr("Normal"),
                                                     tr("Hard")};
  QVector<TankQualities> available_tank_types_;

  const int timer_duration_ = 10;
  int timer_id_ = 0;
  int time_since_tooltip_appearing_ = 0;
  const int time_for_showing_tooltip_ = 1200;
  int time_since_last_bonus_ = 0;
  int sq_width_;
  int sq_height_;
  int w_indent_;
  int h_indent_;

  const QString app_version_ = "0.5.0.0";
};

#endif  // MAINWINDOW_H
