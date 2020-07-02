#ifndef GAME_CORE_ABOUT_DIALOG_H_
#define GAME_CORE_ABOUT_DIALOG_H_

#include <QDialog>
#include <QDialogButtonBox>
#include <QTextBrowser>
#include <QVBoxLayout>

class AboutDialog : public QDialog {
  Q_OBJECT

 public:
  explicit AboutDialog(QWidget* parent = nullptr);
  ~AboutDialog() override = default;

  void accept() override;
  void reject() override;

 private:
  void ResetCursor();

  QVBoxLayout* layout_;
  QTextBrowser* html_widget_;
  QDialogButtonBox* buttons_;
};

#endif  // GAME_CORE_ABOUT_DIALOG_H_
