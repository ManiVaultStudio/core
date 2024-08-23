// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginLearningCenterOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
#endif

namespace mv::gui
{
    
ViewPluginLearningCenterOverlayWidget::ViewPluginLearningCenterOverlayWidget(QWidget* source, const plugin::ViewPlugin* viewPlugin, const Qt::Alignment& alignment /*= Qt::AlignBottom | Qt::AlignRight*/) :
    OverlayWidget(source),
    _viewPlugin(viewPlugin),
    _alignment(alignment),
    _widgetFader(this, this),
    _popupWidget(viewPlugin)
{
    ////setAttribute(Qt::WA_TransparentForMouseEvents);
    //_popupWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);

    _layout.setAlignment(_alignment);

    _layout.addWidget(&_popupWidget);

    setLayout(&_layout);

    setContentsMargins(4);

    //this->installEventFilter(this);
}

bool ViewPluginLearningCenterOverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        case QEvent::Leave:
        case QEvent::MouseMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
            //qDebug() << __FUNCTION__ << event->type();
            return true;

        default:
            break;
    }
    
    return OverlayWidget::eventFilter(target, event);
}

void ViewPluginLearningCenterOverlayWidget::setTargetWidget(QWidget* targetWidget)
{
    //getWidgetOverlayer().getTargetWidget()->removeEventFilter(this);
    getWidgetOverlayer().setTargetWidget(targetWidget);
    //getWidgetOverlayer().getTargetWidget()->installEventFilter(this);
}

void ViewPluginLearningCenterOverlayWidget::showEvent(QShowEvent* event)
{
    OverlayWidget::showEvent(event);

    _widgetFader.fadeIn();
}

void ViewPluginLearningCenterOverlayWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

ViewPluginLearningCenterOverlayWidget::PopupWidget::PopupWidget(const plugin::ViewPlugin* viewPlugin, QWidget* parent) :
    QWidget(parent),
    _viewPlugin(viewPlugin)
{
    setLayout(&_layout);

    setContentsMargins(6);
}

void ViewPluginLearningCenterOverlayWidget::PopupWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    QLayoutItem* layoutItem;

    while ((layoutItem = _layout.takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(_viewPlugin->getFactory());

    if (_viewPlugin->hasShortcuts())
        _layout.addWidget(const_cast<plugin::ViewPlugin*>(_viewPlugin)->getViewShortcutMapAction().createWidget(this, TriggerAction::WidgetFlag::Icon));

    if (nonConstPluginFactory->hasHelp())
        _layout.addWidget(nonConstPluginFactory->getTriggerHelpAction().createWidget(this, TriggerAction::WidgetFlag::Icon));

    if (nonConstPluginFactory->getReadmeMarkdownUrl().isValid())
        _layout.addWidget(nonConstPluginFactory->getTriggerReadmeAction().createWidget(this, TriggerAction::WidgetFlag::Icon));

    if (nonConstPluginFactory->getRespositoryUrl().isValid())
        _layout.addWidget(nonConstPluginFactory->getVisitRepositoryAction().createWidget(this, TriggerAction::WidgetFlag::Icon));
}

void ViewPluginLearningCenterOverlayWidget::PopupWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

}
