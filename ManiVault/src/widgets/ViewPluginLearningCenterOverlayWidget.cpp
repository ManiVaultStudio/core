// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginLearningCenterOverlayWidget.h"

#include "Application.h"
#include "CoreInterface.h"
#include "util/Icon.h"

#include <QDebug>
#include <QMainWindow>
#include <QMenu>
#include <QDesktopServices>

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
    _toolbarsLayout(nullptr),
    _settingsToolbarWidget(viewPlugin, this, alignment),
    _actionsToolbarWidget(viewPlugin, this, alignment)
{
    Q_ASSERT(target);

    if (!target)
        return;

    addMouseEventReceiverWidget(&_actionsToolbarWidget);

    _layout.setAlignment(_alignment);

    if (alignment & Qt::AlignLeft || alignment & Qt::AlignRight)
        _toolbarsLayout = new QVBoxLayout();

    if (alignment & Qt::AlignTop || alignment & Qt::AlignVCenter || alignment & Qt::AlignBottom)
        _toolbarsLayout = new QHBoxLayout();

    _settingsToolbarWidget.addWidget(new VisibleToolbarItemWidget(viewPlugin, this));

    _actionsToolbarWidget.addWidget(new VideosToolbarItemWidget(viewPlugin, this));
    _actionsToolbarWidget.addWidget(new DescriptionToolbarItemWidget(viewPlugin, this));
    _actionsToolbarWidget.addWidget(new ShortcutsToolbarItemWidget(viewPlugin, this));
    _actionsToolbarWidget.addWidget(new ShowDocumentationToolbarItemWidget(viewPlugin, this));
    _actionsToolbarWidget.addWidget(new VisitGithubRepoToolbarItemWidget(viewPlugin, this));
    _actionsToolbarWidget.addWidget(new ToLearningCenterToolbarItemWidget(viewPlugin, this));

    _toolbarsLayout->addWidget(&_settingsToolbarWidget);
    _toolbarsLayout->addStretch(1);
    _toolbarsLayout->addWidget(&_actionsToolbarWidget);

    _layout.addLayout(_toolbarsLayout);

    setLayout(&_layout);

    setContentsMargins(4);
    setMouseTracking(true);

    raise();
}

void ViewPluginLearningCenterOverlayWidget::setTargetWidget(QWidget* targetWidget)
{
    getWidgetOverlayer().setTargetWidget(targetWidget);

    setParent(targetWidget);
}

void ViewPluginLearningCenterOverlayWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget, const QSize& iconSize /*= QSize(16, 16)*/) :
    QWidget(),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _iconSize(iconSize),
    _widgetFader(nullptr, this, .0f),
    _hasVisibilityToggle(false)
{
    setObjectName("ToolbarItemWidget");

    //_overlayWidget->addMouseEventReceiverWidget(this);

    _iconLabel.setAlignment(Qt::AlignCenter);

    constexpr auto margin = 3;

    _layout.setContentsMargins(margin, margin, margin, margin);
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

    updateVisibility();

    if (getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction().isChecked())
        _widgetFader.setOpacity(0.8f, 100);
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    if (getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction().isChecked())
        _widgetFader.setOpacity(.35f, 350);
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::updateIcon()
{
    _iconLabel.setFixedSize(QSize(16, 16));
    _iconLabel.setPixmap(getIcon().pixmap(_iconSize));
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::installVisibilityToggle()
{
    if (_hasVisibilityToggle)
        return;

    updateVisibility();

    connect(&getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction(), &ToggleAction::toggled, this, &AbstractToolbarItemWidget::updateVisibility);

    _hasVisibilityToggle = true;
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::updateVisibility()
{
    if (!_hasVisibilityToggle)
        return;

    if (getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction().isChecked())
        _widgetFader.setOpacity(.35f, 350);
    else
        _widgetFader.setOpacity(.0f, 350);
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

            if (!_hasVisibilityToggle || (_hasVisibilityToggle && getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction().isChecked()))
                _widgetFader.setOpacity(.35f, 350);

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

ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::VisibleToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget, QSize(12, 12))
{
    const auto updateTooltip = [this]() -> void {
        setToolTip(QString("%1 the plugin learning center").arg(getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction().isChecked() ? "Hide" : "Show"));
    };

    updateTooltip();

    connect(&getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction(), &ToggleAction::toggled, this, [this, updateTooltip]() -> void
    {
        updateTooltip();
        updateIcon();
    });
}

void ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction().toggle();
}

QIcon ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::getIcon() const
{
    return getViewPlugin()->getLearningCenterAction().getViewPluginOverlayVisibleAction().icon();
}

bool ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::shouldDisplay() const
{
    return true;
}

ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::VideosToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip("Watch related videos");

    installVisibilityToggle();
}

void ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    auto contextMenu = new QMenu(this);

    for (const auto& video : getViewPlugin()->getLearningCenterAction().getVideos())
    {
        auto watchVideoAction = new QAction(video._title);

        watchVideoAction->setIcon(Application::getIconFont("FontAwesomeBrands").getIcon(video._youTubeId.isEmpty() ? "video" : "youtube"));

        connect(watchVideoAction, &QAction::triggered, this, [video]() -> void {
#ifdef USE_YOUTUBE_DIALOG
            YouTubeVideoDialog::play(_index.sibling(_index.row(), static_cast<int>(HelpManagerVideosModel::Column::YouTubeId)).data().toString());
#else
            QDesktopServices::openUrl(video._url);
#endif
        });

        contextMenu->addAction(watchVideoAction);
    }
        
    contextMenu->exec(mapToGlobal(event->pos()));
}

