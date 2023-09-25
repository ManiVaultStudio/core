// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksPopupWidget.h"
#include "TasksStatusBarAction.h"
#include "Application.h"

#include "util/Icon.h"

#include <QPainter>
#include <QMainWindow>
#include <QToolButton>

namespace hdps::gui {

const QSize TasksPopupWidget::iconPixmapSize = QSize(64, 64);

TasksPopupWidget::TasksPopupWidget(gui::TasksStatusBarAction& tasksStatusBarAction, QWidget* anchorWidget /*= nullptr*/, QWidget* parent /*= nullptr*/, std::uint32_t minimumDuration /*= 250*/) :
    QWidget(parent),
    _tasksStatusBarAction(tasksStatusBarAction),
    _anchorWidget(anchorWidget),
    _tasksIconPixmap(tasksStatusBarAction.icon().pixmap(iconPixmapSize))
{
    setObjectName("TasksPopupWidget");
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet(QString("QWidget#TasksPopupWidget { border: 1px solid %1; }").arg(palette().color(QPalette::Normal, QPalette::Mid).name(QColor::HexRgb)));

    _minimumDurationTimer.setSingleShot(true);
    _minimumDurationTimer.setInterval(minimumDuration);

    setLayout(new QVBoxLayout());

    auto& tasksModel        = _tasksStatusBarAction.getTasksModel();
    auto& tasksFilterModel  = _tasksStatusBarAction.getTasksFilterModel();

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

    synchronizeWithAnchorWidget();
    updateIcon();
    numberOfTasksChanged();

    if (_anchorWidget) {
        installEventFilter(this);
        _anchorWidget->installEventFilter(this);
        getMainWindow()->installEventFilter(this);
    }
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
            if (targetWidget == _anchorWidget || targetWidget == getMainWindow())
                synchronizeWithAnchorWidget();

            break;
        }

        case QEvent::Resize:
        {
            if (targetWidget == this)
                synchronizeWithAnchorWidget();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

void TasksPopupWidget::updateIcon()
{
    QPixmap iconPixmap(iconPixmapSize);

    iconPixmap.fill(Qt::transparent);

    QPainter painter(&iconPixmap);

    const auto scaledTasksIconPixmapSize = iconPixmapSize - 0.25 * iconPixmapSize;

    painter.drawPixmap(QPoint(0, 0), _tasksIconPixmap.scaled(scaledTasksIconPixmapSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    const auto numberOfTasks = _tasksStatusBarAction.getTasksFilterModel().rowCount();

    const auto badgeRadius = 43.0;

    painter.setPen(QPen(QColor(numberOfTasks == 0 ? 0 : 255, 0, 0, 255), badgeRadius, Qt::SolidLine, Qt::RoundCap));

    const auto center = QPoint(iconPixmapSize.width() - (badgeRadius / 2), iconPixmapSize.height() - (badgeRadius / 2));

    painter.drawPoint(center);

    painter.setPen(QPen(Qt::white));
    painter.setFont(QFont("Arial", numberOfTasks >= 10 ? 18 : 24, 900));

    const auto textRectangleSize = QSize(32, 32);
    const auto textRectangle = QRectF(center - QPointF(textRectangleSize.width() / 2.f, textRectangleSize.height() / 2.f), textRectangleSize);

    painter.drawText(textRectangle, QString::number(numberOfTasks), QTextOption(Qt::AlignCenter));

    _tasksStatusBarAction.setIcon(createIcon(iconPixmap));
}

void TasksPopupWidget::synchronizeWithAnchorWidget()
{
    if (_anchorWidget == nullptr)
        return;

    move(_anchorWidget->mapToGlobal(QPoint(_anchorWidget->width(), 0)) - QPoint(width(), height()));
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
    auto& tasksModel        = _tasksStatusBarAction.getTasksModel();
    auto& tasksFilterModel  = _tasksStatusBarAction.getTasksFilterModel();

    const auto numberOfTasks = tasksFilterModel.rowCount();

    if (numberOfTasks == 0 && isVisible())
        close();

    cleanLayout();

    QVector<Task*> currentTasks;

    for (int rowIndex = 0; rowIndex < numberOfTasks; ++rowIndex) {
        const auto sourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (!sourceModelIndex.isValid())
            continue;

        auto progressItem = dynamic_cast<AbstractTasksModel::ProgressItem*>(tasksModel.itemFromIndex(sourceModelIndex));

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

    updateIcon();

    if (_anchorWidget)
        _anchorWidget->setToolTip(QString("%1 task(s)").arg(QString::number(numberOfTasks)));
}

void TasksPopupWidget::setIcon(const QIcon& icon)
{
    _tasksIconPixmap = icon.pixmap(iconPixmapSize);
}

}
