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
    
ViewPluginLearningCenterOverlayWidget::ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin, const Qt::Alignment& alignment /*= Qt::AlignBottom | Qt::AlignRight*/) :
    OverlayWidget(target),
    _viewPlugin(viewPlugin),
    _alignment(alignment),
    _popupWidget(viewPlugin),
    _widgetFader(this, this)
{
    Q_ASSERT(target);

    if (!target)
        return;

    getWidgetOverlayer().getTargetWidget()->installEventFilter(this);

    _layout.setAlignment(_alignment);

    _layout.addWidget(&_popupWidget);

    setLayout(&_layout);

    setContentsMargins(4);
    setMouseTracking(true);
    //setAutoFillBackground(true);

    //setStyleSheet("background-color: rgba(0, 0, 0, 20)");

    //_hoverOverlayWidget.setLayout(layout);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    //hoverOverlayWidget->raise();
}

bool ViewPluginLearningCenterOverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    //if (target != getWidgetOverlayer().getTargetWidget())
    //    return OverlayWidget::eventFilter(target, event);

    switch (event->type())
    {
        case QEvent::Enter:
        {
            if (target == getWidgetOverlayer().getTargetWidget())
                _widgetFader.fadeIn();

            break;
        }

        case QEvent::Leave:
        {
            if (target == getWidgetOverlayer().getTargetWidget())
                _widgetFader.fadeOut();

            break;
        }

        default:
            break;
    }
    
    return OverlayWidget::eventFilter(target, event);
}

void ViewPluginLearningCenterOverlayWidget::setTargetWidget(QWidget* targetWidget)
{
    getWidgetOverlayer().getTargetWidget()->removeEventFilter(this);
    getWidgetOverlayer().setTargetWidget(targetWidget);
    getWidgetOverlayer().getTargetWidget()->installEventFilter(this);
}

void ViewPluginLearningCenterOverlayWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

//ViewPluginLearningCenterOverlayWidget::LearningCenterIconLabel::LearningCenterIconLabel(QWidget* parent)
//{
//}
//
//void ViewPluginLearningCenterOverlayWidget::LearningCenterIconLabel::mousePressEvent(QMouseEvent* event)
//{
//    QLabel::mousePressEvent(event);
//}

ViewPluginLearningCenterOverlayWidget::PopupWidget::PopupWidget(const plugin::ViewPlugin* viewPlugin, QWidget* parent) :
    QWidget(parent),
    _viewPlugin(viewPlugin),
    _opacityEffect(this)
{
    setGraphicsEffect(&_opacityEffect);
    setMouseTracking(true);
    setToolTip(QString("%1 learning center").arg(viewPlugin->getKind()));

    _iconLabel.setPixmap(Application::getIconFont("FontAwesome").getIcon("chalkboard-teacher").pixmap(QSize(16, 16)));

    _layout.addWidget(&_iconLabel);

    setLayout(&_layout);

    setContentsMargins(8);

    //setFocusPolicy(Qt::StrongFocus);
    //setStyleSheet("background-color: lightblue;");
    //setFocusPolicy(Qt::StrongFocus); // Set focus policy
    _iconLabel.setAttribute(Qt::WA_TransparentForMouseEvents, false);
    //raise();
    //setFocus();
    //stackUnder(parent);

    _opacityEffect.setOpacity(.5f);
}

void ViewPluginLearningCenterOverlayWidget::PopupWidget::mousePressEvent(QMouseEvent* event)
{
    QWidget::mousePressEvent(event);

    QScopedPointer<QMenu> contextMenu(getContextMenu(this));

    contextMenu->exec(mapToGlobal(event->pos()));
}

void ViewPluginLearningCenterOverlayWidget::PopupWidget::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);

    //setStyleSheet("opacity: 1;");
    //_opacityEffect.setOpacity(1.f);
}

void ViewPluginLearningCenterOverlayWidget::PopupWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    //_opacityEffect.setOpacity(.5f);
    //setStyleSheet("opacity: 0.5;");
}

QMenu* ViewPluginLearningCenterOverlayWidget::PopupWidget::getContextMenu(QWidget* parent /*= nullptr*/) const
{
    auto contextMenu = new QMenu(parent);

    auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(_viewPlugin->getFactory());

    if (_viewPlugin->hasShortcuts())
        contextMenu->addAction(&const_cast<plugin::ViewPlugin*>(_viewPlugin)->getViewShortcutMapAction());

    if (nonConstPluginFactory->hasHelp())
        contextMenu->addAction(&nonConstPluginFactory->getTriggerHelpAction());

    if (nonConstPluginFactory->getReadmeMarkdownUrl().isValid())
        contextMenu->addAction(&nonConstPluginFactory->getTriggerReadmeAction());

    if (nonConstPluginFactory->getRespositoryUrl().isValid())
        contextMenu->addAction(&nonConstPluginFactory->getVisitRepositoryAction());

    auto other = new QMenu("Other");

    other->setIcon(Application::getIconFont("FontAwesome").getIcon("ellipsis-h"));

    contextMenu->addMenu(other);

    other->addAction(&mv::help().getShowLearningCenterAction());

    return contextMenu;
}

void ViewPluginLearningCenterOverlayWidget::PopupWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

}
