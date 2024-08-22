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

    this->installEventFilter(this);
}

bool ViewPluginLearningCenterOverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            _widgetFader.fadeIn();
            return true;
        }

        case QEvent::Leave:
        {
            _widgetFader.fadeOut();
            return true;
        }

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

void ViewPluginLearningCenterOverlayWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

ViewPluginLearningCenterOverlayWidget::PopupWidget::PopupWidget(const plugin::ViewPlugin* viewPlugin, QWidget* parent) :
    QWidget(parent),
    _viewPlugin(viewPlugin),
    _label("Test", this)
{
    _layout.addWidget(&_label);

    _label.installEventFilter(this);
    _label.setPixmap(Application::getIconFont("FontAwesome").getIcon("chalkboard-teacher").pixmap(QSize(12, 12)));

    setLayout(&_layout);

    setContentsMargins(6);

    //setStyleSheet("background-color: green;");
}

bool ViewPluginLearningCenterOverlayWidget::PopupWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::MouseButtonPress:
        {
            if (target != &_label)
                break;

            if (auto mouseEvent = dynamic_cast<QMouseEvent*>(event)) {
                QMenu learningCenterMenu;

                auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(_viewPlugin->getFactory());

                if (_viewPlugin->hasShortcuts())
                    learningCenterMenu.addAction(&const_cast<plugin::ViewPlugin*>(_viewPlugin)->getViewShortcutMapAction());

                if (nonConstPluginFactory->hasHelp())
                    learningCenterMenu.addAction(&nonConstPluginFactory->getTriggerHelpAction());

                if (nonConstPluginFactory->getReadmeMarkdownUrl().isValid())
                    learningCenterMenu.addAction(&nonConstPluginFactory->getTriggerReadmeAction());

                if (nonConstPluginFactory->getRespositoryUrl().isValid())
                    learningCenterMenu.addAction(&nonConstPluginFactory->getVisitRepositoryAction());

                learningCenterMenu.exec(mapToGlobal(mouseEvent->pos()));
                
            }

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void ViewPluginLearningCenterOverlayWidget::PopupWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

}
