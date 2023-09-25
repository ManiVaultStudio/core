// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksStatusBarAction.h"
#include "Application.h"

#include <QPainter>
#include <QPaintEvent>

namespace hdps::gui {

TasksStatusBarAction::TasksStatusBarAction(AbstractTasksModel& tasksModel, QObject* parent, const QString& title, const PopupMode& popupMode /*= PopupMode::OnClick*/) :
    WidgetAction(parent, title),
    _tasksModel(tasksModel),
    _tasksFilterModel(this),
    _popupMode(popupMode),
    _menu(),
    _popupForceHidden(false)
{
    _tasksFilterModel.setSourceModel(&_tasksModel);

    setIcon(Application::getIconFont("FontAwesome").getIcon("tasks"));
}

TasksStatusBarAction::Widget::Widget(QWidget* parent, TasksStatusBarAction* tasksStatusBarAction, std::int32_t widgetFlags) :
    WidgetActionWidget(parent, tasksStatusBarAction, widgetFlags),
    _tasksStatusBarAction(tasksStatusBarAction),
    _toolButton(_tasksStatusBarAction, this),
    _tasksPopupWidget(*tasksStatusBarAction, &_toolButton)
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_toolButton);

    setLayout(layout);

    _toolButton.installEventFilter(this);

    auto& menu = _tasksStatusBarAction->getMenu();

    if (!menu.actions().isEmpty()) {
        _toolButton.setMenu(&menu);
        _toolButton.setPopupMode(QToolButton::InstantPopup);
    }

    auto& tasksModel        = _tasksStatusBarAction->getTasksModel();
    auto& tasksFilterModel  = _tasksStatusBarAction->getTasksFilterModel();

    const auto numberOfTasksChanged = [this, &tasksFilterModel]() -> void {
        if (_tasksStatusBarAction->getPopupMode() == PopupMode::Automatic && !_tasksStatusBarAction->getPopupForceHidden()) {
            const auto numberOfTasks = tasksFilterModel.rowCount();
            
            if (numberOfTasks == 0 && _tasksPopupWidget.isVisible())
                _tasksPopupWidget.close();

            if (numberOfTasks >= 1 && !_tasksPopupWidget.isVisible())
                _tasksPopupWidget.show();
        }
    };

    numberOfTasksChanged();

    connect(&tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, numberOfTasksChanged);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, numberOfTasksChanged);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, numberOfTasksChanged);

    const auto updateToolButtonIcon = [this]() -> void {
        _toolButton.setIcon(_tasksStatusBarAction->icon());
    };

    updateToolButtonIcon();

    connect(_tasksStatusBarAction, &TasksStatusBarAction::changed, this, updateToolButtonIcon);

    connect(_tasksStatusBarAction, &TasksStatusBarAction::popupForceHiddenChanged, this, [this, numberOfTasksChanged](bool toggled) -> void {
        if (toggled && _tasksPopupWidget.isVisible())
            _tasksPopupWidget.close();

        numberOfTasksChanged();
    });

    numberOfTasksChanged();
}

bool TasksStatusBarAction::Widget::eventFilter(QObject* target, QEvent* event)
{
    if (_tasksStatusBarAction->getTasksFilterModel().rowCount() == 0 || _tasksStatusBarAction->getPopupMode() != TasksStatusBarAction::PopupMode::Hover || !isEnabled())
        return QWidget::eventFilter(target, event);

    switch (event->type())
    {
        case QEvent::Enter:
            _tasksPopupWidget.show();
            break;

        case QEvent::Leave:
            _tasksPopupWidget.close();
            break;

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

QWidget* TasksStatusBarAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this, widgetFlags);
}

TasksStatusBarAction::PopupMode TasksStatusBarAction::getPopupMode() const
{
    return _popupMode;
}

void TasksStatusBarAction::setPopupMode(const PopupMode& popupMode)
{
    _popupMode = popupMode;
}

QMenu& TasksStatusBarAction::getMenu()
{
    return _menu;
}

bool TasksStatusBarAction::getPopupForceHidden() const
{
    return _popupForceHidden;
}

void TasksStatusBarAction::setPopupForceHidden(bool popupForceHidden)
{
    if (popupForceHidden == _popupForceHidden)
        return;

    _popupForceHidden = popupForceHidden;

    emit popupForceHiddenChanged(_popupForceHidden);
}

AbstractTasksModel& TasksStatusBarAction::getTasksModel()
{
    return _tasksModel;
}

TasksFilterModel& TasksStatusBarAction::getTasksFilterModel()
{
    return _tasksFilterModel;
}

TasksStatusBarAction::Widget::ToolButton::ToolButton(TasksStatusBarAction* tasksStatusBarAction, QWidget* parent /*= nullptr*/) :
    QToolButton(parent),
    _tasksStatusBarAction(tasksStatusBarAction)
{
    setAutoRaise(true);
    setStyleSheet("QToolButton::menu-indicator { image: none; }");
}

void TasksStatusBarAction::Widget::ToolButton::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    const auto margin   = 4;
    const auto icon     = _tasksStatusBarAction->icon();
    const auto height   = paintEvent->rect().size().height();
    const auto rect     = QSize(height, height);

    if (icon.isNull())
        return;

    painter.drawPixmap(QPoint(margin, margin), icon.pixmap(rect - 2 * QSize(margin, margin)));// .scaled(size() - 2 * QSize(margin, margin), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

}
