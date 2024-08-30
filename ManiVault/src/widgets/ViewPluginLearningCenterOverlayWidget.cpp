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
    _iconLabel.setPixmap(getIcon().pixmap(QSize(16, 16)));
}

const plugin::ViewPlugin* ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::getViewPlugin() const
{
    return _viewPlugin;
}

plugin::ViewPlugin* ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::getViewPlugin()
{
    return const_cast<plugin::ViewPlugin*>(_viewPlugin);
}

OverlayWidget* ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::getOverlayWidget() const
{
    return const_cast<OverlayWidget*>(_overlayWidget);
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

void ViewPluginLearningCenterOverlayWidget::CloseToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    parentWidget()->hide();
}

QIcon ViewPluginLearningCenterOverlayWidget::CloseToolbarItemWidget::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("times");
}

bool ViewPluginLearningCenterOverlayWidget::CloseToolbarItemWidget::shouldDisplay() const
{
    return true;
}

void ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    getViewPlugin()->getViewShortcutMapAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::getIcon() const
{
    WidgetActionBadge badge(nullptr, static_cast<std::uint32_t>(getViewPlugin()->getShortcutMap().getShortcuts().size()));

    badge.setScale(0.6);
    badge.setEnabled(true);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    return createIconWithNumberBadgeOverlay(Application::getIconFont("FontAwesome").getIcon("keyboard"), badge);
}

bool ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::shouldDisplay() const
{
    return getViewPlugin()->getShortcutMap().hasShortcuts();
}

ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::VisitGithubRepoToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip(QString("Visit the Github repository website <b>%1</b>").arg())
}

void ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory());

    nonConstPluginFactory->getVisitRepositoryAction().trigger();
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

void ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory());

    nonConstPluginFactory->getTriggerHelpAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("file-prescription");
}

bool ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::shouldDisplay() const
{
    auto nonConstPluginFactory = const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory());

    return nonConstPluginFactory->hasHelp();
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

    _layout.setSpacing(8);

    _layout.addWidget(new CloseToolbarItemWidget(viewPlugin, overlayWidget, "Close this toolbar"));
    _layout.addStretch(1);
    _layout.addWidget(new ShortcutsToolbarItemWidget(viewPlugin, overlayWidget, "View shortcuts <b>F1</b>"));
    _layout.addWidget(new ShowDocumentationToolbarItemWidget(viewPlugin, overlayWidget, "View full documentation <b>F2</b>"));
    _layout.addWidget(new VisitGithubRepoToolbarItemWidget(viewPlugin, overlayWidget));
    _layout.addWidget(new VisitLearningCenterToolbarItemWidget(viewPlugin, overlayWidget, "Go to the main learning center"));

    setLayout(&_layout);

    setContentsMargins(8);

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

bool ViewPluginLearningCenterOverlayWidget::ToolbarWidget::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            show();
            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(watched, event);
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
