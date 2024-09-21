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

ViewPluginLearningCenterOverlayWidget::ViewPluginLearningCenterOverlayWidget(QWidget* target, const plugin::ViewPlugin* viewPlugin, const Qt::Alignment& alignment /*= Qt::AlignBottom */) :
    OverlayWidget(target),
    _viewPlugin(viewPlugin),
    _alignment(0),
    _settingsToolbarWidget(viewPlugin, this, alignment, true),
    _actionsToolbarWidget(viewPlugin, this, alignment),
    _backgroundOverlayWidget(target),
    _backgroundOverlayWidgetFader(this, &_backgroundOverlayWidget)
{
    try {
        Q_ASSERT(target);

        if (!target)
            throw std::runtime_error("Target widget may not be a nullptr");

        addMouseEventReceiverWidget(&_toolbarsWidget);
        addMouseEventReceiverWidget(&_settingsToolbarWidget);

        static const std::vector<Qt::Alignment> supportedAlignments{
            Qt::AlignTop,
            Qt::AlignBottom,
            Qt::AlignLeft,
            Qt::AlignRight,
            Qt::AlignCenter
        };

        if (std::find(supportedAlignments.begin(), supportedAlignments.end(), alignment) == supportedAlignments.end())
            throw std::runtime_error("Supported toolbar alignment options are: Qt::AlignTop, Qt::AlignBottom, Qt::AlignLeft, Qt::AlignRight and Qt::AlignCenter");

        setLayout(&_layout);

        _layout.addWidget(&_toolbarsWidget);

        setAlignment(alignment);

        _settingsToolbarWidget.layout()->setSpacing(0);

        _settingsToolbarWidget.addWidget(new VisibleToolbarItemWidget(viewPlugin, this));
        _settingsToolbarWidget.addWidget(new SettingsToolbarItemWidget(viewPlugin, this));

        _actionsToolbarWidget.addWidget(new VideosToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new DescriptionToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new ShortcutsToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new ShowDocumentationToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new VisitGithubRepoToolbarItemWidget(viewPlugin, this));
        _actionsToolbarWidget.addWidget(new ToLearningCenterToolbarItemWidget(viewPlugin, this));

        setContentsMargins(4);
        setMouseTracking(true);

        raise();

        _backgroundOverlayWidget.setAutoFillBackground(true);
        _backgroundOverlayWidget.lower();
        _backgroundOverlayWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);

        target->installEventFilter(this);

        updateBackgroundStyle();
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

bool ViewPluginLearningCenterOverlayWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
            updateBackgroundStyle();
            break;

        case QEvent::Enter:
            _backgroundOverlayWidgetFader.fadeIn();
            break;

        case QEvent::Leave:
            _backgroundOverlayWidgetFader.fadeOut();
            break;

        default:
            break;
    }

    return OverlayWidget::eventFilter(target, event);
}

Qt::Alignment ViewPluginLearningCenterOverlayWidget::getAlignment() const
{
    return _alignment;
}

void ViewPluginLearningCenterOverlayWidget::setAlignment(const Qt::Alignment& alignment)
{
    if (alignment == _alignment)
        return;

#ifdef VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE
    
#endif

    _alignment = alignment;

    _layout.setAlignment(_alignment);

    QBoxLayout* toolbarsLayout = nullptr;

    if (_alignment & Qt::AlignLeft || _alignment & Qt::AlignRight)
        toolbarsLayout = new QVBoxLayout();

    if (_alignment & Qt::AlignTop || _alignment & Qt::AlignCenter || _alignment & Qt::AlignBottom)
        toolbarsLayout = new QHBoxLayout();

    if (!toolbarsLayout)
        return;

    toolbarsLayout->setContentsMargins(0, 0, 0, 0);

    _settingsToolbarWidget.setAlignment(_alignment);
    _actionsToolbarWidget.setAlignment(_alignment);

    replaceLayout(&_toolbarsWidget, toolbarsLayout);

    if (_alignment == Qt::AlignCenter) {
        toolbarsLayout->addWidget(&_settingsToolbarWidget, 1);
        toolbarsLayout->addWidget(&_actionsToolbarWidget, 1);
        toolbarsLayout->addStretch(1);
    }
    else {
        toolbarsLayout->addWidget(&_settingsToolbarWidget);
        toolbarsLayout->addStretch(1);
        toolbarsLayout->addWidget(&_actionsToolbarWidget);
    }

    updateBackgroundStyle();
}

