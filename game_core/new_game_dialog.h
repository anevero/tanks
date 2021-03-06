#ifndef GAME_CORE_NEW_GAME_DIALOG_H_
#define GAME_CORE_NEW_GAME_DIALOG_H_

#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QString>
#include <QVBoxLayout>
#include <vector>

class NewGameDialog : public QDialog {
  Q_OBJECT

 public:
  explicit NewGameDialog(QWidget* parent = nullptr);
  ~NewGameDialog() override = default;

  void accept() override;
  void reject() override;

  int GetCurrentMapNumber() const;
  int GetCurrentTankNumber() const;
  int GetCurrentDifficultyLevel() const;

 private:
  int current_map_number_ = 0;
  int current_tank_number_ = 0;
  int current_difficulty_level_ = 0;

  QVBoxLayout* layout_;
  QComboBox* switch_map_menu_;
  QComboBox* switch_tank_menu_;
  QComboBox* switch_difficulty_menu_;
  QLabel* new_game_label_;
  QLabel* switch_map_label_;
  QLabel* switch_tank_label_;
  QLabel* switch_difficulty_label_;
  QDialogButtonBox* buttons_;

  const std::vector<QString> kDifficultyLevelsNames =
      {tr("Easy"), tr("Normal"), tr("Hard")};
};

#endif  // GAME_CORE_NEW_GAME_DIALOG_H_
