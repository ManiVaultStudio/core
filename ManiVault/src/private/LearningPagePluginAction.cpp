// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LearningPagePluginAction.h"

#include <Application.h>

#include <QDebug>
#include <QEvent>

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

    auto categoryIconLabel = new QLabel();

    categoryIconLabel->setPixmap(_pluginFactory->getCategoryIcon().pixmap(QSize(16, 16)));

    auto label = new QLabel(_pluginFactory->getKind());

    auto versionLabel = new QLabel(QString("v%1").arg(_pluginFactory->getVersion()));

    _mainLayout.addWidget(categoryIconLabel);
    _mainLayout.addWidget(label);
    _mainLayout.addWidget(versionLabel);

    if (_pluginFactory->getReadmeMarkdownUrl().isValid() || _pluginFactory->getRespositoryUrl().isValid())
        _mainLayout.addWidget(new QLabel("..."));

    setLayout(&_mainLayout);

    _actionsOverlayWidget.hide();

    connect(qApp, &QApplication::paletteChanged, this, &LearningPagePluginActionsWidget::updateStyle);

    updateStyle();
}

const PluginFactory* LearningPagePluginActionsWidget::getPluginFactory() const
{
    return _pluginFactory;
}

void LearningPagePluginActionsWidget::enterEvent(QEnterEvent* enterEvent)
{
    QWidget::enterEvent(enterEvent);

    _actionsOverlayWidget.show();
    _actionsOverlayWidget.raise();
    _actionsOverlayWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);

    updateStyle();
}

void LearningPagePluginActionsWidget::leaveEvent(QEvent* leaveEvent)
{
    QWidget::leaveEvent(leaveEvent);

    _actionsOverlayWidget.hide();
    _actionsOverlayWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);

    updateStyle();
}

void LearningPagePluginActionsWidget::updateStyle()
{
    setStyleSheet("QWidget#LearningPagePluginActionsWidget { \
        background-color: rgb(150, 150, 150); \
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
    

    _mainLayout.addStretch(1);

    const auto hasReadmeMarkdownUrl = _learningPagePluginActionWidget->getPluginFactory()->getReadmeMarkdownUrl().isValid();
    const auto hasRespositoryUrl    = _learningPagePluginActionWidget->getPluginFactory()->getRespositoryUrl().isValid();

    if (hasReadmeMarkdownUrl) {
        _mainLayout.addWidget(new ActionWidget("book", [this]() -> void {
            const_cast<PluginFactory*>(_learningPagePluginActionWidget->getPluginFactory())->getTriggerReadmeAction().trigger();
        }));
    }

    if (hasRespositoryUrl) {
        _mainLayout.addWidget(new ActionWidget("globe", [this]() -> void {
            const_cast<PluginFactory*>(_learningPagePluginActionWidget->getPluginFactory())->getVisitRepositoryAction().trigger();
        }));
    }

    if (hasReadmeMarkdownUrl || hasRespositoryUrl) {
        setStyleSheet("QWidget#ActionsOverlayWidget { \
            background: qlineargradient( x1:0 y1:0, x2:1 y2:0, stop: 0.0 rgba(150, 150, 150, 0), stop: 0.5 rgb(150, 150, 150), stop: 1.0 rgb(150, 150, 150)); \
            border-radius: 5px; \
        }");
    }
    else {
        setStyleSheet("QWidget#ActionsOverlayWidget { \
            background-color: rgba(150, 150, 150, 150); \
            border-radius: 5px; \
        }");
    }

    setLayout(&_mainLayout);
}

LearningPagePluginActionsWidget::ActionWidget::ActionWidget(const QString& iconName, ClickedFunction clickedFunction, QWidget* parent /*= nullptr*/) :
    QLabel(parent),
    _clickedFunction(clickedFunction)
{
    setPixmap(mv::Application::getIconFont("FontAwesome").getIcon(iconName).pixmap(QSize(16, 16)));
}

void LearningPagePluginActionsWidget::ActionWidget::mousePressEvent(QMouseEvent* event)
{
    if (!_clickedFunction)
        return;

    _clickedFunction();
}
