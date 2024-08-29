// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginLearningCenterOverlayWidget.h"

#include <QDebug>

#ifdef _DEBUG
    #define VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ViewPluginLearningCenterOverlayWidget::ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin, const Qt::Alignment& alignment /*= Qt::AlignBottom | Qt::AlignRight*/) :
    OverlayWidget(target),
    _viewPlugin(viewPlugin),
    _alignment(alignment),
    _toolbarWidget(viewPlugin, this)
{
    //setStyleSheet("background-color: green;");

    Q_ASSERT(target);

    if (!target)
        return;

    addMouseEventReceiverWidget(&_toolbarWidget);

    _layout.setAlignment(_alignment);

    _layout.addWidget(&_toolbarWidget);

    setLayout(&_layout);

    setContentsMargins(4);
    setMouseTracking(true);

    raise();
}

void ViewPluginLearningCenterOverlayWidget::setTargetWidget(QWidget* targetWidget)
{
    getWidgetOverlayer().setTargetWidget(targetWidget);
}

void ViewPluginLearningCenterOverlayWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

ViewPluginLearningCenterOverlayWidget::ToolbarItemWidget::ToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget, const QIcon& icon, const QString& tooltip) :
    QWidget(),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _widgetFader(nullptr, &_iconLabel, .0f)
{
    setObjectName("ToolbarItemWidget");
    setToolTip(tooltip);

    _iconLabel.setPixmap(icon.pixmap(QSize(16, 16)));

    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_iconLabel);

    setLayout(&_layout);

    const auto installEventFilterOnTargetWidget = [this](QWidget* previousTargetWidget, QWidget* currentTargetWidget) -> void {
        Q_ASSERT(currentTargetWidget);

        if (!currentTargetWidget)
            return;

        if (previousTargetWidget)
            previousTargetWidget->removeEventFilter(this);

        currentTargetWidget->installEventFilter(this);
    };

    installEventFilterOnTargetWidget(nullptr, _overlayWidget->getWidgetOverlayer().getTargetWidget());

    connect(&_overlayWidget->getWidgetOverlayer(), &WidgetOverlayer::targetWidgetChanged, this, installEventFilterOnTargetWidget);
}

void ViewPluginLearningCenterOverlayWidget::ToolbarItemWidget::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);

    _widgetFader.setOpacity(1.f, 150);
}

void ViewPluginLearningCenterOverlayWidget::ToolbarItemWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    _widgetFader.setOpacity(.25f, 250);
}

const plugin::ViewPlugin* ViewPluginLearningCenterOverlayWidget::ToolbarItemWidget::getViewPlugin() const
{
    return _viewPlugin;
}

bool ViewPluginLearningCenterOverlayWidget::ToolbarItemWidget::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            _widgetFader.setOpacity(.25f, 350);
            break;
        }

        case QEvent::Leave:
        {
            _widgetFader.setOpacity(0.f, 350);
            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(watched, event);
}

void ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    const_cast<plugin::ViewPlugin*>(getViewPlugin())->getViewShortcutMapAction().trigger();
}

ViewPluginLearningCenterOverlayWidget::ToolbarWidget::ToolbarWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    QWidget(overlayWidget),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget)//,
    //_widgetFader(this, this, 0.f)
{
    setObjectName("ToolbarWidget");
    setMouseTracking(true);
    setToolTip(QString("%1 learning center").arg(viewPlugin->getKind()));
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    //_iconLabel.setPixmap(Application::getIconFont("FontAwesome").getIcon("chalkboard-teacher").pixmap(QSize(16, 16)));

    _layout.addWidget(new ShortcutsToolbarItemWidget(viewPlugin, overlayWidget, Application::getIconFont("FontAwesome").getIcon("keyboard"), "View shortcuts"));
    //_layout.addWidget(new ToolbarItemWidget(viewPlugin, overlayWidget, Application::getIconFont("FontAwesome").getIcon("book")));
    //_layout.addWidget(new ToolbarItemWidget(viewPlugin, overlayWidget, Application::getIconFont("FontAwesome").getIcon("chalkboard-teacher")));

    setLayout(&_layout);

    setContentsMargins(8);

    //_overlayWidget->getWidgetOverlayer().getTargetWidget()->installEventFilter(this);

    //return;
    
}

QMenu* ViewPluginLearningCenterOverlayWidget::ToolbarWidget::getContextMenu(QWidget* parent /*= nullptr*/) const
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

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

}
