// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "TasksPopupWidget.h"
#include "TasksStatusBarAction.h"
#include "Application.h"
#include "CoreInterface.h"

#include "util/Icon.h"

#include <QPainter>
#include <QMainWindow>
#include <QToolButton>

#ifdef _DEBUG
    //#define TASKS_POPUP_WIDGET_VERBOSE
#endif

namespace mv::gui {

const QSize TasksPopupWidget::iconPixmapSize = QSize(64, 64);

TasksPopupWidget::TasksPopupWidget(gui::TasksStatusBarAction& tasksStatusBarAction, QWidget* anchorWidget /*= nullptr*/, QWidget* parent /*= nullptr*/, std::uint32_t minimumDuration /*= 250*/) :
    QWidget(parent),
    _tasksStatusBarAction(tasksStatusBarAction),
    _anchorWidget(anchorWidget),
    _tasksIconPixmap(tasksStatusBarAction.icon().pixmap(iconPixmapSize)),
    _widgetsMap(),
    _minimumDurationTimer()
{
    setObjectName("TasksPopupWidget");
    
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowFlag(Qt::SubWindow);

    setStyleSheet(QString("QWidget#TasksPopupWidget { border: 1px solid %1; }").arg(palette().color(QPalette::Normal, QPalette::Mid).name(QColor::HexRgb)));

    _minimumDurationTimer.setSingleShot(true);
    _minimumDurationTimer.setInterval(minimumDuration);

    setLayout(new QGridLayout());

    auto& tasksFilterModel = _tasksStatusBarAction.getTasksFilterModel();

    /*
    const auto numberOfTasksChangedDeferred = [this]() -> void {
        if (isHidden() && !_minimumDurationTimer.isActive())
            _minimumDurationTimer.start();

        if (isVisible())
            numberOfTasksChanged();
    };

    connect(&_minimumDurationTimer, &QTimer::timeout, this, &TasksPopupWidget::numberOfTasksChanged);
    */

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, [this](const QModelIndex& parent, int first, int last) -> void {
#ifdef TASKS_POPUP_WIDGET_VERBOSE
        for (int rowIndex = first; rowIndex < last; rowIndex++)
            qDebug() << "TasksPopupWidget: " << _tasksStatusBarAction.getTasksFilterModel().index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name)).data(Qt::DisplayRole).toString() << " inserted into TasksFilterModel";
#endif

        updateContents();
    });

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, [this](const QModelIndex& parent, int first, int last) -> void {
#ifdef TASKS_POPUP_WIDGET_VERBOSE
        for (int rowIndex = first; rowIndex < last; rowIndex++)
            qDebug() << "TasksPopupWidget: " << _tasksStatusBarAction.getTasksFilterModel().index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name)).data(Qt::DisplayRole).toString() << " removed from TasksFilterModel";
#endif

        updateContents();
    });

    synchronizeWithAnchorWidget();
    updateIcon();
    updateContents();

    if (_anchorWidget) {
        installEventFilter(this);
        _anchorWidget->installEventFilter(this);
        Application::getMainWindow()->installEventFilter(this);
    }
}

bool TasksPopupWidget::eventFilter(QObject* target, QEvent* event)
{
    auto targetWidget = qobject_cast<QWidget*>(target);

    if (targetWidget == nullptr)
        return QObject::eventFilter(target, event);

    switch (event->type())
    {
        case QEvent::Show:
        case QEvent::Move:
        {
            if (targetWidget == _anchorWidget || targetWidget == Application::getMainWindow())
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

    const auto offset = QPoint(width(), height());

    if (_anchorWidget->isVisible())
        move(_anchorWidget->mapToGlobal(QPoint(_anchorWidget->width(), 0)) - offset);
    else
        move(Application::getMainWindow()->geometry().bottomRight() - offset - QPoint(5, 5));
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

void TasksPopupWidget::updateContents()
{
#ifdef TASKS_POPUP_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    //if (isHidden())
    //    return;

    auto& tasksModel        = _tasksStatusBarAction.getTasksModel();
    auto& tasksFilterModel  = _tasksStatusBarAction.getTasksFilterModel();

    const auto numberOfTasks = tasksFilterModel.rowCount();

    if (numberOfTasks == 0 && isVisible())
        close();

    cleanLayout();

    QVector<Task*> currentTasks;

    auto gridLayout = static_cast<QGridLayout*>(layout());

    gridLayout->setColumnStretch(1, 1);

    for (int rowIndex = 0; rowIndex < numberOfTasks; ++rowIndex) {
        const auto nameSourceModelIndex     = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Name)));
        const auto progressSourceModelIndex = tasksFilterModel.mapToSource(tasksFilterModel.index(rowIndex, static_cast<int>(AbstractTasksModel::Column::Progress)));

        if (!nameSourceModelIndex.isValid() || !progressSourceModelIndex.isValid())
            continue;

        auto nameItem       = dynamic_cast<AbstractTasksModel::NameItem*>(tasksModel.itemFromIndex(nameSourceModelIndex));
        auto progressItem   = dynamic_cast<AbstractTasksModel::ProgressItem*>(tasksModel.itemFromIndex(progressSourceModelIndex));

        Q_ASSERT(nameItem != nullptr);
        Q_ASSERT(progressItem != nullptr);

        if (nameItem == nullptr || progressItem == nullptr)
            continue;

        currentTasks << progressItem->getTask();

        if (!_widgetsMap.contains(progressItem->getTask())) {
            auto labelWidget    = nameItem->getStringAction().createWidget(this, StringAction::Label);
            auto progressWidget = progressItem->getTaskAction().createWidget(this);
            
            progressWidget->setFixedHeight(18);

            _widgetsMap[progressItem->getTask()] = { labelWidget, progressWidget };
        }

        const auto rowCount = gridLayout->rowCount();

        gridLayout->addWidget(_widgetsMap[progressItem->getTask()][0], rowCount, 0);
        gridLayout->addWidget(_widgetsMap[progressItem->getTask()][1], rowCount, 1);
    }

    for (auto task : _widgetsMap.keys()) {
        if (currentTasks.contains(task))
            continue;

        for (auto widget : _widgetsMap[task])
            delete widget;

        _widgetsMap.remove(task);
    }

    adjustSize();

    updateIcon();

    if (_anchorWidget)
        _anchorWidget->setToolTip(QString("%1 %2 task(s)").arg(QString::number(numberOfTasks), Task::guiScopeNames[_tasksStatusBarAction.getTaskScope()].toLower()));
}

void TasksPopupWidget::setIcon(const QIcon& icon)
{
    _tasksIconPixmap = icon.pixmap(iconPixmapSize);
}

}
