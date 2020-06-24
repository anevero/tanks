#include "about_dialog.h"

#include <QScroller>
#include <QUrl>

AboutDialog::AboutDialog(QWidget* parent)
    : QDialog(parent),
      layout_(new QVBoxLayout(this)),
      html_widget_(new QTextBrowser(this)),
      buttons_(new QDialogButtonBox(
          QDialogButtonBox::Ok, Qt::Horizontal, this)) {
  html_widget_->setSource(QUrl("qrc:/rules/rules.html"));
  html_widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  html_widget_->setOpenExternalLinks(true);

#ifdef Q_OS_ANDROID
  html_widget_->setTextInteractionFlags(
      Qt::LinksAccessibleByMouse | Qt::LinksAccessibleByKeyboard);
  QScroller::grabGesture(html_widget_, QScroller::TouchGesture);
#endif

  layout_->addWidget(html_widget_);
  layout_->addWidget(buttons_);
  connect(buttons_, &QDialogButtonBox::accepted, this, &AboutDialog::accept);
  connect(buttons_, &QDialogButtonBox::rejected, this, &AboutDialog::reject);
}
