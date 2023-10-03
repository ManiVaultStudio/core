// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksStatusBarAction.h"
#include "Application.h"

#include <QPainter>
#include <QPaintEvent>

#ifdef _DEBUG
    #define TASKS_STATUS_BAR_ACTION_VERBOSE
#endif

namespace hdps::gui {

TasksStatusBarAction::TasksStatusBarAction(AbstractTasksModel& tasksModel, QObject* parent, const QString& title, const QIcon& icon /*= QIcon()*/, const PopupMode& popupMode /*= PopupMode::Click*/, const Task::Scope& taskScope /*= Task::Scope::Foreground*/) :
    WidgetAction(parent, title),
    _tasksModel(tasksModel),
    _tasksFilterModel(this),
    _popupMode(popupMode),
    _taskScope(taskScope),
    _menu(),
    _popupForceHidden(false)
{
    _tasksFilterModel.setSourceModel(&_tasksModel);

    setIcon(icon);
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

    auto& tasksFilterModel = _tasksStatusBarAction->getTasksFilterModel();

    const auto numberOfTasksChanged = [this, &tasksFilterModel]() -> void {
        const auto numberOfTasks = tasksFilterModel.rowCount();

#ifdef TASKS_STATUS_BAR_ACTION_VERBOSE
        qDebug() << _tasksStatusBarAction->text() << "Tasks filter model number of rows changed to " << numberOfTasks;
#endif

        if (_tasksStatusBarAction->getPopupMode() == PopupMode::Automatic && !_tasksStatusBarAction->getPopupForceHidden()) {
            if (numberOfTasks == 0 && _tasksPopupWidget.isVisible()) {
#ifdef TASKS_STATUS_BAR_ACTION_VERBOSE
                qDebug() << _tasksStatusBarAction->text() << "close tasks popup widget";
#endif

                _tasksPopupWidget.close();
            }

            if (numberOfTasks >= 1 && !_tasksPopupWidget.isVisible()) {
#ifdef TASKS_STATUS_BAR_ACTION_VERBOSE
                qDebug() << _tasksStatusBarAction->text() << "open tasks popup widget";

#endif

                _tasksPopupWidget.updateContents();
                _tasksPopupWidget.show();
            }
        }
    };

    numberOfTasksChanged();

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, [this, numberOfTasksChanged](const QModelIndex& parent, int first, int last) -> void {
#ifdef TASKS_STATUS_BAR_ACTION_VERBOSE
        for (int rowIndex = first; rowIndex < last; rowIndex++)
            qDebug() << _tasksStatusBarAction->text() << _tasksStatusBarAction->getTasksFilterModel().index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name)).data(Qt::DisplayRole).toString() << " inserted into TasksFilterModel";
#endif

        numberOfTasksChanged();
    });

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, [this, numberOfTasksChanged](const QModelIndex& parent, int first, int last) -> void {
#ifdef TASKS_STATUS_BAR_ACTION_VERBOSE
        for (int rowIndex = first; rowIndex < last; rowIndex++)
            qDebug() << _tasksStatusBarAction->text() << _tasksStatusBarAction->getTasksFilterModel().index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name)).data(Qt::DisplayRole).toString() << " removed from TasksFilterModel";
#endif

        numberOfTasksChanged();
    });

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

Task::Scope TasksStatusBarAction::getTaskScope() const
{
    return _taskScope;
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

    painter.drawPixmap(QPoint(margin, margin), icon.pixmap(rect - 2 * QSize(margin, margin)));
}

}
