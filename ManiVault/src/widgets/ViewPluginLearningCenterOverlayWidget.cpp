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

ViewPluginLearningCenterOverlayWidget::ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin) :
    OverlayWidget(target),
    _viewPlugin(viewPlugin),
    _settingsToolbarWidget(viewPlugin, this, true),
    _actionsToolbarWidget(viewPlugin, this)
{
    try {
        Q_ASSERT(target);

        if (!target)
            throw std::runtime_error("Target widget may not be a nullptr");

        addMouseEventReceiverWidget(&_toolbarsWidget);
        addMouseEventReceiverWidget(&_settingsToolbarWidget);

        if (std::find(PluginLearningCenterAction::alignmentFlags.begin(), PluginLearningCenterAction::alignmentFlags.end(), getLearningCenterAction().getAlignment()) == PluginLearningCenterAction::alignmentFlags.end())
            throw std::runtime_error("Supported toolbar alignment options are: Qt::AlignTop, Qt::AlignBottom, Qt::AlignLeft and Qt::AlignRight");

        setLayout(&_layout);

        setLayoutContentsMargins(&_layout, 0);

        _layout.setSpacing(0);

        _layout.addWidget(&_toolbarsWidget);

        updateAlignment();

        _settingsToolbarWidget.layout()->setSpacing(0);

        _settingsToolbarWidget.addWidget(new VisibleToolbarItemWidget(viewPlugin, this));
        _settingsToolbarWidget.addWidget(new AlignmentToolbarItemWidget(viewPlugin, this));

        _actionsToolbarWidget.addWidget(new VideosToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new DescriptionToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new ShortcutsToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new ShowDocumentationToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new VisitGithubRepoToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new ToLearningCenterToolbarItemWidget(viewPlugin, this));

        setMouseTracking(true);

        raise();

        target->installEventFilter(this);

        connect(&getLearningCenterAction().getAlignmentAction(), &OptionAction::currentIndexChanged, this, &ViewPluginLearningCenterOverlayWidget::updateAlignment);

        /*connect(&getLearningCenterAction().getOverlayVisibleAction(), &ToggleAction::toggled, this, [this](bool toggled) -> void {
            _actionsToolbarWidget.setAttribute(Qt::WA_TransparentForMouseEvents, !toggled);
        });*/
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget");
    }
}

void ViewPluginLearningCenterOverlayWidget::setTargetWidget(QWidget* targetWidget)
{
    getWidgetOverlayer().setTargetWidget(targetWidget);

    setParent(targetWidget);
}

void ViewPluginLearningCenterOverlayWidget::updateAlignment()
{
    const auto alignment = getLearningCenterAction().getAlignment();

    _layout.setAlignment(getLearningCenterAction().getAlignment());

    QBoxLayout* toolbarsLayout = nullptr;

    if (alignment & Qt::AlignLeft || alignment & Qt::AlignRight)
        toolbarsLayout = new QVBoxLayout();

    if (alignment & Qt::AlignTop || alignment & Qt::AlignCenter || alignment & Qt::AlignBottom)
        toolbarsLayout = new QHBoxLayout();

    if (!toolbarsLayout)
        return;

    toolbarsLayout->setContentsMargins(0, 0, 0, 0);

    _settingsToolbarWidget.updateAlignment();
    _actionsToolbarWidget.updateAlignment();

    replaceLayout(&_toolbarsWidget, toolbarsLayout);

    toolbarsLayout->addWidget(&_settingsToolbarWidget);
    toolbarsLayout->addStretch(1);
    toolbarsLayout->addWidget(&_actionsToolbarWidget);

    switch (alignment) {
	    case Qt::AlignLeft:
	    case Qt::AlignRight:
	    {
	        _toolbarsWidget.setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
	        break;
	    }

	    case Qt::AlignTop:
	    case Qt::AlignBottom:
	    {
	        _toolbarsWidget.setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
	        break;
	    }

        default:
            break;
    }

    update();
    updateGeometry();
    updateMask();
}

