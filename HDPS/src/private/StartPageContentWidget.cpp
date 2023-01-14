#include "StartPageContentWidget.h"
#include "StartPageWidget.h"

#include <QDebug>

StartPageContentWidget::StartPageContentWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _layout(),
    _openProjectWidget(this),
    _getStartedWidget(this)
{
    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    _layout.setContentsMargins(35, 35, 35, 35);

    _layout.addWidget(&_openProjectWidget, 1);
    _layout.addWidget(&_getStartedWidget, 1);

    setLayout(&_layout);
}

QLabel* StartPageContentWidget::createHeaderLabel(const QString& title, const QString& tooltip)
{
    auto label = new QLabel(title);

    label->setAlignment(Qt::AlignLeft);
    label->setStyleSheet("QLabel { font-weight: 200; font-size: 13pt; }");
    label->setToolTip(tooltip);

    return label;
}

void StartPageContentWidget::updateActions()
{
    _openProjectWidget.updateActions();
    _getStartedWidget.updateActions();
}
