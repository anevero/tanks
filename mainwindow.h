#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "map.h"
#include <QComboBox>
#include <QEvent>
#include <QFileInfo>
#include <QMainWindow>
#include <QPainter>
#include <QPushButton>
#include <QString>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow() override;

private:
  void paintEvent(QPaintEvent *) override;
  void resizeEvent(QResizeEvent *) override;

private slots:
  void UpdateIndents();
  void RedrawButtons();
  void RedrawMap();

private:
  QPushButton *new_game_button_;
  QComboBox *swith_map_menu_;
  Map map_ = Map(1);

  // размеры и отступы прямоугольника 4*3 внутри окна, в котором
  // отрисовываются все элементы (для того, чтобы клетки были квадратными)
  int sq_width;
  int sq_height;
  int w_indent;
  int h_indent;
};

#endif // MAINWINDOW_H
