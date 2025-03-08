// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginLearningCenterOverlayWidget.h"

#include "Application.h"
#include "CoreInterface.h"
#include "ViewPlugin.h"

#include "actions/PluginLearningCenterAction.h"

#include "util/Icon.h"
#include "util/Miscellaneous.h"

#include <actions/WatchVideoAction.h>

#include <QDebug>
#include <QMenu>
#include <QDesktopServices>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QMainWindow>
#include <QPainter>
#include <QResizeEvent>

#ifdef _DEBUG
    //#define VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
#endif

using namespace mv::util;

namespace mv::gui
{

ViewPluginLearningCenterOverlayWidget::ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin) :
    OverlayWidget(target),
    _viewPlugin(viewPlugin),
    _toolbarWidget(viewPlugin, this),
    _learningCenterToolbarItemWidget(_viewPlugin, this),
    _videosToolbarItemWidget(_viewPlugin, this),
    _tutorialsToolbarItemWidget(_viewPlugin, this),
    _aboutToolbarItemWidget(_viewPlugin, this),
    _shortcutsToolbarItemWidget(_viewPlugin, this),
    _showDocumentationToolbarItemWidget(_viewPlugin, this),
    _visitGithubRepoToolbarItemWidget(_viewPlugin, this),
    _toLearningCenterToolbarItemWidget(_viewPlugin, this),
    _hideToolbarItemWidget(_viewPlugin, this),
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
        _tutorialsToolbarItemWidget.hide();
        _aboutToolbarItemWidget.hide();
        _shortcutsToolbarItemWidget.hide();
        _showDocumentationToolbarItemWidget.hide();
        _visitGithubRepoToolbarItemWidget.hide();
        _toLearningCenterToolbarItemWidget.hide();
        _hideToolbarItemWidget.hide();
        _alignmentToolbarItemWidget.hide();

        _toolbarWidget.addWidget(&_learningCenterToolbarItemWidget);
        _toolbarWidget.addWidget(&_videosToolbarItemWidget);
        _toolbarWidget.addWidget(&_tutorialsToolbarItemWidget);
        _toolbarWidget.addWidget(&_aboutToolbarItemWidget);
        _toolbarWidget.addWidget(&_shortcutsToolbarItemWidget);
        _toolbarWidget.addWidget(&_showDocumentationToolbarItemWidget);
        _toolbarWidget.addWidget(&_visitGithubRepoToolbarItemWidget);
        _toolbarWidget.addWidget(&_toLearningCenterToolbarItemWidget);
        _toolbarWidget.addWidget(&_hideToolbarItemWidget);
        _toolbarWidget.addWidget(&_alignmentToolbarItemWidget);

        raise();

        _toolbarItemWidgets = {
	        &_videosToolbarItemWidget,
	        &_tutorialsToolbarItemWidget,
	        &_aboutToolbarItemWidget,
	        &_shortcutsToolbarItemWidget,
	        &_showDocumentationToolbarItemWidget,
	        &_visitGithubRepoToolbarItemWidget,
	        &_toLearningCenterToolbarItemWidget,
	        &_hideToolbarItemWidget,
	        &_alignmentToolbarItemWidget
        };

        for (auto toolbarItemWidget : _toolbarItemWidgets)
            toolbarItemWidget->installEventFilter(this);

        target->installEventFilter(this);

        _learningCenterToolbarItemWidget.installEventFilter(this);
        _toolbarWidget.installEventFilter(this);

        connect(&getLearningCenterAction().getAlignmentAction(), &OptionAction::currentIndexChanged, this, &ViewPluginLearningCenterOverlayWidget::alignmentChanged);

        connect(&getLearningCenterAction().getHideToolbarAction(), &TriggerAction::triggered, this, [this]() -> void {
            collapse();
            //_learningCenterToolbarItemWidget.getWidgetFader().setOpacity(0.f, animationDuration);
            //hide();
        });

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
    if (!getLearningCenterAction().getToolbarVisibleAction().isChecked())
        return OverlayWidget::eventFilter(target, event);

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

    for (auto _toolbarItemWidget : _toolbarItemWidgets)
        _toolbarItemWidget->showConditionally();

    for (auto _toolbarItemWidget : _toolbarItemWidgets)
        _toolbarItemWidget->getWidgetFader().setOpacity(intermediateOpacity, animationDuration, animationDuration / 2);
}