void ViewPluginLearningCenterOverlayWidget::setContentsMargins(std::int32_t margin)
{
    _layout.setContentsMargins(margin, margin, margin, margin);
}

void ViewPluginLearningCenterOverlayWidget::updateBackgroundStyle()
{
    const auto applicationPalette   = QApplication::palette();
    const auto windowColor          = applicationPalette.color(QPalette::Window);

    QColor gradientColors[2] {
        windowColor,
        windowColor
    };

    gradientColors[0].setAlpha(255);
    gradientColors[1].setAlpha(0);

    const QStringList gradientColorsStrings{
        getColorAsCssString(gradientColors[0]),
        getColorAsCssString(gradientColors[1])
    };

    constexpr float stopsInPixels[2] = { 10.f, 50.f };

    QStringList rangeStrings{ "0", "1" };

    if (_alignment == Qt::AlignTop || _alignment == Qt::AlignBottom) {
        const auto height = getWidgetOverlayer().getTargetWidget()->height();

        const QStringList stopsNormalizedStrings = {
                QString::number(stopsInPixels[0] / static_cast<float>(height)),
                QString::number(stopsInPixels[1] / static_cast<float>(height))
        };

        if (_alignment == Qt::AlignBottom)
            std::reverse(rangeStrings.begin(), rangeStrings.end());

        _backgroundOverlayWidget.setStyleSheet(QString(
            "background: qlineargradient(x1: 0, y1: %1, x2: 0, y2: %2,"
            "stop: 0 %5"
            "stop: %3 %5"
            "stop: %4 %6,"
            "stop: 1 %6);"
        ).arg(rangeStrings.first(), rangeStrings.last(), stopsNormalizedStrings[0], stopsNormalizedStrings[1], gradientColorsStrings.first(), gradientColorsStrings.last()));
    }

    if (_alignment == Qt::AlignLeft || _alignment == Qt::AlignRight) {
        const auto width = getWidgetOverlayer().getTargetWidget()->width();

        const QStringList stopsNormalizedStrings = {
                QString::number(stopsInPixels[0] / static_cast<float>(width)),
                QString::number(stopsInPixels[1] / static_cast<float>(width))
        };

        if (_alignment == Qt::AlignRight)
            std::reverse(rangeStrings.begin(), rangeStrings.end());

        _backgroundOverlayWidget.setStyleSheet(QString(
            "background: qlineargradient(x1: %1, y1: 0, x2: %2, y2: 0,"
            "stop: 0 %5"
            "stop: %3 %5"
            "stop: %4 %6,"
            "stop: 1 %6);"
        ).arg(rangeStrings.first(), rangeStrings.last(), stopsNormalizedStrings[0], stopsNormalizedStrings[1], gradientColorsStrings.first(), gradientColorsStrings.last()));
    }
}

ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::AbstractToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, const QSize& iconSize /*= QSize(16, 16)*/) :
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

ViewPluginLearningCenterOverlayWidget* ViewPluginLearningCenterOverlayWidget::AbstractToolbarItemWidget::getOverlayWidget() const
{
    return const_cast<ViewPluginLearningCenterOverlayWidget*>(_overlayWidget);
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

ViewPluginLearningCenterOverlayWidget::VisibleToolbarItemWidget::VisibleToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget, QSize(10, 10))
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

ViewPluginLearningCenterOverlayWidget::SettingsToolbarItemWidget::SettingsToolbarItemWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget) :
    AbstractToolbarItemWidget(viewPlugin, overlayWidget, QSize(10, 10))
{
    setToolTip("Move the toolbar");
}

void ViewPluginLearningCenterOverlayWidget::SettingsToolbarItemWidget::mousePressEvent(QMouseEvent* event)
{
    AbstractToolbarItemWidget::mousePressEvent(event);

    auto contextMenu = new QMenu(this);

    struct Alignment {
        Qt::Alignment   _alignment;
        QString         _title;
        QIcon           _icon;
    };

    static const std::vector<Alignment> alignments{
        { Qt::AlignTop, "Move to top", getDockAreaIcon(DockAreaFlag::Top) },
        { Qt::AlignBottom, "Move to bottom", getDockAreaIcon(DockAreaFlag::Bottom) },
        { Qt::AlignLeft, "Move to left", getDockAreaIcon(DockAreaFlag::Left) },
        { Qt::AlignCenter, "Move to center", getDockAreaIcon(DockAreaFlag::Center) },
        { Qt::AlignRight, "Move to right", getDockAreaIcon(DockAreaFlag::Right) }
    };

    std::vector<Alignment> candidateAlignments;

    std::copy_if(alignments.begin(), alignments.end(), std::back_inserter(candidateAlignments), [this](const auto& alignment) {
        return alignment._alignment != getOverlayWidget()->getAlignment();
    });

    for (const auto& candidateAlignment : candidateAlignments) {
        auto alignAction = new TriggerAction(contextMenu, candidateAlignment._title);

        alignAction->setIcon(candidateAlignment._icon);

        connect(alignAction, &TriggerAction::triggered, this, [this, candidateAlignment]() -> void {
            getOverlayWidget()->setAlignment(candidateAlignment._alignment);
        });

        contextMenu->addAction(alignAction);
    }

    contextMenu->exec(mapToGlobal(event->pos()));
}