PluginLearningCenterAction& ViewPluginLearningCenterOverlayWidget::getLearningCenterAction()
{
    return const_cast<plugin::ViewPlugin*>(_viewPlugin)->getLearningCenterAction();
}

ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, const QSize& iconSize /*= QSize(14, 14)*/) :
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _iconSize(iconSize),
    _widgetFader(nullptr, this, .0f),
    _hasVisibilityToggle(false)
{
    setObjectName("ToolbarItemWidget");

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

    if (getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction().isChecked())
        _widgetFader.setOpacity(0.8f, 100);
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::leaveEvent(QEvent* event)
{
    QWidget::leaveEvent(event);

    if (getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction().isChecked())
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

    connect(&getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction(), &ToggleAction::toggled, this, &AbstractToolbarItemWidget::updateVisibility);

    _hasVisibilityToggle = true;
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::updateVisibility()
{
    if (!_hasVisibilityToggle)
        return;

    if (getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction().isChecked())
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

ViewPluginLearningCenterOverlayWidget* ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::getOverlayWidget() const
{
    return _overlayWidget;
}

bool ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::eventFilter(QObject* watched, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            setVisible(shouldDisplay());

            if (!_hasVisibilityToggle || (_hasVisibilityToggle && getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction().isChecked()))
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

ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::VisibleToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget, QSize(11, 11))
{
    const auto updateTooltip = [this]() -> void {
        setToolTip(QString("%1 the plugin learning center").arg(getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction().isChecked() ? "Hide" : "Show"));
    };

    updateTooltip();

    connect(&getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction(), &ToggleAction::toggled, this, [this, updateTooltip]() -> void
    {
        updateTooltip();
        updateIcon();
    });
}

void ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction().toggle();
}

QIcon ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::getIcon() const
{
    return getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction().icon();
}

bool ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::shouldDisplay() const
{
    return true;
}

std::vector<ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::Alignment> ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::alignments = {};

ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::AlignmentToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget, QSize(11, 11))
{
    setToolTip("Move the toolbar");

    if (alignments.empty()) {
        alignments = {
		    { Qt::AlignTop, "Move to top", getDockAreaIcon(DockAreaFlag::Top) },
		    { Qt::AlignBottom, "Move to bottom", getDockAreaIcon(DockAreaFlag::Bottom) },
		    { Qt::AlignLeft, "Move to left", getDockAreaIcon(DockAreaFlag::Left) },
		    { Qt::AlignCenter, "Move to center", getDockAreaIcon(DockAreaFlag::Center) },
		    { Qt::AlignRight, "Move to right", getDockAreaIcon(DockAreaFlag::Right) }
        };
    }
}

void ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    auto contextMenu = new QMenu(this);

    std::vector<Alignment> candidateAlignments;

    std::copy_if(alignments.begin(), alignments.end(), std::back_inserter(candidateAlignments), [this](const auto& alignment) {
        return alignment._alignment != getViewPlugin()->getLearningCenterAction().getAlignment();
    });

    for (const auto& candidateAlignment : candidateAlignments) {
        auto alignAction = new TriggerAction(contextMenu, candidateAlignment._title);

        alignAction->setIcon(candidateAlignment._icon);

        connect(alignAction, &TriggerAction::triggered, this, [this, candidateAlignment]() -> void {
            getViewPlugin()->getLearningCenterAction().setAlignment(candidateAlignment._alignment);
        });

        contextMenu->addAction(alignAction);
    }

    contextMenu->exec(mapToGlobal(event->pos()));
}

QIcon ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("arrows-alt");
}

bool ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::shouldDisplay() const
{
    return true;
}

ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::VideosToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
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

ViewPluginLearningCenterOverlayWidget::DescriptionToolbarItemWidget::DescriptionToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
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

ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::ShowDocumentationToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
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

ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::ShortcutsToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
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

ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::VisitGithubRepoToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
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

ViewPluginLearningCenterOverlayWidget::ToLearningCenterToolbarItemWidget::ToLearningCenterToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
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

ViewPluginLearningCenterOverlayWidget::ToolbarWidget::BackgroundWidget::BackgroundWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin) :
    OverlayWidget(target),
    _viewPlugin(viewPlugin)
{
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::BackgroundWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::RenderHint::Antialiasing);

    QStyleOption styleOption;

    styleOption.initFrom(this);

    const auto margin               = 5;
    const auto backgroundColor      = styleOption.palette.color(QPalette::Inactive, QPalette::Window);

	auto backgroundEdgeColor  = backgroundColor;

    backgroundEdgeColor.setAlphaF(0.5f);

    switch (const_cast<plugin::ViewPlugin*>(_viewPlugin)->getLearningCenterAction().getAlignment()) {
	    case Qt::AlignLeft:
	    case Qt::AlignRight:
	    {
            const auto size     = rect().width();
            const auto radius   = size / 2;

            painter.setPen(QPen(backgroundColor, size - 2 * margin, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap));
            painter.drawLine(radius, rect().top() + radius - margin, radius, rect().bottom() - radius + margin);

            painter.setPen(QPen(backgroundEdgeColor, 2 + (size - 2 * margin), Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap));
            painter.drawLine(radius, rect().top() + radius - margin, radius, rect().bottom() - radius + margin);
	        break;
	    }

	    case Qt::AlignTop:
	    case Qt::AlignBottom:
	    {
            const auto size     = rect().height();
            const auto radius   = size / 2;

            painter.setPen(QPen(backgroundColor, size - 2 * margin, Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap));
            painter.drawLine(radius - margin, radius, rect().right() - radius + margin, radius);

            painter.setPen(QPen(backgroundEdgeColor, 2 + (size - 2 * margin), Qt::PenStyle::SolidLine, Qt::PenCapStyle::RoundCap));
            painter.drawLine(radius, rect().top() + radius - margin, radius, rect().bottom() - radius + margin);

	        break;
	    }
    }

    QWidget::paintEvent(event);
}

