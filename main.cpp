#include <QApplication>
#include <QStyleFactory>
#include <QTranslator>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setStyle(QStyleFactory::create("Fusion"));

  QTranslator translator;
  if (!translator.load(":/translations/tanks_" + QLocale::system().name())) {
    translator.load(":/translations/tanks_en_US");
  }
  a.installTranslator(&translator);

  MainWindow w;
  w.setWindowTitle("Tanks");
  w.show();

  return a.exec();
}