QIcon ViewPluginLearningCenterOverlayWidget::SettingsToolbarItemWidget::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("arrows-alt");
}

bool ViewPluginLearningCenterOverlayWidget::SettingsToolbarItemWidget::shouldDisplay() const
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

ViewPluginLearningCenterOverlayWidget::ToolbarWidget::ToolbarWidget(const plugin::ViewPlugin* viewPlugin, ViewPluginLearningCenterOverlayWidget* overlayWidget, const Qt::Alignment& alignment, bool alwaysVisible /*= false*/) :
    QWidget(overlayWidget),
    _viewPlugin(viewPlugin),
    _overlayWidget(overlayWidget),
    _alignment(0),
    _alwaysVisible(alwaysVisible)
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

        setAlignment(alignment);

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

        connect(&_viewPlugin->getLearningCenterAction().getViewPluginOverlayVisibleAction(), &ToggleAction::toggled, this, &ToolbarWidget::visibilityChanged);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize ViewPluginLearningCenterOverlayWidget::ToolbarWidget");
    }
}

Qt::Alignment ViewPluginLearningCenterOverlayWidget::ToolbarWidget::getAlignment() const
{
    return _alignment;
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::setAlignment(const Qt::Alignment& alignment)
{
    if (alignment == _alignment)
        return;

#ifdef VIEW_PLUGIN_LEARNING_CENTER_OVERLAY_WIDGET_VERBOSE

#endif

    _alignment = alignment;

    static const std::vector<Qt::Alignment> supportedAlignments{
        Qt::AlignTop,
        Qt::AlignBottom,
        Qt::AlignLeft,
        Qt::AlignRight,
        Qt::AlignCenter
    };

    if (std::find(supportedAlignments.begin(), supportedAlignments.end(), _alignment) == supportedAlignments.end())
        throw std::runtime_error("Supported toolbar alignment options are: Qt::AlignTop, Qt::AlignBottom, Qt::AlignLeft, Qt::AlignRight and Qt::AlignCenter");

    QBoxLayout* layout = nullptr;

    if (_alignment & Qt::AlignLeft || _alignment & Qt::AlignRight)
        layout = new QVBoxLayout();

    if (_alignment & Qt::AlignTop || _alignment & Qt::AlignCenter || _alignment & Qt::AlignBottom)
        layout = new QHBoxLayout();

    if (!layout)
        return;

    layout->setSpacing(0);

    if (_alignment == Qt::AlignCenter)
        layout->setAlignment(Qt::AlignLeft);

    const auto setContentsMargins = [this, layout](std::int32_t margin) -> void {
        layout->setContentsMargins(margin, margin, margin, margin);
    };

    setContentsMargins(4);

    for (auto widget : _widgets)
        layout->addWidget(widget);

    replaceLayout(this, layout);

    update();
    updateGeometry();

    _overlayWidget->updateMask();
}

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::addWidget(QWidget* widget)
{
    try {
        if (!layout())
            throw std::runtime_error("No layout present");

        if (!widget)
            throw std::runtime_error("Supplied wWidget is a nullptr");

        _widgets.push_back(widget);

        layout()->addWidget(widget);
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

void ViewPluginLearningCenterOverlayWidget::ToolbarWidget::visibilityChanged()
{
    if (_alwaysVisible)
        return;

    if (_viewPlugin->getLearningCenterAction().getViewPluginOverlayVisibleAction().isChecked()) {
        _overlayWidget->addMouseEventReceiverWidget(this);
        update();
    }
    else {
        _overlayWidget->removeMouseEventReceiverWidget(this);
    }
}

}
