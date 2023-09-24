// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksPopupWidget.h"
#include "Application.h"

#include "util/Icon.h"

#include <QPainter>
#include <QMainWindow>
#include <QToolButton>

using namespace hdps::gui;

namespace hdps {

const QSize TasksPopupWidget::iconPixmapSize = QSize(64, 64);

TasksPopupWidget::TasksPopupWidget(AbstractTaskHandler* taskHandler, QToolButton* toolButton, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _taskHandler(taskHandler),
    _toolButton(toolButton),
    _tasksAction(this, "Foreground Tasks"),
    _tasksIconPixmap(Application::getIconFont("FontAwesome").getIcon("tasks").pixmap(iconPixmapSize))
{
    Q_ASSERT(_taskHandler != nullptr);

    if (!_taskHandler)
        return;

    Q_ASSERT(_toolButton != nullptr);

    if (!_toolButton)
        return;

    setObjectName("ForegroundTasksPopupWidget");
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet(QString("QWidget#ForegroundTasksPopupWidget { border: 1px solid %1; }").arg(palette().color(QPalette::Normal, QPalette::Mid).name(QColor::HexRgb)));

    _minimumDurationTimer.setSingleShot(true);

    const auto updateMinimumDurationTimer = [this]() -> void {
        _minimumDurationTimer.setInterval(_taskHandler->getMinimumDuration());
    };

    updateMinimumDurationTimer();

    connect(_taskHandler, &AbstractTaskHandler::minimumDurationChanged, this, updateMinimumDurationTimer);

    setLayout(new QVBoxLayout());

    auto& tasksModel        = _tasksAction.getTasksModel();
    auto& tasksFilterModel  = _tasksAction.getTasksFilterModel();

    const auto numberOfTasksChangedDeferred = [this]() -> void {
        if (isHidden() && !_minimumDurationTimer.isActive())
            _minimumDurationTimer.start();

        if (isVisible())
            numberOfTasksChanged();
    };

    connect(&_minimumDurationTimer, &QTimer::timeout, this, &TasksPopupWidget::numberOfTasksChanged);

    connect(&tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, numberOfTasksChangedDeferred);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, numberOfTasksChangedDeferred);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, numberOfTasksChangedDeferred);

    synchronizeWithToolButton();
    updateToolButtonIcon();
    numberOfTasksChanged();

    installEventFilter(this);
    _toolButton->installEventFilter(this);
    getMainWindow()->installEventFilter(this);
}

bool TasksPopupWidget::eventFilter(QObject* target, QEvent* event)
{
    auto targetWidget = qobject_cast<QWidget*>(target);

    if (targetWidget == nullptr)
        return QObject::eventFilter(target, event);

    switch (event->type())
    {
        case QEvent::Move:
        {
            if (targetWidget == _toolButton || targetWidget == getMainWindow())
                synchronizeWithToolButton();

            break;
        }

        case QEvent::Resize:
        {
            if (targetWidget == this)
                synchronizeWithToolButton();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

void TasksPopupWidget::updateToolButtonIcon()
{
    QPixmap iconPixmap(iconPixmapSize);

    iconPixmap.fill(Qt::transparent);

    QPainter painter(&iconPixmap);

    const auto scaledTasksIconPixmapSize = iconPixmapSize - 0.25 * iconPixmapSize;

    painter.drawPixmap(QPoint(0, 0), _tasksIconPixmap.scaled(scaledTasksIconPixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    const auto numberOfTasks = _tasksAction.getTasksFilterModel().rowCount();

    const auto badgeRadius = 43.0;

    painter.setPen(QPen(QColor(numberOfTasks == 0 ? 0 : 255, 0, 0, 255), badgeRadius, Qt::SolidLine, Qt::RoundCap));

    const auto center = QPoint(iconPixmapSize.width() - (badgeRadius / 2), iconPixmapSize.height() - (badgeRadius / 2));

    painter.drawPoint(center);

    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Arial", numberOfTasks >= 10 ? 18 : 24, 900));

    const auto textRectangleSize = QSize(32, 32);
    const auto textRectangle = QRectF(center - QPointF(textRectangleSize.width() / 2.f, textRectangleSize.height() / 2.f), textRectangleSize);

    painter.drawText(textRectangle, QString::number(numberOfTasks), QTextOption(Qt::AlignCenter));

    _toolButton->setIcon(createIcon(iconPixmap));
}

void TasksPopupWidget::synchronizeWithToolButton()
{
    move(_toolButton->mapToGlobal(QPoint(_toolButton->width(), 0)) - QPoint(width(), height()));
}

QMainWindow* TasksPopupWidget::getMainWindow()
{
    foreach(QWidget * widget, qApp->topLevelWidgets())
        if (auto mainWindow = qobject_cast<QMainWindow*>(widget))
            return mainWindow;

    return nullptr;
}

QSize TasksPopupWidget::sizeHint() const
{
    return QSize(500, 0);
}

void TasksPopupWidget::cleanLayout()
{
    QLayoutItem* item;

    while ((item = this->layout()->takeAt(0)) != 0)
        delete item;
}

void TasksPopupWidget::numberOfTasksChanged()
{
    auto& tasksModel = _tasksAction.getTasksModel();
    auto& tasksFilterModel = _tasksAction.getTasksFilterModel();

    const auto numberOfTasks = tasksFilterModel.rowCount();

    if (numberOfTasks == 0 && isVisible())
        close();

    if (numberOfTasks >= 1 && !isVisible() && !isEnabled())
        show();

    cleanLayout();

    QVector<Task*> currentTasks;

    for (int rowIndex = 0; rowIndex < numberOfTasks; ++rowIndex) {
        const auto sourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, static_cast<int>(TasksModel::Column::Progress)));

        if (!sourceModelIndex.isValid())
            continue;

        auto progressItem = dynamic_cast<TasksModel::ProgressItem*>(tasksModel.itemFromIndex(sourceModelIndex));

        Q_ASSERT(progressItem != nullptr);

        if (progressItem == nullptr)
            continue;

        currentTasks << progressItem->getTask();

        QWidget* taskWidget = nullptr;

        if (!_widgetsMap.contains(progressItem->getTask())) {
            _widgetsMap[progressItem->getTask()] = progressItem->getTaskAction().createWidget(this);
        }
        else {
            taskWidget = _widgetsMap[progressItem->getTask()];
        }

        layout()->addWidget(_widgetsMap[progressItem->getTask()]);
    }

    for (auto task : _widgetsMap.keys()) {
        if (currentTasks.contains(task))
            continue;

        delete _widgetsMap[task];
        _widgetsMap.remove(task);
    }

    adjustSize();

    updateToolButtonIcon();

    _toolButton->setToolTip(QString("%1 task(s)").arg(QString::number(numberOfTasks)));
}

}
