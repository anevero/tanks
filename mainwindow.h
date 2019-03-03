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
  void paintEvent(QPaintEvent *event) override;

private slots:
  void RedrawMap();

private:
  QPushButton *new_game_button_;
  QComboBox *swith_map_menu_;
  Map map_ = Map(1);
};

#endif // MAINWINDOW_H
