// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "BackgroundTasksStatusBarAction.h"
#include "BackgroundTaskHandler.h"
#include "Application.h"

#include <QPainter>

namespace hdps::gui {

BackgroundTasksStatusBarAction::BackgroundTasksStatusBarAction(QObject* parent, const QString& title) :
    gui::HorizontalGroupAction(parent, title),
    _taskAction(this, "Overall background task"),
    _detailsAction(this, "Details")
{
    setShowLabels(false);

    addAction(&_taskAction);
    addAction(&_detailsAction);
}

BackgroundTasksStatusBarAction::DetailsAction::DetailsAction(QObject* parent, const QString& title) :
    WidgetAction(parent, title)
{
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
}

QWidget* BackgroundTasksStatusBarAction::DetailsAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    //auto tasksPopupWidget = new TasksPopupWidget(nullptr, parent);

    //auto& tasksAction       = tasksPopupWidget->getTasksAction();
    //auto& tasksFilterModel  = tasksAction.getTasksFilterModel();

    //tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Background" });
    //tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running Indeterminate", "Running", "Finished" });

    return nullptr;
}

BackgroundTasksStatusBarAction::DetailsAction::StatusBarButton::StatusBarButton(QWidget* parent /*= nullptr*/) :
    QToolButton(parent)
{
    setAutoRaise(true);
    setStyleSheet("QToolButton::menu-indicator { image: none; }");
    setPopupMode(QToolButton::InstantPopup);
}

void BackgroundTasksStatusBarAction::DetailsAction::StatusBarButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    QPainter painter(this);

    const auto margin = 3;
    const auto icon = this->icon();

    if (icon.isNull())
        return;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(QPoint(margin, margin), icon.pixmap(icon.availableSizes().first()).scaled(size() - 2 * QSize(margin, margin), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

}
