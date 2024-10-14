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
#include <QGraphicsPixmapItem>
#include <QResizeEvent>

#ifdef _DEBUG
    #define VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
#endif

//#define VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE

using namespace mv::util;

namespace mv::gui
{

ViewPluginLearningCenterOverlayWidget::ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin) :
    OverlayWidget(target),
    _viewPlugin(viewPlugin),
    _toolbarWidget(viewPlugin, this),
    _learningCenterToolbarItemWidget(_viewPlugin, this),
    _videosToolbarItemWidget(_viewPlugin, this),
    _descriptionToolbarItemWidget(_viewPlugin, this),
    _shortcutsToolbarItemWidget(_viewPlugin, this),
    _showDocumentationToolbarItemWidget(_viewPlugin, this),
    _visitGithubRepoToolbarItemWidget(_viewPlugin, this),
    _toLearningCenterToolbarItemWidget(_viewPlugin, this),
    _alignmentToolbarItemWidget(_viewPlugin, this)
{
    try {
        Q_ASSERT(target);

        if (!target)
            throw std::runtime_error("Target widget may not be a nullptr");

        addMouseEventReceiverWidget(&_toolbarWidget);

        if (std::find(PluginLearningCenterAction::alignmentFlags.begin(), PluginLearningCenterAction::alignmentFlags.end(), getLearningCenterAction().getAlignment()) == PluginLearningCenterAction::alignmentFlags.end())
            throw std::runtime_error("Supported toolbar alignment options are: Qt::AlignTop | Qt::AlignLeft, Qt::AlignTop | Qt::AlignRight, Qt::AlignBottom | Qt::AlignLeft and Qt::AlignBottom | Qt::AlignRight");

        setLayout(&_layout);

        setLayoutContentsMargins(&_layout, 0);
        
        _layout.setSpacing(0);

        _layout.addWidget(&_toolbarWidget);

        _videosToolbarItemWidget.hide();
        _descriptionToolbarItemWidget.hide();
        _shortcutsToolbarItemWidget.hide();
        _showDocumentationToolbarItemWidget.hide();
        _visitGithubRepoToolbarItemWidget.hide();
        _toLearningCenterToolbarItemWidget.hide();
        _alignmentToolbarItemWidget.hide();

        _toolbarWidget.addWidget(&_learningCenterToolbarItemWidget);
        _toolbarWidget.addWidget(&_videosToolbarItemWidget);
        _toolbarWidget.addWidget(&_descriptionToolbarItemWidget);
        _toolbarWidget.addWidget(&_shortcutsToolbarItemWidget);
        _toolbarWidget.addWidget(&_showDocumentationToolbarItemWidget);
        _toolbarWidget.addWidget(&_visitGithubRepoToolbarItemWidget);
        _toolbarWidget.addWidget(&_toLearningCenterToolbarItemWidget);
        _toolbarWidget.addWidget(&_alignmentToolbarItemWidget);

        raise();

        _toolbarItemWidgets = {
	        &_videosToolbarItemWidget,
	        &_descriptionToolbarItemWidget,
	        &_shortcutsToolbarItemWidget,
	        &_showDocumentationToolbarItemWidget,
	        &_visitGithubRepoToolbarItemWidget,
	        &_toLearningCenterToolbarItemWidget,
	        &_alignmentToolbarItemWidget
        };

        for (auto toolbarItemWidget : _toolbarItemWidgets)
            toolbarItemWidget->installEventFilter(this);

        target->installEventFilter(this);

        _learningCenterToolbarItemWidget.installEventFilter(this);
        _toolbarWidget.installEventFilter(this);

        connect(&getLearningCenterAction().getAlignmentAction(), &OptionAction::currentIndexChanged, this, &ViewPluginLearningCenterOverlayWidget::alignmentChanged);

        alignmentChanged();
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

ViewPluginLearningCenterOverlayWidget::ToolbarWidget& ViewPluginLearningCenterOverlayWidget::getToolbarWidget()
{
    return _toolbarWidget;
}

bool ViewPluginLearningCenterOverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    const auto alignment = getLearningCenterAction().getAlignment();

    switch (event->type())
    {
        case QEvent::Enter:
        {
            if (target == getWidgetOverlayer().getTargetWidget()) {
                _learningCenterToolbarItemWidget.show();
                _learningCenterToolbarItemWidget.getWidgetFader().setOpacity(intermediateOpacity, animationDuration);
            }

            if (target == &_learningCenterToolbarItemWidget)
                expand();

            if (auto toolbarItemWidget = dynamic_cast<AbstractToolbarItemWidget*>(target))
                if (std::find(_toolbarItemWidgets.begin(), _toolbarItemWidgets.end(), target) != _toolbarItemWidgets.end())
                    toolbarItemWidget->getWidgetFader().setOpacity(1.f, animationDuration / 5);

            break;
        }

        case QEvent::Leave:
        {
            if (target == getWidgetOverlayer().getTargetWidget())
                _learningCenterToolbarItemWidget.getWidgetFader().setOpacity(0.f, animationDuration);

            if (target == &_toolbarWidget)
                collapse();

            if (auto toolbarItemWidget = dynamic_cast<AbstractToolbarItemWidget*>(target))
                if (std::find(_toolbarItemWidgets.begin(), _toolbarItemWidgets.end(), target) != _toolbarItemWidgets.end())
                    toolbarItemWidget->getWidgetFader().setOpacity(intermediateOpacity, animationDuration / 2);

            break;
        }

        default:
            break;
    }

	return OverlayWidget::eventFilter(target, event);
}

bool ViewPluginLearningCenterOverlayWidget::isExpanded()
{
    const auto numberOfDisplayableToolbarItemWidgets = std::count_if(_toolbarItemWidgets.begin(), _toolbarItemWidgets.end(), [](auto toolbarItemWidget) -> bool {
        return toolbarItemWidget->shouldDisplay();
	});

    const auto numberOfVisibleToolbarItemWidgets = std::count_if(_toolbarItemWidgets.begin(), _toolbarItemWidgets.end(), [](auto toolbarItemWidget) -> bool {
        return toolbarItemWidget->isVisible();
    });

    return numberOfVisibleToolbarItemWidgets == numberOfDisplayableToolbarItemWidgets;
}

bool ViewPluginLearningCenterOverlayWidget::isCollapsed()
{
    const auto numberOfDisplayableToolbarItemWidgets = std::count_if(_toolbarItemWidgets.begin(), _toolbarItemWidgets.end(), [](auto toolbarItemWidget) -> bool {
        return toolbarItemWidget->shouldDisplay();
	});

    const auto numberOfHiddenToolbarItemWidgets = std::count_if(_toolbarItemWidgets.begin(), _toolbarItemWidgets.end(), [](auto toolbarItemWidget) -> bool {
        return toolbarItemWidget->isHidden();
    });

    return numberOfHiddenToolbarItemWidgets == numberOfDisplayableToolbarItemWidgets;
}

void ViewPluginLearningCenterOverlayWidget::toolbarItemWidgetShown(QWidget* toolbarItemWidget)
{
    if (!isExpanded())
        return;

	emit expanded();
}

void ViewPluginLearningCenterOverlayWidget::toolbarItemWidgetHidden(QWidget* toolbarItemWidget)
{
    if (!isCollapsed())
        return;

	emit collapsed();
}

void ViewPluginLearningCenterOverlayWidget::alignmentChanged()
{
    _layout.setAlignment(getLearningCenterAction().getAlignment());
}

PluginLearningCenterAction& ViewPluginLearningCenterOverlayWidget::getLearningCenterAction() const
{
    return const_cast<plugin::ViewPlugin*>(_viewPlugin)->getLearningCenterAction();
}

void ViewPluginLearningCenterOverlayWidget::expand()
{
#ifdef VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    if (isExpanded())
        return;

    _videosToolbarItemWidget.showConditionally();
    _descriptionToolbarItemWidget.showConditionally();
    _shortcutsToolbarItemWidget.showConditionally();
    _showDocumentationToolbarItemWidget.showConditionally();
    _visitGithubRepoToolbarItemWidget.showConditionally();
    _toLearningCenterToolbarItemWidget.showConditionally();
    _alignmentToolbarItemWidget.showConditionally();

    constexpr auto delay            = animationDuration / 2;
    constexpr auto fadeInDuration   = animationDuration;

    _videosToolbarItemWidget.getWidgetFader().setOpacity(intermediateOpacity, fadeInDuration, delay);
    _descriptionToolbarItemWidget.getWidgetFader().setOpacity(intermediateOpacity, fadeInDuration, delay);
    _shortcutsToolbarItemWidget.getWidgetFader().setOpacity(intermediateOpacity, fadeInDuration, delay);
    _showDocumentationToolbarItemWidget.getWidgetFader().setOpacity(intermediateOpacity, fadeInDuration, delay);
    _visitGithubRepoToolbarItemWidget.getWidgetFader().setOpacity(intermediateOpacity, fadeInDuration, delay);
    _toLearningCenterToolbarItemWidget.getWidgetFader().setOpacity(intermediateOpacity, fadeInDuration, delay);
    _alignmentToolbarItemWidget.getWidgetFader().setOpacity(intermediateOpacity, fadeInDuration, delay);
}

void ViewPluginLearningCenterOverlayWidget::collapse()
{
#ifdef VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    constexpr auto fadeOutDuration = animationDuration;

    _videosToolbarItemWidget.getWidgetFader().fadeOut(fadeOutDuration, true);
    _descriptionToolbarItemWidget.getWidgetFader().fadeOut(fadeOutDuration, true);
    _shortcutsToolbarItemWidget.getWidgetFader().fadeOut(fadeOutDuration, true);
    _showDocumentationToolbarItemWidget.getWidgetFader().fadeOut(fadeOutDuration, true);
    _visitGithubRepoToolbarItemWidget.getWidgetFader().fadeOut(fadeOutDuration, true);
    _toLearningCenterToolbarItemWidget.getWidgetFader().fadeOut(fadeOutDuration, true);
    _alignmentToolbarItemWidget.getWidgetFader().fadeOut(fadeOutDuration, true);
}

ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, const QSize& iconSize /*= QSize(14, 14)*/) :
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _iconSize(iconSize),
    _widgetFader(nullptr, this, .0f)
{
    setObjectName("ToolbarItemWidget");

    _iconLabel.setAlignment(Qt::AlignCenter);

    constexpr auto margin = 3;

    _layout.setContentsMargins(margin, margin, margin, margin);
    _layout.addWidget(&_iconLabel);

    setLayout(&_layout);

    _overlayWidget->getToolbarWidget().installEventFilter(this);
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::showConditionally()
{
    if (!shouldDisplay())
        return;

    show();
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    updateIcon();

    _overlayWidget->toolbarItemWidgetShown(this);
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::hideEvent(QHideEvent* event)
{
	QWidget::hideEvent(event);

    _overlayWidget->toolbarItemWidgetHidden(this);
}

void ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::updateIcon()
{
    _iconLabel.setFixedSize(QSize(16, 16));
    _iconLabel.setPixmap(getIcon().pixmap(_iconSize));
}

WidgetFader& ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::getWidgetFader()
{
    return _widgetFader;
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

ViewPluginLearningCenterOverlayWidget::LearningCenterToolbarItemWidget::LearningCenterToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    const auto updateTooltip = [this]() -> void {
        setToolTip(QString("%1 learning center").arg(getViewPlugin()->getKind()));
    };

    updateTooltip();

    connect(&getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction(), &ToggleAction::toggled, this, [this, updateTooltip]() -> void
    {
        updateTooltip();
        updateIcon();
    });

    getWidgetFader().setOpacity(0.f);
}

QIcon ViewPluginLearningCenterOverlayWidget::LearningCenterToolbarItemWidget::getIcon() const
{
    return getViewPlugin()->getLearningCenterAction().icon();
}

bool ViewPluginLearningCenterOverlayWidget::LearningCenterToolbarItemWidget::shouldDisplay() const
{
    return getViewPlugin()->getLearningCenterAction().getOverlayVisibleAction().isChecked();
}

ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::VideosToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip("Watch related videos");
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

std::vector<ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::Alignment> ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::alignments = {};

ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::AlignmentToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip("Move the toolbar");

    if (alignments.empty()) {
        alignments = {
            { Qt::AlignTop | Qt::AlignLeft, "Move to top-left", getAlignmentIcon(Qt::AlignTop | Qt::AlignLeft) },
            { Qt::AlignTop | Qt::AlignRight, "Move to top-right", getAlignmentIcon(Qt::AlignTop | Qt::AlignRight) },
            { Qt::AlignBottom | Qt::AlignLeft, "Move to bottom-left", getAlignmentIcon(Qt::AlignBottom | Qt::AlignLeft) },
            { Qt::AlignBottom | Qt::AlignRight, "Move to bottom-right", getAlignmentIcon(Qt::AlignBottom | Qt::AlignRight) }
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

ViewPluginLearningCenterOverlayWidget::ToolbarWidget::BackgroundWidget::BackgroundWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin) :
    QWidget(target),
    _viewPlugin(viewPlugin),
    _geometryAnimation(this, "geometry")
{
    _geometryAnimation.setEasingCurve(QEasingCurve::OutExpo);
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::BackgroundWidget::transitionGeometry(const QRect& geometry, bool animate /*= true*/)
{
#ifdef VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << geometry;
#endif

    if (_previousGeometry.isValid()) {
        if (_geometryAnimation.state() == QPropertyAnimation::Running) {
            _previousGeometry = _geometryAnimation.currentValue().value<QRect>();

            _geometryAnimation.stop();
        }

        _geometryAnimation.setDuration(animate ? ViewPluginLearningCenterOverlayWidget::animationDuration / 2 : 0);

        if (_previousGeometry.isValid())
            _geometryAnimation.setStartValue(_previousGeometry);

        _geometryAnimation.setEndValue(geometry);
        _geometryAnimation.start();
    } else {
        setGeometry(geometry);
    }

    _previousGeometry = geometry;
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::BackgroundWidget::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::RenderHint::Antialiasing);

    constexpr auto  rectangleMargin = 3;
    const auto      backgroundColor = qApp->palette().highlightedText().color();

    QPixmap backgroundPixmap(size());

    backgroundPixmap.fill(Qt::transparent);

    QPainter pixmapPainter(&backgroundPixmap);

    pixmapPainter.setPen(Qt::NoPen);
    pixmapPainter.setBrush(backgroundColor);

    auto backgroundRectangle = rect();

    std::int32_t radius = 0;

    const auto alignment = const_cast<plugin::ViewPlugin*>(_viewPlugin)->getLearningCenterAction().getAlignment();

    if (alignment & Qt::AlignLeft || alignment & Qt::AlignRight) {
        radius              = (width() - 2 * rectangleMargin) / 2;
        backgroundRectangle = rect().adjusted(rectangleMargin, rectangleMargin, -rectangleMargin, -rectangleMargin);
    }

    if (alignment & Qt::AlignTop || alignment & Qt::AlignBottom) {
        radius              = (height() - 2 * rectangleMargin) / 2;
        backgroundRectangle = rect().adjusted(rectangleMargin, rectangleMargin, -rectangleMargin, -rectangleMargin);
    }

    pixmapPainter.drawRoundedRect(backgroundRectangle, radius, radius);

    QGraphicsScene scene;

    auto pixmapItem = new QGraphicsPixmapItem(backgroundPixmap);
    auto blurEffect = new QGraphicsBlurEffect;

    blurEffect->setBlurRadius(2.5);
    blurEffect->setBlurHints(QGraphicsBlurEffect::QualityHint);

    pixmapItem->setGraphicsEffect(blurEffect);

    scene.addItem(pixmapItem);

    QPixmap blurredBackgroundPixmap(size());

    blurredBackgroundPixmap.fill(Qt::transparent);

    QPainter scenePainter(&blurredBackgroundPixmap);

    scene.render(&scenePainter);
    scenePainter.end();

    painter.drawPixmap(0, 0, blurredBackgroundPixmap);

    QWidget::paintEvent(event);
}

ViewPluginLearningCenterOverlayWidget::ToolbarWidget::ToolbarWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, bool alwaysVisible /*= false*/) :
    QWidget(overlayWidget),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _alwaysVisible(alwaysVisible),
    _backgroundWidget(overlayWidget, viewPlugin),
    _backgroundWidgetFader(this, &_backgroundWidget),
    _numberOfShowEvents(0)
{
    try {
        Q_ASSERT(_viewPlugin && _overlayWidget);

        if (!_viewPlugin)
            throw std::runtime_error("View plugin is a nullptr");

        if (!_overlayWidget)
            throw std::runtime_error("Overlay widget is a nullptr");

        setObjectName("ToolbarWidget");
        setMouseTracking(true);
        setToolTip(QString("%1 learning center").arg(viewPlugin->getKind()));
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

        _layout.setSpacing(0);

        _layout.setContentsMargins(ToolbarWidget::margin, ToolbarWidget::margin, ToolbarWidget::margin, ToolbarWidget::margin);

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

    	connect(_overlayWidget, &ViewPluginLearningCenterOverlayWidget::expanded, this, [this]() -> void {
            QTimer::singleShot(widgetAsyncUpdateTimerInterval, _overlayWidget, &ViewPluginLearningCenterOverlayWidget::updateMask);
            QTimer::singleShot(widgetAsyncUpdateTimerInterval, this, &ToolbarWidget::synchronizeBackgroundWidgetGeometry);
    	});

        connect(_overlayWidget, &ViewPluginLearningCenterOverlayWidget::collapsed, this, [this]() -> void {
            QTimer::singleShot(widgetAsyncUpdateTimerInterval, this, &ToolbarWidget::synchronizeBackgroundWidgetGeometry);
            QTimer::singleShot(widgetAsyncUpdateTimerInterval + animationDuration, _overlayWidget, &ViewPluginLearningCenterOverlayWidget::updateMask);
		});

        connect(&_viewPlugin->getLearningCenterAction().getOverlayVisibleAction(), &ToggleAction::toggled, this, &ToolbarWidget::visibilityChanged);

        connect(&_viewPlugin->getLearningCenterAction().getAlignmentAction(), &OptionAction::currentIndexChanged, this, &ToolbarWidget::alignmentChanged);

        alignmentChanged();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget");
    }
}

PluginLearningCenterAction& ViewPluginLearningCenterOverlayWidget::ToolbarWidget::getLearningCenterAction() const
{
    return const_cast<plugin::ViewPlugin*>(_viewPlugin)->getLearningCenterAction();
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::addWidget(QWidget* widget)
{
    try {
        if (!layout())
            throw std::runtime_error("No layout present");

        if (!widget)
            throw std::runtime_error("Supplied widget is a nullptr");

        _widgets.push_back(widget);

        const auto alignment = getLearningCenterAction().getAlignment();

        if (alignment & Qt::AlignLeft)
	         _layout.addWidget(widget);

        if (alignment & Qt::AlignRight)
            _layout.insertWidget(0, widget);
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
	        _backgroundWidgetFader.setOpacity(.95f, animationDuration);
	        break;
	    }

	    case QEvent::Leave:
	    {
	        _backgroundWidgetFader.setOpacity(0.f, animationDuration);
	        break;
	    }

	    default:
	        break;
    }

    return QWidget::eventFilter(target, event);
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::showEvent(QShowEvent* event)
{
#ifdef VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << _numberOfShowEvents << _viewPlugin->getKind() << geometry();
#endif

    QWidget::showEvent(event);

    if (_numberOfShowEvents == 0)
        firstShowEvent(event);

    _numberOfShowEvents++;

    QTimer::singleShot(widgetAsyncUpdateTimerInterval, [this]() -> void {
        _backgroundWidget.transitionGeometry(geometry());
	});
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::synchronizeBackgroundWidgetGeometry()
{
    _backgroundWidget.transitionGeometry(geometry());
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::visibilityChanged()
{
    if (_alwaysVisible)
        return;

    if (_viewPlugin->getLearningCenterAction().getOverlayVisibleAction().isChecked()) {
        _backgroundWidgetFader.setOpacity(0.95f, animationDuration);
        _overlayWidget->addMouseEventReceiverWidget(this);
        update();
    }
    else {
        _backgroundWidgetFader.setOpacity(0.f, animationDuration);
        _overlayWidget->removeMouseEventReceiverWidget(this);
    }
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::firstShowEvent(QShowEvent* showEvent)
{
#ifdef VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__ << _viewPlugin->getKind();
#endif

    _backgroundWidget.lower();
    _backgroundWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);

    _backgroundWidgetFader.setOpacity(0.f);

    QTimer::singleShot(widgetAsyncUpdateTimerInterval, [this]() -> void {
        _backgroundWidget.transitionGeometry(geometry());
	});
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::alignmentChanged()
{
    const auto alignment = getLearningCenterAction().getAlignment();

    if (std::find(PluginLearningCenterAction::alignmentFlags.begin(), PluginLearningCenterAction::alignmentFlags.end(), alignment) == PluginLearningCenterAction::alignmentFlags.end())
        throw std::runtime_error("Supported toolbar alignment options are: Qt::AlignTop | Qt::AlignLeft, Qt::AlignTop | Qt::AlignRight, Qt::AlignBottom | Qt::AlignLeft and Qt::AlignBottom | Qt::AlignRight");

    clearLayout(&_layout);

    if (alignment & Qt::AlignLeft) {
        for (auto widget : _widgets)
            _layout.addWidget(widget);
    }

    _layout.addStretch(1);

    if (alignment & Qt::AlignRight) {
        for (auto widget : _widgets)
            _layout.insertWidget(0, widget);
    }

    QTimer::singleShot(widgetAsyncUpdateTimerInterval, [this]() -> void {
        _overlayWidget->updateMask();
    	synchronizeBackgroundWidgetGeometry();
	});
}

}
