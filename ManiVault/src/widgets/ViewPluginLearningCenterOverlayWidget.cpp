// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginLearningCenterOverlayWidget.h"

#include "util/Icon.h"

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

ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget, const QString& tooltip) :
    QWidget(),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _widgetFader(nullptr, &_iconLabel, .0f)
{
    setObjectName("ToolbarItemWidget");
    setToolTip(tooltip);

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

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    updateIcon();
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::enterEvent(QEnterEvent* event)
{
    QWidget::enterEvent(event);

    _widgetFader.setOpacity(0.7f, 150);
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    _widgetFader.setOpacity(.25f, 250);
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::updateIcon()
{
    _iconLabel.setPixmap(getIcon().pixmap(QSize(18, 18)));
}

const plugin::ViewPlugin* ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::getViewPlugin() const
{
    return _viewPlugin;
}

plugin::ViewPlugin* ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::getViewPlugin()
{
    return const_cast<plugin::ViewPlugin*>(_viewPlugin);
}

bool ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            setVisible(shouldDisplay());
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
    getViewPlugin()->getViewShortcutMapAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::getIcon() const
{
    WidgetActionBadge badge(nullptr, getViewPlugin()->getShortcutMap().getShortcuts().size());

    badge.setScale(0.5);
    badge.setEnabled(true);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    return createIconWithNumberBadgeOverlay(Application::getIconFont("FontAwesome").getIcon("keyboard"), badge);
}

bool ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::shouldDisplay() const
{
    return getViewPlugin()->getShortcutMap().hasShortcuts();
}

void ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    mv::help().getToRepositoryAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::getIcon() const
{
    return Application::getIconFont("FontAwesomeBrands").getIcon("github");
}

bool ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::shouldDisplay() const
{
    auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory());

    return nonConstPluginFactory->getRespositoryUrl().isValid();
}

void ViewPluginLearningCenterOverlayWidget::VisitLearningCenterToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    mv::help().getShowLearningCenterAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::VisitLearningCenterToolbarItemWidget::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("chalkboard-teacher");
}

bool ViewPluginLearningCenterOverlayWidget::VisitLearningCenterToolbarItemWidget::shouldDisplay() const
{
    return true;
}

void ViewPluginLearningCenterOverlayWidget::ShowReadmeToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory());

    nonConstPluginFactory->getTriggerReadmeAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ShowReadmeToolbarItemWidget::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("book");
}

bool ViewPluginLearningCenterOverlayWidget::ShowReadmeToolbarItemWidget::shouldDisplay() const
{
    auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory());

    return nonConstPluginFactory->getReadmeMarkdownUrl().isValid();
}

ViewPluginLearningCenterOverlayWidget::ToolbarWidget::ToolbarWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    QWidget(overlayWidget),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget)
{
    setObjectName("ToolbarWidget");
    setMouseTracking(true);
    setToolTip(QString("%1 learning center").arg(viewPlugin->getKind()));
    setAttribute(Qt::WA_TransparentForMouseEvents, false);

    _layout.setSpacing(10);

    _layout.addWidget(new ShortcutsToolbarItemWidget(viewPlugin, overlayWidget, "View shortcuts"));
    _layout.addWidget(new ShowReadmeToolbarItemWidget(viewPlugin, overlayWidget, "Show readme information from the Github repository"));
    _layout.addWidget(new VisitGithubRepoToolbarItemWidget(viewPlugin, overlayWidget, "Visit the Github repository website"));
    _layout.addWidget(new VisitLearningCenterToolbarItemWidget(viewPlugin, overlayWidget, "Go to the main learning center"));

    setLayout(&_layout);

    setContentsMargins(8);
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
