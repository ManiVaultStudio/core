#include "StartPageHeaderWidget.h"
#include "StartPageWidget.h"

#include <QDebug>

using namespace hdps;
using namespace hdps::gui;

StartPageHeaderWidget::StartPageHeaderWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _headerLabel()
{
    setLayout(&_layout);
    setAutoFillBackground(true);

    const int pixelRatio = devicePixelRatio();

    QString iconName = ":/Images/AppBackground256";

    if (pixelRatio > 1)
        iconName = ":/Images/AppBackground512";

    if (pixelRatio > 2)
        iconName = ":/Images/AppBackground1024";

    _headerLabel.setPixmap(QPixmap(iconName).scaled(256, 256));
    _headerLabel.setAlignment(Qt::AlignCenter);

    _layout.setContentsMargins(50, 50, 50, 50);
    _layout.addWidget(&_headerLabel);

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);
}