void ViewPluginLearningCenterOverlayWidget::collapse()
{
#ifdef VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    for (auto _toolbarItemWidget : _toolbarItemWidgets)
        _toolbarItemWidget->getWidgetFader().fadeOut(animationDuration, true);
}

ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, const QSize& iconSize /*= QSize(16, 16)*/) :
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _iconSize(iconSize),
    _widgetFader(nullptr, this, .0f)
{
    setObjectName("ToolbarItemWidget");

    _iconLabel.setAlignment(Qt::AlignCenter);
    _iconLabel.setFixedSize(QSize(20, 20));

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
    const auto icon = StyledIcon(getIcon()).withColorGroups(QPalette::ColorGroup::Normal, QPalette::ColorGroup::Normal).withColorRoles(QPalette::ColorRole::Text, QPalette::ColorRole::Text);

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

    connect(&getViewPlugin()->getLearningCenterAction().getToolbarVisibleAction(), &ToggleAction::toggled, this, [this, updateTooltip]() -> void
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
    return getViewPlugin()->getLearningCenterAction().getToolbarVisibleAction().isChecked();
}

ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::VideosToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip("Watch related video(s)");
}

void ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    auto contextMenu = new QMenu(this);

    for (const auto& video : getViewPlugin()->getLearningCenterAction().getVideos())
        contextMenu->addAction(new WatchVideoAction(contextMenu, video->getTitle(), const_cast<LearningCenterVideo*>(video)));
        
    contextMenu->exec(mapToGlobal(event->pos()));
}

QIcon ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::getIcon() const
{
    Badge badge(nullptr, static_cast<std::uint32_t>(getViewPlugin()->getLearningCenterAction().getVideos().size()));

    badge.setScale(.6f);
    badge.setEnabled(true);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    return StyledIcon("video").withBadge(badge.getParameters());
}

bool ViewPluginLearningCenterOverlayWidget::VideosToolbarItemWidget::shouldDisplay() const
{
    return !getViewPlugin()->getLearningCenterAction().getVideos().empty();
}

ViewPluginLearningCenterOverlayWidget::TutorialsToolbarItemWidget::TutorialsToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip("Explore related tutorial(s)");
}

void ViewPluginLearningCenterOverlayWidget::TutorialsToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    auto contextMenu = new QMenu(this);

    for (const auto tutorial : getViewPlugin()->getLearningCenterAction().getTutorials()) {
        auto watchTutorialAction = new QAction(StyledIcon(tutorial->getIconName()), tutorial->getTitle());

        connect(watchTutorialAction, &QAction::triggered, watchTutorialAction, [tutorial]() -> void {
            if (auto tutorialPlugin = mv::plugins().requestViewPlugin("Tutorial")) {
                if (auto pickerAction = dynamic_cast<OptionAction*>(tutorialPlugin->findChildByPath("Pick tutorial")))
                    pickerAction->setCurrentText(tutorial->getTitle());

                if (auto toolbarAction = dynamic_cast<HorizontalGroupAction*>(tutorialPlugin->findChildByPath("Toolbar")))
                    toolbarAction->setVisible(false);
            }
        });

        contextMenu->addAction(watchTutorialAction);
    }
        
    contextMenu->exec(mapToGlobal(event->pos()));
}

QIcon ViewPluginLearningCenterOverlayWidget::TutorialsToolbarItemWidget::getIcon() const
{
    Badge badge(nullptr, static_cast<std::uint32_t>(getViewPlugin()->getLearningCenterAction().getTutorials().size()));

    badge.setScale(.6f);
    badge.setEnabled(true);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    return StyledIcon("user-graduate").withBadge(badge.getParameters());
}

bool ViewPluginLearningCenterOverlayWidget::TutorialsToolbarItemWidget::shouldDisplay() const
{
    return !getViewPlugin()->getLearningCenterAction().getTutorials().empty();
}

ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::ShowDocumentationToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip(const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getPluginMetadata().getTriggerHelpAction().toolTip());
}

void ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getPluginMetadata().getTriggerHelpAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::getIcon() const
{
    return StyledIcon("file-prescription");
}

bool ViewPluginLearningCenterOverlayWidget::ShowDocumentationToolbarItemWidget::shouldDisplay() const
{
    return const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->hasHelp();
}

ViewPluginLearningCenterOverlayWidget::AboutToolbarItemWidget::AboutToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip("View about");
}

void ViewPluginLearningCenterOverlayWidget::AboutToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    getViewPlugin()->getLearningCenterAction().getPluginMetadata().getViewAboutAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::AboutToolbarItemWidget::getIcon() const
{
    return StyledIcon("info");
}

bool ViewPluginLearningCenterOverlayWidget::AboutToolbarItemWidget::shouldDisplay() const
{
    return true;
}

ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::ShortcutsToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip("View shortcuts");
}

void ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    getViewPlugin()->getLearningCenterAction().getPluginMetadata().getViewShortcutsAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::getIcon() const
{
    Badge badge(nullptr, static_cast<std::uint32_t>(getViewPlugin()->getShortcuts().getMap().getShortcuts().size()));

    badge.setScale(.6f);
    badge.setEnabled(true);
    badge.setBackgroundColor(qApp->palette().highlight().color());

    return StyledIcon("keyboard").withBadge(badge.getParameters());
}

bool ViewPluginLearningCenterOverlayWidget::ShortcutsToolbarItemWidget::shouldDisplay() const
{
    return getViewPlugin()->getShortcuts().hasShortcuts();
}

ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::VisitGithubRepoToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip(const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getPluginMetadata().getVisitRepositoryAction().toolTip());
}

void ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getPluginMetadata().getVisitRepositoryAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::VisitGithubRepoToolbarItemWidget::getIcon() const
{
    return StyledIcon(const_cast<plugin::PluginFactory*>(getViewPlugin()->getFactory())->getPluginMetadata().getVisitRepositoryAction().icon());
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
    return StyledIcon(mv::help().getToLearningCenterAction().icon());
}

bool ViewPluginLearningCenterOverlayWidget::ToLearningCenterToolbarItemWidget::shouldDisplay() const
{
    return true;
}

ViewPluginLearningCenterOverlayWidget::HideToolbarItemWidget::HideToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget)
{
    setToolTip(getViewPlugin()->getLearningCenterAction().getHideToolbarAction().toolTip());
}

void ViewPluginLearningCenterOverlayWidget::HideToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    getViewPlugin()->getLearningCenterAction().getHideToolbarAction().trigger();
}

QIcon ViewPluginLearningCenterOverlayWidget::HideToolbarItemWidget::getIcon() const
{
    return StyledIcon(getViewPlugin()->getLearningCenterAction().getHideToolbarAction().icon());
}

bool ViewPluginLearningCenterOverlayWidget::HideToolbarItemWidget::shouldDisplay() const
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

    if (auto contextMenu = getViewPlugin()->getLearningCenterAction().getAlignmentContextMenu(this))
        contextMenu->exec(mapToGlobal(event->pos()));
}

QIcon ViewPluginLearningCenterOverlayWidget::AlignmentToolbarItemWidget::getIcon() const
{
    return StyledIcon("arrows-up-down-left-right");
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
    const auto      backgroundColor = qApp->palette().color(QPalette::ColorGroup::Normal, QPalette::ColorRole::Window);

	auto borderColor = qApp->palette().color(QPalette::ColorGroup::Inactive, QPalette::ColorRole::Text);

    borderColor.setAlpha(15);

    QPixmap backgroundPixmap(size());

    backgroundPixmap.fill(Qt::transparent);

    QPainter pixmapPainter(&backgroundPixmap);

    pixmapPainter.setRenderHint(QPainter::Antialiasing);
    pixmapPainter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    pixmapPainter.setRenderHint(QPainter::LosslessImageRendering, true);
    pixmapPainter.setPen(QPen(borderColor, 1.5));
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

    /*QGraphicsScene scene;

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

    painter.drawPixmap(0, 0, blurredBackgroundPixmap);*/

    painter.drawPixmap(0, 0, backgroundPixmap);

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

        connect(&_viewPlugin->getLearningCenterAction().getToolbarVisibleAction(), &ToggleAction::toggled, this, &ToolbarWidget::visibilityChanged);
        connect(&_viewPlugin->getLearningCenterAction().getAlignmentAction(), &OptionAction::currentIndexChanged, this, &ToolbarWidget::alignmentChanged);

        visibilityChanged();
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
    if (!getLearningCenterAction().getToolbarVisibleAction().isChecked())
        return QWidget::eventFilter(target, event);

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

    QTimer::singleShot(widgetAsyncUpdateTimerInterval, this, [this]() -> void {
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

    if (_viewPlugin->getLearningCenterAction().getToolbarVisibleAction().isChecked()) {
    	_overlayWidget->addMouseEventReceiverWidget(this);
        _overlayWidget->show();

        //_backgroundWidgetFader.setOpacity(0.95f, animationDuration);
        _backgroundWidgetFader.setOpacity(0.f, 0);

        update();
    }
    else {
        _backgroundWidgetFader.setOpacity(0.f, 0);
        _overlayWidget->removeMouseEventReceiverWidget(this);
        _overlayWidget->hide();
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
