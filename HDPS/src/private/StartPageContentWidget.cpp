#include "StartPageContentWidget.h"
#include "StartPageWidget.h"

#include <QDebug>

using namespace hdps::gui;

StartPageContentWidget::StartPageContentWidget(QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _mainLayout(),
    _collumnsLayout(),
    _toolbarLayout(),
    _openProjectWidget(this),
    _getStartedWidget(this),
    _compactViewAction(this, "Compact view")
{
    setAutoFillBackground(true);

    StartPageWidget::setWidgetBackgroundColorRole(this, QPalette::Midlight);

    _collumnsLayout.setContentsMargins(35, 35, 35, 35);
    _toolbarLayout.setContentsMargins(35, 10, 35, 10);

    _collumnsLayout.addWidget(&_openProjectWidget, 1);
    _collumnsLayout.addWidget(&_getStartedWidget, 1);

    _mainLayout.addLayout(&_collumnsLayout, 1);
    _mainLayout.addLayout(&_toolbarLayout);

    _toolbarLayout.addWidget(_compactViewAction.createWidget(this), 1);

    setLayout(&_mainLayout);

    _compactViewAction.setSettingsPrefix("StartPage/CompactView");

    connect(&_compactViewAction, &ToggleAction::toggled, this, &StartPageContentWidget::updateActions);
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
    StartPageAction::setCompactView(_compactViewAction.isChecked());

    _openProjectWidget.updateActions();
    _getStartedWidget.updateActions();
}
