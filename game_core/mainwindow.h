#ifndef GAME_CORE_MAINWINDOW_H_
#define GAME_CORE_MAINWINDOW_H_

#include <QBoxLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QKeyEvent>
#include <QLCDNumber>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QPushButton>
#include <QSettings>
#include <QSize>
#include <QScreen>
#include <QString>
#include <QTextBrowser>
#include <QTimer>
#include <QToolTip>
#include <QTouchDevice>
#include <QTranslator>
#include <QUrl>
#include <QVector>
#include <QtGlobal>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "game_core/about_dialog.h"
#include "game_core/constants.h"
#include "game_core/map.h"
#include "game_core/new_game_dialog.h"
#include "game_core/settings_dialog.h"
#include "static_objects/objectonmap.h"
#include "static_objects/portal.h"
#include "movable_objects/boom.h"
#include "movable_objects/bot.h"
#include "movable_objects/cleverbot.h"
#include "movable_objects/improvedbot.h"
#include "movable_objects/rocket.h"
#include "movable_objects/tank.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override = default;

 private:
  [[maybe_unused]] void mouseReleaseEvent(QMouseEvent* event) override;
  [[maybe_unused]] void keyReleaseEvent(QKeyEvent* event) override;
  [[maybe_unused]] void paintEvent(QPaintEvent*) override;
  [[maybe_unused]] void resizeEvent(QResizeEvent*) override;
  [[maybe_unused]] void timerEvent(QTimerEvent*) override;

 private:
  void ExecNewGameDialog();
  void PauseOrContinue();
  void ExecSettingsDialog();
  void ExecAboutDialog();

  // Creates KeyEvent with specified key. It's used for virtual keys buttons.
  void PressVirtualKey(Qt::Key key);
  // Switches charge button on the screen and type of charge of the player's
  // tank.
  void ChangeChargeButton(int type);

 private:
  void LoadApplicationSettings();
  void LoadTanksTypesInfo();
  void LoadRocketsTypesInfo();

  void SetVirtualKeysEnabled(bool enabled);
  void SetMobileVirtualKeysStyleEnabled(bool enabled);
  void SetMusicEnabled(bool enabled);
  void SetFpsOption(int index, bool start_timer = false);

  // Clears all the data connected with the current round and initializes
  // objects with values necessary to start the new round. After that repaints
  // everything on the screen.
  void ResetGame();
  // Updates info about size of indents in the current window.
  void UpdateIndents();
  // Changes buttons coordinates in the window according to the window size.
  void RedrawButtons();
  // Updates charge buttons color according to the state of charge of the
  // player's tank.
  void RedrawChargeButtons();
  void UpdateScreenTimer();
  // Workaround for the bug with not-adjusting fonts on buttons (Android).
  void AdjustFont(QWidget* widget);

  // Checks if rockets are interacting with the tanks at the moment and
  // destroys rockets / reduces tanks' health.
  void FindInteractingObjects();

  // Checks if given objects have been collided, i.e. they have common points
  // on the screen. Compares its coordinates for that.
  bool HaveObjectsCollided(const std::shared_ptr<Movable>& obj1,
                           const std::shared_ptr<Movable>& obj2) const;
  // Checks if given rocket has been released by the given tank. The result of
  // this function is used when we decide if we need to reduce tank's health
  // (rockets released by the tank can't reduce its own health).
  bool IsRocketByThisTank(const std::shared_ptr<Movable>& rocket,
                          const std::shared_ptr<Movable>& tank) const;
  // Checks if some objects are 'dead', i.e. searches for the tanks with 0
  // health, rockets, which need to be destroyed. Erases such objects.
  // If player's tank is 'dead', calls GameOver().
  void CheckDeadObjects();
  // Runs game over dialog with the appropriate content (i.e 'you win' or
  // 'you lose').
  void GameOver(bool win);

  // Creates a rocket which begins to move from the given tank. Rocket's
  // type is defined by the tank current type of charge. Rocket's geometry on
  // the map is defined by tank's geometry.
  void ShootRocket(const std::shared_ptr<Tank>& tank);
  // Creates and initializes explosion. Its geometry is defined by the
  // given object.
  void MakeBoom(const std::shared_ptr<Movable>& object);
  // Generates bonus of the given type. Erases old bonuses of such type from
  // the map.
  void RandomBonus(Bonus bonus);

  QJsonObject GetJsonObjectFromFile(const QString& filepath);
  // Returns direction object corresponding to the string.
  Direction DetermineDirection(
      const QString& start_direction) const;

 private:
  int current_map_number_ = 0;
  int current_tank_number_ = 0;
  int current_difficulty_level_ = 0;

  std::vector<TankQualities> types_of_tanks_ = {};
  std::vector<RocketParameters> types_of_rockets_ = {};

 private:
  std::shared_ptr<Map> map_;
  // List of all the tanks (including player's tank and all the types of bots).
  // The player's tank, if exists, is always the first item in the list.
  QList<std::shared_ptr<Movable>> tanks_ = {};
  // List of rockets and booms.
  QList<std::shared_ptr<Movable>> rockets_ = {};
  std::vector<std::vector<std::shared_ptr<ObjectOnMap>>>
      obstacles_and_bonuses_ = {};
  // List of 'copies' of tanks. Every item is just a pointer to the real tank
  // and coordinates of the copy. It's used while a tank is moving through the
  // portal (while it must be shown in two places at the same time).
  QList<QPair<std::shared_ptr<Movable>, Coordinates>> objects_copies_ = {};

  int timer_id_ = 0;
  int time_since_last_medicalkit_ = 0;
  int time_since_last_charge_ = 0;
  int timer_duration_;
  bool paused_ = false;

 private:
  bool virtual_keys_enabled_;
  bool mobile_virtual_keys_style_enabled_;
  bool charge_line_enabled_;
  bool music_enabled_;
  int current_fps_option_;

 private:
  QLCDNumber* screen_timer_;
  QPalette standart_lcdnumber_palette_;
  QPalette red_lcdnumber_palette_;
  int screen_timer_ms_ = 0;
  int screen_timer_sec_ = 0;
  int screen_timer_min_ = 0;

  NewGameDialog* new_game_dialog_;
  SettingsDialog* settings_dialog_;
  AboutDialog* about_dialog_;

  QVBoxLayout* main_buttons_layout_;
  QPushButton* new_game_button_;
  QPushButton* pause_continue_button_;
  QPushButton* settings_button_;
  QPushButton* about_button_;

  QHBoxLayout* charge_buttons_layout_;
  QVector<QPushButton*> charge_buttons_;
  QPalette standart_button_palette_;
  QVector<Qt::GlobalColor> charge_colors_{Qt::red, Qt::yellow, Qt::green};
  QVector<QPalette> charge_palettes_;

  QGridLayout* virtual_buttons_layout_;
  QVector<QPushButton*> virtual_keys_buttons_;
  QVector<Qt::Key> virtual_keys_encodings_;
  const int number_of_virtual_keys_in_first_row_ = 2;

  QVBoxLayout* mobile_virtual_buttons_layout_left_;
  QHBoxLayout* mobile_virtual_buttons_layout_right_;

  QMediaPlayer music_player_;
  QMediaPlaylist music_playlist_;

  int sq_width_;
  int sq_height_;
  int w_indent_;
  int h_indent_;
};

#endif  // GAME_CORE_MAINWINDOW_H_
