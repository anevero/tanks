#include <QApplication>
#include <QStyleFactory>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setStyle(QStyleFactory::create("Fusion"));
  MainWindow w;
  w.setWindowTitle("Tanks");
  w.show();

  return a.exec();
}