ViewPluginLearningCenterOverlayWidget::ToolbarWidget::ToolbarWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, bool alwaysVisible /*= false*/) :
    QWidget(overlayWidget),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _alwaysVisible(alwaysVisible),
    _backgroundWidget(this, viewPlugin),
    _backgroundWidgetFader(this, &_backgroundWidget)
{
    setObjectName("ToolbarWidget");
    setMouseTracking(true);
    setToolTip(QString("%1 learning center").arg(viewPlugin->getKind()));

    _layout.setSpacing(0);

    _layout.addLayout(&_verticalLayout);
    _layout.addLayout(&_horizontalLayout);

    setLayoutContentsMargins(&_layout, 0);

    setLayout(&_layout);

    try {
        Q_ASSERT(_viewPlugin && _overlayWidget);

        if (!_viewPlugin)
            throw std::runtime_error("View plugin is a nullptr");

        if (!_overlayWidget)
            throw std::runtime_error("Overlay widget is a nullptr");

        updateAlignment();

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

        visibilityChanged();

        connect(&_viewPlugin->getLearningCenterAction().getOverlayVisibleAction(), &ToggleAction::toggled, this, &ToolbarWidget::visibilityChanged);

        _backgroundWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget");
    }
}

PluginLearningCenterAction& ViewPluginLearningCenterOverlayWidget::ToolbarWidget::getLearningCenterAction()
{
    return const_cast<plugin::ViewPlugin*>(_viewPlugin)->getLearningCenterAction();
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::addWidget(QWidget* widget)
{
    try {
        if (!layout())
            throw std::runtime_error("No layout present");

        if (!widget)
            throw std::runtime_error("Supplied wWidget is a nullptr");

        _widgets.push_back(widget);

        switch (getLearningCenterAction().getAlignment()) {
	        case Qt::AlignLeft:
	        case Qt::AlignRight:
	        {
	            _verticalLayout.addWidget(widget);
	            break;
	        }

	        case Qt::AlignTop:
	        case Qt::AlignBottom:
	        {
				_horizontalLayout.addWidget(widget);
	            break;
	        }
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add widget to ViewPluginLearningCenterOverlayWidget::ToolbarWidget", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to add widget to ViewPluginLearningCenterOverlayWidget::ToolbarWidget");
    }
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);

    _backgroundWidget.lower();
}

bool ViewPluginLearningCenterOverlayWidget::ToolbarWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Enter:
        {
            show();

            _backgroundWidgetFader.fadeIn();
            break;
        }

        case QEvent::Leave:
        {
            show();

            _backgroundWidgetFader.fadeOut();
            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::visibilityChanged()
{
    if (_alwaysVisible)
        return;

    if (_viewPlugin->getLearningCenterAction().getOverlayVisibleAction().isChecked()) {
        _overlayWidget->addMouseEventReceiverWidget(this);
        update();
    }
    else {
        _overlayWidget->removeMouseEventReceiverWidget(this);
    }
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::updateAlignment()
{
    const auto alignment = getLearningCenterAction().getAlignment();

    if (std::find(PluginLearningCenterAction::alignmentFlags.begin(), PluginLearningCenterAction::alignmentFlags.end(), alignment) == PluginLearningCenterAction::alignmentFlags.end())
        throw std::runtime_error("Supported toolbar alignment options are: Qt::AlignTop, Qt::AlignBottom, Qt::AlignLeft and Qt::AlignRight");

    clearLayout(&_verticalLayout);
    clearLayout(&_horizontalLayout);

    switch (alignment) {
	    case Qt::AlignLeft:
	    case Qt::AlignRight:
	    {
            setLayoutContentsMargins(&_verticalLayout, ToolbarWidget::margin);
            setLayoutContentsMargins(&_horizontalLayout, 0);

	        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);

	        for (auto widget : _widgets)
	            _verticalLayout.addWidget(widget);

	        break;
	    }

	    case Qt::AlignTop:
	    case Qt::AlignBottom:
	    {
            setLayoutContentsMargins(&_verticalLayout, 0);
            setLayoutContentsMargins(&_horizontalLayout, ToolbarWidget::margin);

	        setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

	        for (auto widget : _widgets)
	            _horizontalLayout.addWidget(widget);

	        break;
	    }

	    default:
	        break;
    }

    QTimer::singleShot(25, [this]() -> void {
        _overlayWidget->updateMask();
	});
}

}
