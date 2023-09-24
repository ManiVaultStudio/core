// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ForegroundTaskHandler.h"
#include "Application.h"
#include "CoreInterface.h"

#include "util/Icon.h"

#include <QPainter>
#include <QMainWindow>

using namespace hdps::gui;

namespace hdps {

const QSize ForegroundTaskHandler::PopupWidget::iconPixmapSize = QSize(64, 64);

ForegroundTaskHandler::ForegroundTaskHandler(QObject* parent) :
    AbstractTaskHandler(parent, nullptr),
    _statusBarButton(),
    _popupWidget(this, &_statusBarButton)
{
}

void ForegroundTaskHandler::init()
{
}

ForegroundTaskHandler::StatusBarButton::StatusBarButton(QWidget* parent /*= nullptr*/) :
    QToolButton(parent),
    _menu(this),
    _seeThroughAction(this, "See-through")
{
    setAutoRaise(true);
    setStyleSheet("QToolButton::menu-indicator { image: none; }");
    setMenu(&_menu);
    setPopupMode(QToolButton::InstantPopup);

    _menu.addAction(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction());
    //_menu.addAction(&_seeThroughAction);
}

void ForegroundTaskHandler::StatusBarButton::paintEvent(QPaintEvent* paintEvent)
{
    QToolButton::paintEvent(paintEvent);

    QPainter painter(this);

    const auto margin = 5;
    const auto icon = this->icon();

    if (icon.isNull())
        return;

    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawPixmap(QPoint(margin, margin), icon.pixmap(icon.availableSizes().first()).scaled(size() - 2 * QSize(margin, margin), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

ForegroundTaskHandler::PopupWidget::PopupWidget(ForegroundTaskHandler* foregroundTaskHandler, StatusBarButton* statusBarButton, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _foregroundTaskHandler(foregroundTaskHandler),
    _statusBarButton(statusBarButton),
    _tasksAction(this, "Foreground Tasks"),
    _tasksIconPixmap(Application::getIconFont("FontAwesome").getIcon("tasks").pixmap(iconPixmapSize))
{
    Q_ASSERT(_foregroundTaskHandler != nullptr);

    if (!_foregroundTaskHandler)
        return;

    Q_ASSERT(_statusBarButton != nullptr);

    if (!_statusBarButton)
        return;

    setObjectName("ForegroundTasksPopupWidget");
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet(QString("QWidget#ForegroundTasksPopupWidget { border: 1px solid %1; }").arg(palette().color(QPalette::Normal, QPalette::Mid).name(QColor::HexRgb)));

    _minimumDurationTimer.setSingleShot(true);
    
    const auto updateMinimumDurationTimer = [this]() -> void {
        _minimumDurationTimer.setInterval(_foregroundTaskHandler->getMinimumDuration());
    };

    updateMinimumDurationTimer();

    connect(_foregroundTaskHandler, &AbstractTaskHandler::minimumDurationChanged, this, updateMinimumDurationTimer);

    setLayout(new QVBoxLayout());
    
    auto& tasksModel        = _tasksAction.getTasksModel();
    auto& tasksFilterModel  = _tasksAction.getTasksFilterModel();

    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Foreground" });

    const auto numberOfForegroundTasksChangedDeferred = [this]() -> void {
        if (isHidden() && !_minimumDurationTimer.isActive())
            _minimumDurationTimer.start();

        if (isVisible())
            numberOfForegroundTasksChanged();
    };

    connect(&_minimumDurationTimer, &QTimer::timeout, this, &PopupWidget::numberOfForegroundTasksChanged);

    connect(&tasksFilterModel, &QSortFilterProxyModel::layoutChanged, this, numberOfForegroundTasksChangedDeferred);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, numberOfForegroundTasksChangedDeferred);

    connect(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
        if (toggled) {
            close();
        }
        else {
            if (_tasksAction.getTasksFilterModel().rowCount() >= 1 && isHidden())
                show();
        }
    });

    synchronizeWithStatusBarButton();

    installEventFilter(this);
    _statusBarButton->installEventFilter(this);
    getMainWindow()->installEventFilter(this);

    updateStatusBarButtonIcon();
    numberOfForegroundTasksChanged();
}

bool ForegroundTaskHandler::PopupWidget::eventFilter(QObject* target, QEvent* event)
{
    auto targetWidget = qobject_cast<QWidget*>(target);

    if (targetWidget == nullptr)
        return QObject::eventFilter(target, event);

    switch (event->type())
    {
        case QEvent::Move:
        {
            if (targetWidget == _statusBarButton || targetWidget == getMainWindow())
                synchronizeWithStatusBarButton();

            break;
        }

        case QEvent::Resize:
        {
            if (targetWidget == this)
                synchronizeWithStatusBarButton();

            break;
        }

        default:
            break;
    }

    return QObject::eventFilter(target, event);
}

void ForegroundTaskHandler::PopupWidget::updateStatusBarButtonIcon()
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

    const auto textRectangleSize    = QSize(32, 32);
    const auto textRectangle        = QRectF(center - QPointF(textRectangleSize.width() / 2.f, textRectangleSize.height() / 2.f), textRectangleSize);

    painter.drawText(textRectangle, QString::number(numberOfTasks), QTextOption(Qt::AlignCenter));

    _statusBarButton->setIcon(createIcon(iconPixmap));
}

QMainWindow* ForegroundTaskHandler::PopupWidget::getMainWindow()
{
    foreach(QWidget * widget, qApp->topLevelWidgets())
        if (auto mainWindow = qobject_cast<QMainWindow*>(widget))
            return mainWindow;

    return nullptr;
}

void ForegroundTaskHandler::PopupWidget::synchronizeWithStatusBarButton()
{
    move(_statusBarButton->mapToGlobal(QPoint(_statusBarButton->width(), 0)) - QPoint(width(), height()));
}

QSize ForegroundTaskHandler::PopupWidget::sizeHint() const
{
    return QSize(500, 0);
}

void ForegroundTaskHandler::PopupWidget::cleanLayout()
{
    QLayoutItem* item;

    while ((item = this->layout()->takeAt(0)) != 0)
        delete item;
}

void ForegroundTaskHandler::PopupWidget::numberOfForegroundTasksChanged()
{
    auto& tasksModel        = _tasksAction.getTasksModel();
    auto& tasksFilterModel  = _tasksAction.getTasksFilterModel();

    const auto numberOfForegroundTasks = tasksFilterModel.rowCount();

    if (numberOfForegroundTasks == 0 && isVisible())
        close();

    if (numberOfForegroundTasks >= 1 && !isVisible() && !settings().getTasksSettingsAction().getHideForegroundTasksPopupAction().isChecked())
        show();

    cleanLayout();

    QVector<Task*> currentTasks;

    for (int rowIndex = 0; rowIndex < numberOfForegroundTasks; ++rowIndex) {
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

    updateStatusBarButtonIcon();

    _statusBarButton->setToolTip(QString("%1 foreground task(s)").arg(QString::number(numberOfForegroundTasks)));

    //QCoreApplication::processEvents();
}

}
