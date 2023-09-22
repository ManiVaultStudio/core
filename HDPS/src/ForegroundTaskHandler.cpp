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
    _popupWidget(&_statusBarButton)
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

ForegroundTaskHandler::PopupWidget::PopupWidget(StatusBarButton* statusBarButton, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _statusBarButton(statusBarButton),
    _tasksAction(this, "Foreground Tasks"),
    _tasksIconPixmap(Application::getIconFont("FontAwesome").getIcon("tasks").pixmap(iconPixmapSize)),
    _opacityEffect(this)
{
    Q_ASSERT(_statusBarButton != nullptr);

    if (!_statusBarButton)
        return;

    setObjectName("ForegroundTasksPopupWidget");
    setGraphicsEffect(&_opacityEffect);
    setWindowFlag(Qt::FramelessWindowHint);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setStyleSheet(QString("QWidget#ForegroundTasksPopupWidget { border: 1px solid %1; }").arg(palette().color(QPalette::Normal, QPalette::Mid).name(QColor::HexRgb)));

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(4, 4, 4, 4);

    auto tasksWidget = _tasksAction.createWidget(this);

    layout->addWidget(tasksWidget);

    auto hierarchyWidget = findChild<HierarchyWidget*>("HierarchyWidget");

    Q_ASSERT(hierarchyWidget != nullptr);

    if (hierarchyWidget != nullptr)
        hierarchyWidget->setHeaderHidden(true);

    auto treeView = findChild<QTreeView*>("TreeView");

    Q_ASSERT(treeView != nullptr);

    if (treeView != nullptr) {
        treeView->setStyleSheet("QTreeView { border: none; }");

        treeView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        treeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Name), true);
        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Status), true);
        treeView->setColumnHidden(static_cast<int>(TasksModel::Column::Type), true);

        treeView->viewport()->setBackgroundRole(QPalette::Window);
    }
    
    setLayout(layout);
    
    auto& tasksModel        = _tasksAction.getTasksModel();
    auto& tasksFilterModel  = _tasksAction.getTasksFilterModel();

    tasksFilterModel.getTaskStatusFilterAction().setSelectedOptions({ "Running", "Running Indeterminate", "Finished" });
    tasksFilterModel.getTaskScopeFilterAction().setSelectedOptions({ "Foreground" });

    const auto updateVisibility = [this, &tasksFilterModel]() -> void {
        const auto numberOfForegroundTasks = tasksFilterModel.rowCount();

        if (numberOfForegroundTasks == 0 && isVisible())
            close();

        if (numberOfForegroundTasks >= 1 && !isVisible() && !settings().getTasksSettingsAction().getHideForegroundTasksPopupAction().isChecked())
            show();

        updateStatusBarButtonIcon();

        _statusBarButton->setToolTip(QString("%1 foreground task(s)").arg(QString::number(numberOfForegroundTasks)));

        QCoreApplication::processEvents();
    };

    updateVisibility();

    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsInserted, this, updateVisibility);
    connect(&tasksFilterModel, &QSortFilterProxyModel::rowsRemoved, this, updateVisibility);
    connect(&tasksFilterModel, &QSortFilterProxyModel::dataChanged, this, updateVisibility);

    connect(&settings().getTasksSettingsAction().getHideForegroundTasksPopupAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
        if (toggled) {
            close();
        }
        else {
            if (_tasksAction.getTasksFilterModel().rowCount() >= 1 && isHidden())
                show();
        }
    });

    const auto updateOpacityEffect = [this]() -> void {
        _opacityEffect.setOpacity(_statusBarButton->getSeeThroughAction().isChecked() ? 0.5f : 1.f);
    };

    updateOpacityEffect();

    connect(&_statusBarButton->getSeeThroughAction(), &ToggleAction::toggled, this, updateOpacityEffect);

    synchronizeWithAnchor();

    installEventFilter(this);
    _statusBarButton->installEventFilter(this);
    getMainWindow()->installEventFilter(this);

    updateStatusBarButtonIcon();
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
                synchronizeWithAnchor();

            break;
        }

        case QEvent::Resize:
        {
            if (targetWidget == this)
                synchronizeWithAnchor();

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

void ForegroundTaskHandler::PopupWidget::synchronizeWithAnchor()
{
    move(_statusBarButton->mapToGlobal(QPoint(_statusBarButton->width(), 0)) - QPoint(width(), height()));
}

QSize ForegroundTaskHandler::PopupWidget::sizeHint() const
{
    return QSize(0, 0);
}

}
