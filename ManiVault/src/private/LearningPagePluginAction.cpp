// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPagePluginAction.h"

#include <Application.h>
#include <PluginFactory.h>

#include <QDebug>
#include <QEnterEvent>
#include <QEvent>

using namespace mv;
using namespace mv::plugin;

LearningPagePluginActionsWidget::LearningPagePluginActionsWidget(const mv::plugin::PluginFactory* pluginFactory, QWidget* parent /*= nullptr*/) :
    QWidget(parent),
    _pluginFactory(pluginFactory),
    _mainLayout(),
    _actionsOverlayWidget(this)
{
    Q_ASSERT(_pluginFactory);

    if (!_pluginFactory)
        return;

    setObjectName("LearningPagePluginActionsWidget");
    setMouseTracking(true);

    auto categoryIconLabel  = new QLabel();
    auto nameLabel          = new QLabel(_pluginFactory->getKind());
    auto versionLabel       = new QLabel(QString("v%1").arg(_pluginFactory->getVersion()));

    categoryIconLabel->setPixmap(_pluginFactory->getCategoryIcon().pixmap(QSize(12, 12)));

    categoryIconLabel->setToolTip("Plugin category");
    nameLabel->setToolTip("Name of the plugin");
    versionLabel->setToolTip("Plugin version");

    //if (!hasOverlay()) {
    //    nameLabel->setStyleSheet("color: gray;");
    //    versionLabel->setStyleSheet("color: gray;");
    //}

    _mainLayout.setContentsMargins(6, 3, 6, 3);
    _mainLayout.addWidget(categoryIconLabel);
    _mainLayout.addWidget(nameLabel);
    _mainLayout.addWidget(versionLabel);

    if (hasOverlay()) {
        auto elipsisLabel = new QLabel();

        elipsisLabel->setPixmap(Application::getIconFont("FontAwesome").getIcon("ellipsis-h").pixmap(QSize(12, 12)));
        elipsisLabel->setToolTip("Plugin has additional resources");

        _mainLayout.addWidget(elipsisLabel);
    }

    setLayout(&_mainLayout);

    _actionsOverlayWidget.hide();

    updateStyle();
}

const PluginFactory* LearningPagePluginActionsWidget::getPluginFactory() const
{
    return _pluginFactory;
}

void LearningPagePluginActionsWidget::enterEvent(QEnterEvent* enterEvent)
{
    QWidget::enterEvent(enterEvent);

    if (!hasOverlay())
        return;

    _actionsOverlayWidget.show();
    _actionsOverlayWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);

    updateStyle();
}

void LearningPagePluginActionsWidget::leaveEvent(QEvent* leaveEvent)
{
    QWidget::leaveEvent(leaveEvent);

    if (!hasOverlay())
        return;

    _actionsOverlayWidget.hide();
    _actionsOverlayWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);

    updateStyle();
}

bool LearningPagePluginActionsWidget::event(QEvent* event)
{
    if (event->type() == QEvent::ApplicationPaletteChange)
        updateStyle();

    return QWidget::event(event);
}

bool LearningPagePluginActionsWidget::hasOverlay() const
{
    return _pluginFactory->getReadmeMarkdownUrl().isValid() || _pluginFactory->getRepositoryUrl().isValid();
}

void LearningPagePluginActionsWidget::updateStyle()
{
    setStyleSheet("QWidget#LearningPagePluginActionsWidget { \
        background-color: rgb(200, 200, 200); \
        border-radius: 5px; \
    }");
}

LearningPagePluginActionsWidget::ActionsOverlayWidget::ActionsOverlayWidget(LearningPagePluginActionsWidget* learningPagePluginActionWidget) :
    QWidget(learningPagePluginActionWidget),
    _learningPagePluginActionWidget(learningPagePluginActionWidget),
    _mainLayout(),
    _widgetOverlayer(this, this, learningPagePluginActionWidget)
{
    setObjectName("ActionsOverlayWidget");

    _mainLayout.setContentsMargins(4, 2, 4, 2);
    _mainLayout.addStretch(1);

    const auto hasReadmeMarkdownUrl = _learningPagePluginActionWidget->getPluginFactory()->getReadmeMarkdownUrl().isValid();
    const auto hasRespositoryUrl    = _learningPagePluginActionWidget->getPluginFactory()->getRepositoryUrl().isValid();

    if (hasReadmeMarkdownUrl) {
        auto triggerReadmeActionWidget = new ActionWidget("book", [this]() -> void {
            const_cast<PluginFactory*>(_learningPagePluginActionWidget->getPluginFactory())->getPluginMetadata().getTriggerReadmeAction().trigger();
        });

        triggerReadmeActionWidget->setToolTip("View the plugin readme content");

        _mainLayout.addWidget(triggerReadmeActionWidget);
    }

    if (hasRespositoryUrl) {
        auto visitRepositoryActionWidget = new ActionWidget("globe", [this]() -> void {
            const_cast<PluginFactory*>(_learningPagePluginActionWidget->getPluginFactory())->getPluginMetadata().getVisitRepositoryAction().trigger();
        });

        visitRepositoryActionWidget->setToolTip("Visit the plugin repository");

        _mainLayout.addWidget(visitRepositoryActionWidget);
    }

    if (hasReadmeMarkdownUrl || hasRespositoryUrl) {
        setStyleSheet("QWidget#ActionsOverlayWidget { \
            background: qlineargradient( x1:0 y1:0, x2:1 y2:0, stop: 0.2 rgba(150, 150, 150, 0), stop: 0.7 rgba(200, 200, 200, 200), stop: 1.0 rgba(200, 200, 200, 200)); \
            border-radius: 5px; \
        }");
    }
    else {
        setStyleSheet("QWidget#ActionsOverlayWidget { \
            background-color: rgba(200, 200, 200, 200); \
            border-radius: 5px; \
        }");
    }

    setLayout(&_mainLayout);
}

LearningPagePluginActionsWidget::ActionWidget::ActionWidget(const QString& iconName, ClickedFunction clickedFunction, QWidget* parent /*= nullptr*/) :
    QLabel(parent),
    _iconName(iconName),
    _clickedFunction(clickedFunction)
{
    setObjectName("ActionWidget");
    setMouseTracking(true);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    const auto fixedSize = QSize(12, 12);

    setFixedSize(fixedSize);

    updateStyle();
}

void LearningPagePluginActionsWidget::ActionWidget::mousePressEvent(QMouseEvent* event)
{
    if (!_clickedFunction)
        return;

    _clickedFunction();
}

void LearningPagePluginActionsWidget::ActionWidget::enterEvent(QEnterEvent* enterEvent)
{
    updateStyle();
}

void LearningPagePluginActionsWidget::ActionWidget::leaveEvent(QEvent* leaveEvent)
{
    updateStyle();
}

void LearningPagePluginActionsWidget::ActionWidget::updateStyle()
{
    setPixmap(mv::Application::getIconFont("FontAwesome").getIcon(_iconName, underMouse() ? QColor(40, 40, 40) : QColor(0, 0, 0)).pixmap(QSize(12, 12)));
}
