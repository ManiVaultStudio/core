// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "StatusBarAction.h"

#include "CoreInterface.h"

#ifdef _DEBUG
    #define STATUS_BAR_ACTION_VERBOSE
#endif

#include <QDebug>

using namespace mv::util;

namespace mv::gui {

WidgetActions StatusBarAction::statusBarActions;

StatusBarAction::StatusBarAction(QObject* parent, const QString& title, const StyledIcon& icon /*= StyledIcon()*/) :
    WidgetAction(parent, title),
    _barGroupAction(this, "Bar Group"),
    _iconAction(this, "Icon"),
    _popupAction(nullptr),
    _index(-1)
{
    initialize(icon);

    StatusBarAction::statusBarActions << this;

    /* TODO: Fix plugin status bar action visibility
    const auto updateVisibility = [this]() -> void {
        setVisible(mv::settings().getMiscellaneousSettings().getStatusBarOptionsAction().isOptionSelected(text()));
    };

    updateVisibility();

    connect(&mv::settings().getMiscellaneousSettings().getStatusBarOptionsAction(), &OptionsAction::selectedOptionsChanged, this, updateVisibility);

    mv::settings().getMiscellaneousSettings().updateStatusBarOptionsAction();
    */
}

StatusBarAction::StatusBarAction(QObject* parent, const QString& title, const QString& icon /*= ""*/) :
    StatusBarAction(parent, title, icon.isEmpty() ? StyledIcon() : StyledIcon(icon))
{
}

StatusBarAction::~StatusBarAction()
{
    if (!core())
        return;

    if (!core()->isInitialized())
        return;

    StatusBarAction::statusBarActions.removeOne(this);

    /* TODO: Fix plugin status bar action visibility
    if (!core()->isAboutToBeDestroyed())
		settings().getMiscellaneousSettings().updateStatusBarOptionsAction();
	*/
}

WidgetActions StatusBarAction::getStatusBarActions()
{
    return statusBarActions;
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

void StatusBarAction::addMenuAction(WidgetAction* menuAction)
{
    Q_ASSERT(menuAction);

    if (!menuAction)
        return;

    _menuActions << menuAction;

    emit menuActionAdded(menuAction);
}

void StatusBarAction::removeMenuAction(WidgetAction* menuAction)
{
    Q_ASSERT(menuAction);

    if (!menuAction)
        return;

    emit menuActionAboutToBeRemoved(menuAction);

    _menuActions.removeOne(menuAction);
}

WidgetActions StatusBarAction::getMenuActions()
{
    return _menuActions;
}

std::int32_t StatusBarAction::getIndex()
{
    return _index;
}

void StatusBarAction::setIndex(std::int32_t index)
{
    if (index == _index)
        return;

    _index = index;

    emit indexChanged(_index);
}

void StatusBarAction::initialize(const QIcon& icon)
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

    if (!icon.isNull()) {
        _barGroupAction.addAction(&_iconAction);

        _iconAction.setEnabled(false);
        _iconAction.setDefaultWidgetFlags(StringAction::Label);
        _iconAction.setWidgetConfigurationFunction([this, icon](WidgetAction* action, QWidget* widget) -> void {
            auto labelWidget = widget->findChild<QLabel*>("Label");

            Q_ASSERT(labelWidget != nullptr);

            if (labelWidget == nullptr)
                return;

            labelWidget->setPixmap(icon.pixmap(QSize(12, 12)));
        });
    }

    const auto tooltipChanged = [this]() -> void {
        _barGroupAction.setToolTip(toolTip());
        _iconAction.setToolTip(toolTip());
    };

    tooltipChanged();

    connect(this, &WidgetAction::changed, this, tooltipChanged);

    /* TODO: Fix plugin status bar action visibility
    const auto updateStatusBarActionsVisibility = [this]() -> void {
        const auto selectedOptions = mv::settings().getMiscellaneousSettings().getStatusBarOptionsAction().getSelectedOptions();

        for (auto statusBarAction : StatusBarAction::statusBarActions)
            statusBarAction->setVisible(selectedOptions.contains(statusBarAction->text()));
    };

    updateStatusBarActionsVisibility();

    connect(&mv::settings().getMiscellaneousSettings().getStatusBarOptionsAction(), &OptionsAction::selectedOptionsChanged, this, updateStatusBarActionsVisibility);
    */
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
    _toolButton.setAutoRaise(true);
    _toolButton.setShowIndicator(false);

    if (auto popupAction = _statusBarAction->getPopupAction())
        _toolButton.setAction(popupAction);

    connect(_statusBarAction, &StatusBarAction::popupActionChanged, this, [this](WidgetAction* previousPopupAction, WidgetAction* popupAction) -> void {
        _toolButton.setAction(popupAction);
    });

    connect(_statusBarAction, &StatusBarAction::requirePopupShow, this, [this]() -> void {
        //_toolButton.getMenu().popup(_toolButton.mapToGlobal(_toolButton.pos()));// .show();
    });

    connect(_statusBarAction, &StatusBarAction::requirePopupHide, this, [this]() -> void {
        //_toolButton.getMenu().hide();
    });

    const auto statusBarEnabledChanged = [this]() -> void {
        _toolButton.setEnabled(_statusBarAction->isEnabled());
    };

    statusBarEnabledChanged();

    connect(statusBarAction, &WidgetAction::enabledChanged, this, statusBarEnabledChanged);

    for (auto menuAction : _statusBarAction->getMenuActions())
        _toolButton.getMenu().addAction(menuAction);

    connect(_statusBarAction, &StatusBarAction::menuActionAdded, this, [this](WidgetAction* menuAction) -> void {
        _toolButton.getMenu().addAction(menuAction);
    });

    connect(_statusBarAction, &StatusBarAction::menuActionAboutToBeRemoved, this, [this](WidgetAction* menuAction) -> void {
        _toolButton.getMenu().removeAction(menuAction);
    });
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

}