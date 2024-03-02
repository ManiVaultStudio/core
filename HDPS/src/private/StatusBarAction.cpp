// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StatusBarAction.h"

#ifdef _DEBUG
    #define STATUS_BAR_ACTION_VERBOSE
#endif

#include <QDebug>

using namespace mv;
using namespace mv::gui;

StatusBarAction::StatusBarAction(QObject* parent, const QString& title, const QString& icon /*= ""*/) :
    WidgetAction(parent, title),
    _barGroupAction(this, "Bar Group"),
    _iconAction(this, "Icon"),
    _popupAction(nullptr)
{
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);

    _barGroupAction.setShowLabels(false);
    _barGroupAction.setWidgetConfigurationFunction([](WidgetAction* action, QWidget* widget) -> void {
        auto horizontalWidget = widget->findChild<QWidget*>("HorizontalWidget");

        Q_ASSERT(horizontalWidget != nullptr);

        if (horizontalWidget == nullptr)
            return;

        auto horizontalLayout = horizontalWidget->layout();

        horizontalLayout->setContentsMargins(2, 0, 2, 0);
        horizontalLayout->setSpacing(4);
    });

    if (!icon.isEmpty()) {
        _barGroupAction.addAction(&_iconAction);

        _iconAction.setEnabled(false);
        _iconAction.setDefaultWidgetFlags(StringAction::Label);
        _iconAction.setString(icon);
        _iconAction.setWidgetConfigurationFunction([this](WidgetAction* action, QWidget* widget) -> void {
            auto labelWidget = widget->findChild<QLabel*>("Label");

            Q_ASSERT(labelWidget != nullptr);

            if (labelWidget == nullptr)
                return;

            labelWidget->setFont(Application::getIconFont("FontAwesome").getFont(9));
        });
    }

    const auto tooltipChanged = [this]() -> void {
        _barGroupAction.setToolTip(toolTip());
        _iconAction.setToolTip(toolTip());
    };

    tooltipChanged();

    connect(this, &WidgetAction::changed, this, tooltipChanged);
}

QWidget* StatusBarAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this, widgetFlags);
}

HorizontalGroupAction& StatusBarAction::getBarGroupAction()
{
    return _barGroupAction;
}

StringAction& StatusBarAction::getBarIconStringAction()
{
    return _iconAction;
}

WidgetAction* StatusBarAction::getPopupAction()
{
    return _popupAction;
}

void StatusBarAction::setPopupAction(WidgetAction* popupAction)
{
    auto previousPopupAction = _popupAction;

    _popupAction = popupAction;

    if (_popupAction)
        _popupAction->setConfigurationFlag(WidgetAction::ConfigurationFlag::ToolButtonAutoRaise);

    emit popupActionChanged(previousPopupAction, _popupAction);
}

void StatusBarAction::showPopup()
{
#ifdef STATUS_BAR_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << text();
#endif

    emit requirePopupShow();
}

void StatusBarAction::hidePopup()
{
#ifdef STATUS_BAR_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << text();
#endif

    emit requirePopupHide();
}

StatusBarAction::Widget::Widget(QWidget* parent, StatusBarAction* statusBarAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, statusBarAction, widgetFlags),
    _statusBarAction(statusBarAction),
    _toolButton(this, statusBarAction),
    _popupWidget(nullptr)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(&_toolButton);

    setLayout(layout);

    _toolButton.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
    _toolButton.setAction(_statusBarAction->getPopupAction());
    _toolButton.setAutoRaise(true);
    _toolButton.setShowIndicator(false);

    connect(_statusBarAction, &StatusBarAction::popupActionChanged, this, [this](WidgetAction* previousPopupAction, WidgetAction* popupAction) -> void {
        _toolButton.setAction(popupAction);
    });

    connect(&_toolButton, &ToolButton::clicked, statusBarAction, &StatusBarAction::toolButtonClicked);

    connect(_statusBarAction, &StatusBarAction::requirePopupShow, this, [this]() -> void {
        //_toolButton.click();

        auto popupWidget = _toolButton.getAction()->requestWidget(&_toolButton);

        popupWidget->show();
        popupWidget->raise();

        //Application::processEvents();

        for (auto childWidget : _toolButton.findChildren<QObject*>()) { // Qt::FindDirectChildrenOnly
            if (childWidget->property("Popup").isValid() && childWidget->property("Popup").toBool()) {
                //_popupWidget = childWidget;
                break;
            }
        }
    });

    connect(_statusBarAction, &StatusBarAction::requirePopupHide, this, [this]() -> void {
        if (_toolButton.menu())
            qDebug() << "---HAS_MENU---";

        //_toolButton.hide();
    });

    const auto statusBarEnabledChanged = [this]() -> void {
        _toolButton.setEnabled(_statusBarAction->isEnabled());
    };

    statusBarEnabledChanged();

    connect(statusBarAction, &WidgetAction::enabledChanged, this, statusBarEnabledChanged);
}

StatusBarAction::Widget::ToolButton::ToolButton(QWidget* parent, StatusBarAction* statusBarAction) :
    WidgetActionToolButton(parent, statusBarAction->getPopupAction()),
    _statusBarAction(statusBarAction)
{
    auto layout = new QVBoxLayout();

    layout->setSizeConstraint(QLayout::SetMinimumSize);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(statusBarAction->getBarGroupAction().createWidget(this));

    setLayout(layout);
}