QIcon ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::getIcon() const
{
    WidgetActionBadge badge(nullptr, static_cast<std::uint32_t>(getViewPlugin()->getLearningCenterAction().getVideos().size()));

    badge.setScale(.6f);
    badge.setEnabled(true);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    return createIconWithNumberBadgeOverlay(Application::getIconFont("FontAwesome").getIcon("video"), badge);
}

bool ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::shouldDisplay() const
{
    return !getViewPlugin()->getLearningCenterAction().getVideos().empty();
}

ViewPluginLearningCenterOverlayWidget::DescriptionToolbarItemWidget::DescriptionToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip(getViewPlugin()->getLearningCenterAction().getViewDescriptionAction().toolTip());

    installVisibilityToggle();
}

void ViewPluginLearningCenterOverlayWidget::DescriptionToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    getViewPlugin()->getLearningCenterAction().getViewDescriptionAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::DescriptionToolbarItemWidget::getIcon() const
{
    return getViewPlugin()->getLearningCenterAction().getViewDescriptionAction().icon();
}

bool ViewPluginLearningCenterOverlayWidget::DescriptionToolbarItemWidget::shouldDisplay() const
{
    return getViewPlugin()->getLearningCenterAction().hasDescription();
}

ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::ShowDocumentationToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip(const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getTriggerHelpAction().toolTip());

    installVisibilityToggle();
}

void ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getTriggerHelpAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("file-prescription");
}

bool ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::shouldDisplay() const
{
    return const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->hasHelp();
}

ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::ShortcutsToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip("View shortcuts");

    installVisibilityToggle();
}

void ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    getViewPlugin()->getLearningCenterAction().getViewShortcutsAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::getIcon() const
{
    WidgetActionBadge badge(nullptr, static_cast<std::uint32_t>(getViewPlugin()->getShortcuts().getMap().getShortcuts().size()));

    badge.setScale(.6f);
    badge.setEnabled(true);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    return createIconWithNumberBadgeOverlay(Application::getIconFont("FontAwesome").getIcon("keyboard"), badge);
}

bool ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::shouldDisplay() const
{
    return getViewPlugin()->getShortcuts().hasShortcuts();
}

ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::VisitGithubRepoToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip(const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getVisitRepositoryAction().toolTip());

    installVisibilityToggle();
}

void ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getVisitRepositoryAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::getIcon() const
{
    return const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getVisitRepositoryAction().icon();
}

bool ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::shouldDisplay() const
{
    return const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getRepositoryUrl().isValid();
}

ViewPluginLearningCenterOverlayWidget::ToLearningCenterToolbarItemWidget::ToLearningCenterToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip(mv::help().getToLearningCenterAction().toolTip());

    installVisibilityToggle();
}

void ViewPluginLearningCenterOverlayWidget::ToLearningCenterToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    mv::help().getToLearningCenterAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ToLearningCenterToolbarItemWidget::getIcon() const
{
    return mv::help().getToLearningCenterAction().icon();
}

bool ViewPluginLearningCenterOverlayWidget::ToLearningCenterToolbarItemWidget::shouldDisplay() const
{
    return true;
}

ViewPluginLearningCenterOverlayWidget::ToolbarWidget::ToolbarWidget(const plugin::ViewPlugin* viewPlugin, OverlayWidget* overlayWidget, const Qt::Alignment& alignment) :
    QWidget(overlayWidget),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _layout(nullptr)
{
    setObjectName("ToolbarWidget");
    setMouseTracking(true);
    setToolTip(QString("%1 learning center").arg(viewPlugin->getKind()));

    try {
        Q_ASSERT(_viewPlugin && _overlayWidget);

        if (!_viewPlugin)
            throw std::runtime_error("View plugin is a nullptr");

        if (!_overlayWidget)
            throw std::runtime_error("Overlay widget is a nullptr");

        static const std::vector<Qt::Alignment> supportedAlignments{
            Qt::AlignTop,
            Qt::AlignBottom,
            Qt::AlignLeft,
            Qt::AlignRight
        };

        if (std::find(supportedAlignments.begin(), supportedAlignments.end(), alignment) == supportedAlignments.end())
            throw std::runtime_error("Supported toolbar alignment options are: Qt::AlignTop, Qt::AlignBottom, Qt::AlignLeft and Qt::AlignRight");

        if (alignment & Qt::AlignLeft || alignment & Qt::AlignRight)
            _layout = new QVBoxLayout(this);

        if (alignment & Qt::AlignTop || alignment & Qt::AlignVCenter || alignment & Qt::AlignBottom)
            _layout = new QHBoxLayout(this);

        _layout->setSpacing(0);

        setLayout(_layout);

        const auto setContentsMargins = [this](std::int32_t margin) -> void {
            _layout->setContentsMargins(margin, margin, margin, margin);
        };

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

        const auto updateTransparentForMouseEvents = [this]() -> void {
            setAttribute(Qt::WA_TransparentForMouseEvents, !_viewPlugin->getLearningCenterAction().getViewPluginOverlayVisibleAction().isChecked());
        };

        updateTransparentForMouseEvents();

        connect(&_viewPlugin->getLearningCenterAction().getViewPluginOverlayVisibleAction(), &ToggleAction::toggled, this, updateTransparentForMouseEvents);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget");
    }
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::addWidget(QWidget* widget)
{
    Q_ASSERT(_layout && widget);

    try {
        if (!_layout)
            throw std::runtime_error("No layout present");

        if (!widget)
            throw std::runtime_error("Supplied wWidget is a nullptr");

        _layout->addWidget(widget);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add widget to ViewPluginLearningCenterOverlayWidget::ToolbarWidget", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add widget to ViewPluginLearningCenterOverlayWidget::ToolbarWidget");
    }
}

bool ViewPluginLearningCenterOverlayWidget::ToolbarWidget::eventFilter(QObject* target, QEvent* event)
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

    return QWidget::eventFilter(target, event);
}

}
