// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginSamplerAction.h"

#include "CoreInterface.h"
#include "ViewPlugin.h"

#include "util/Exception.h"

#include <QEvent>

#include "PluginPickerAction.h"

using namespace mv::util;

namespace mv::gui {

ViewPluginSamplerAction::ViewPluginSamplerAction(QObject* parent, const QString& title, const ViewingMode& viewingMode /*= ViewingMode::None*/) :
    HorizontalGroupAction(parent, title),
    _viewPlugin(nullptr),
    _isInitialized(false),
    _pixelSelectionAction(nullptr),
    _samplerPixelSelectionAction(nullptr),
    _enabledAction(this, "Enabled", true),
    _highlightFocusedElementsAction(this, "Highlight focused elements", true),
    _settingsAction(this, "Settings"),
    _restrictNumberOfElementsAction(this, "Restrict number of elements", false),
    _maximumNumberOfElementsAction(this, "Max. number of elements", 0, 1000, 100),
    _lazyUpdateIntervalAction(this, "Lazy update interval", 10, 1000, 100),
    _samplingModeAction(this, "Sampling mode", { "Selection", "Focus Region" }, "Focus Region"),
	_viewingModeAction(this, "Viewing mode", { "None", "Windowed", "Tooltip" }, "None"),
    _openSampleScopeWindow(this, "Open sample scope window"),
    _sampleScopePlugin(nullptr)
{
    setShowLabels(false);
    setIconByName("eye-dropper");
    setConfigurationFlag(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
    setPopupSizeHint(QSize(200, 0));

    addAction(&_enabledAction);
    addAction(&_settingsAction);

    _settingsAction.addAction(&_samplingModeAction);
    _settingsAction.addAction(&_viewingModeAction);
    _settingsAction.addAction(&_highlightFocusedElementsAction);
    _settingsAction.addAction(&_restrictNumberOfElementsAction);
    _settingsAction.addAction(&_maximumNumberOfElementsAction);
    _settingsAction.addAction(&_lazyUpdateIntervalAction);

    _enabledAction.setStretch(1);

    _settingsAction.setIconByName("gear");
    _settingsAction.setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    _settingsAction.setPopupSizeHint(QSize(500, 0));

    _enabledAction.setToolTip("Toggle focus region visibility");
    _highlightFocusedElementsAction.setToolTip("Toggle highlighting of focused elements");
    _settingsAction.setToolTip("Additional focus region settings");
    _maximumNumberOfElementsAction.setToolTip("Puts a cap on the amount of points captured by the focus region");
    _lazyUpdateIntervalAction.setToolTip("Controls the time interval between successive sample collection updates");
    _samplingModeAction.setToolTip("Determines how to collect samples");
    _viewingModeAction.setToolTip("Determines how the collected samples will be displayed");
    _openSampleScopeWindow.setToolTip("Open a sample scope window to inspect the samples");

    const auto enabledChanged = [this]() -> void {
        _settingsAction.setEnabled(_enabledAction.isChecked());

        if (_samplerPixelSelectionAction)
            _samplerPixelSelectionAction->getPixelSelectionTool()->setEnabled(_enabledAction.isChecked());

        emit canViewChanged(canView());
    };

    enabledChanged();

    connect(&_enabledAction, &ToggleAction::toggled, this, enabledChanged);
    connect(&_enabledAction, &ToggleAction::toggled, this, &ViewPluginSamplerAction::updateReadOnly);

    const auto updateMaximumNumberOfElementsAction = [this]() -> void {
        _maximumNumberOfElementsAction.setEnabled(_restrictNumberOfElementsAction.isChecked() && getSamplingMode() == SamplingMode::FocusRegion);
    };

    updateMaximumNumberOfElementsAction();

    connect(&_restrictNumberOfElementsAction, &ToggleAction::toggled, this, updateMaximumNumberOfElementsAction);

    const auto updateSampleContextLazyUpdateTimerInterval = [this]() -> void {
        _sampleContextLazyUpdateTimer.setInterval(_lazyUpdateIntervalAction.getValue());
    };

    updateSampleContextLazyUpdateTimerInterval();

    connect(&_lazyUpdateIntervalAction, &IntegralAction::valueChanged, this, updateSampleContextLazyUpdateTimerInterval);

    connect(&_sampleContextLazyUpdateTimer, &QTimer::timeout, this, [this]() -> void {
        if (!_sampleContextDirty || (!_htmlViewGeneratorFunction && !_widgetViewGeneratorFunction) || getViewingMode() == ViewingMode::None)
            return;

        if (getSamplingMode() == SamplingMode::FocusRegion)
			emit sampleContextRequested();

        _sampleContextDirty = false;
    });

    const auto samplingModeChanged = [this, updateMaximumNumberOfElementsAction]() -> void {
        const auto isFocusRegion = getSamplingMode() == SamplingMode::FocusRegion;

        _highlightFocusedElementsAction.setEnabled(isFocusRegion);
        _restrictNumberOfElementsAction.setEnabled(isFocusRegion);
        _lazyUpdateIntervalAction.setEnabled(isFocusRegion);

        updateMaximumNumberOfElementsAction();

        switch (getSamplingMode()) {
        	case SamplingMode::FocusRegion:
			{
                _viewingModeAction.setOptions({ "None", "Windowed", "Tooltip" });
                _sampleContextDirty = true;
                break;
        	}

            case SamplingMode::Selection:
            {
                _viewingModeAction.setOptions({ "None", "Windowed" });
                break;
            }
        }
	};

    samplingModeChanged();

    connect(&_samplingModeAction, &OptionAction::currentIndexChanged, this, samplingModeChanged);

    const auto viewingModeChanged = [this]() -> void {
        if (!_isInitialized)
            return;

        drawToolTip();

        switch (getViewingMode())
        {
            case ViewingMode::None:
            case ViewingMode::Tooltip:
            {
                if (_sampleScopePlugin && _sampleScopePlugin->getVisibleAction().isChecked())
                    _sampleScopePlugin->getVisibleAction().setChecked(false);

                break;
            }

            case ViewingMode::Windowed:
            {
                //if (!mv::projects().isOpeningProject())
                //    openSampleWindow();

                break;
            }
        }

        emit canViewChanged(canView());
    };

    viewingModeChanged();

    connect(&_viewingModeAction, &OptionAction::currentIndexChanged, this, viewingModeChanged);

    updateReadOnly();
}

void ViewPluginSamplerAction::initialize(plugin::ViewPlugin* viewPlugin, PixelSelectionAction* pixelSelectionAction, PixelSelectionAction* samplerPixelSelectionAction)
{
    try
    {
        if (_isInitialized)
            throw std::runtime_error("Sampler is already initialized");

        Q_ASSERT(viewPlugin && pixelSelectionAction && samplerPixelSelectionAction);

        if (!viewPlugin || !pixelSelectionAction || !samplerPixelSelectionAction)
            return;

        _viewPlugin                     = viewPlugin;
        _pixelSelectionAction           = pixelSelectionAction;
        _samplerPixelSelectionAction    = samplerPixelSelectionAction;
        _toolTipOverlayWidget           = std::make_unique<OverlayWidget>(&_viewPlugin->getWidget());

        _toolTipOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

        _viewPlugin->getWidget().installEventFilter(this);

        connect(_samplerPixelSelectionAction->getPixelSelectionTool(), &PixelSelectionTool::areaChanged, this, [this]() {
            _sampleContextDirty = true;
        });

        connect(_samplerPixelSelectionAction->getPixelSelectionTool(), &PixelSelectionTool::ended, this, [this]() {
            _sampleContextDirty = true;
        });

        _toolTipLabel.setParent(_toolTipOverlayWidget.get());
        _toolTipLabel.raise();
        _toolTipLabel.setWindowFlag(Qt::WindowStaysOnTopHint);
        _toolTipLabel.setAutoFillBackground(true);
        _toolTipLabel.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        _toolTipLabel.setWordWrap(getViewingMode() == ViewingMode::Tooltip);

        _sampleContextLazyUpdateTimer.start();

        _viewPlugin->getWidget().addAction(&_openSampleScopeWindow);

        _openSampleScopeWindow.setShortcut(QKeySequence(Qt::Key_F4));

        connect(&_openSampleScopeWindow, &TriggerAction::triggered, this, &ViewPluginSamplerAction::openSampleWindow);

        _isInitialized = true;

        drawToolTip();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize view plugin sampler action", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize view plugin sampler action");
    }
}

ViewPluginSamplerAction::ViewingMode ViewPluginSamplerAction::getViewingMode() const
{
    return static_cast<ViewingMode>(_viewingModeAction.getCurrentIndex());
}

void ViewPluginSamplerAction::setViewingMode(const ViewingMode& viewingMode)
{
    _viewingModeAction.setCurrentIndex(static_cast<std::int32_t>(viewingMode));
}

ViewPluginSamplerAction::SamplingMode ViewPluginSamplerAction::getSamplingMode() const
{
    return static_cast<SamplingMode>(_samplingModeAction.getCurrentIndex());
}

void ViewPluginSamplerAction::setSamplingMode(const SamplingMode& samplingMode)
{
    _samplingModeAction.setCurrentIndex(static_cast<std::int32_t>(samplingMode));
}

bool ViewPluginSamplerAction::canView() const
{
	switch (_viewGeneratorType) {
	    case ViewGeneratorType::HTML:
	        return _enabledAction.isChecked() && _htmlViewGeneratorFunction && getViewingMode() != ViewingMode::None && !_sampleContext.isEmpty();

	    case ViewGeneratorType::Widget:
	        return _enabledAction.isChecked() && _widgetViewGeneratorFunction && getViewingMode() != ViewingMode::None && !_sampleContext.isEmpty();
	}

    return false;
}

ViewPluginSamplerAction::ViewGeneratorType ViewPluginSamplerAction::getViewGeneratorType() const
{
    return _viewGeneratorType;
}

void ViewPluginSamplerAction::setViewGeneratorType(const ViewGeneratorType& viewGeneratorType)
{
    if (viewGeneratorType == _viewGeneratorType)
        return;

    const auto previousGeneratedViewType = _viewGeneratorType;

    _viewGeneratorType = viewGeneratorType;

    emit viewGeneratorTypeChanged(previousGeneratedViewType, _viewGeneratorType);
}

void ViewPluginSamplerAction::setHtmlViewGeneratorFunction(const HtmlViewGeneratorFunction& htmlViewGeneratorFunction)
{
    _htmlViewGeneratorFunction = htmlViewGeneratorFunction;

    setViewGeneratorType(ViewGeneratorType::HTML);
    setViewingMode(_htmlViewGeneratorFunction ? ViewingMode::Windowed : ViewingMode::None);
    updateReadOnly();

    emit canViewChanged(canView());
}

void ViewPluginSamplerAction::setWidgetViewGeneratorFunction(const WidgetViewGeneratorFunction& widgetViewGeneratorFunction)
{
    _widgetViewGeneratorFunction = widgetViewGeneratorFunction;

    setViewGeneratorType(ViewGeneratorType::Widget);
    setViewingMode(_widgetViewGeneratorFunction ? ViewingMode::Windowed : ViewingMode::None);
    updateReadOnly();

    emit canViewChanged(canView());
}

QVariantMap ViewPluginSamplerAction::getSampleContext() const
{
    return _sampleContext;
}

void ViewPluginSamplerAction::setSampleContext(const SampleContext& sampleContext)
{
    if (sampleContext == _sampleContext)
        return;

    const auto previousSampleContext = _sampleContext;

    _sampleContext      = sampleContext;
    _sampleContextDirty = true;

    if (_enabledAction.isChecked()) {
	    switch (_viewGeneratorType) {
	        case ViewGeneratorType::HTML:
	        {
	            if (_htmlViewGeneratorFunction)
	                setViewString(_htmlViewGeneratorFunction(_sampleContext));

	            break;
	        }

            case ViewGeneratorType::Widget:
			{
                if (_widgetViewGeneratorFunction)
                    setViewWidget(_widgetViewGeneratorFunction(_sampleContext));

                break;
		    }
	    }

        emit canViewChanged(canView());
    }

    emit sampleContextChanged(previousSampleContext, _sampleContext);
}

QString ViewPluginSamplerAction::getViewString() const
{
    return _viewString;
}

const QWidget* ViewPluginSamplerAction::getViewWidget() const
{
    return _viewWidget;
}

void ViewPluginSamplerAction::setViewString(const QString& viewString)
{
    if (viewString == _viewString)
        return;

    const auto previousViewString = _viewString;

    _viewString = viewString;

    drawToolTip();

    emit viewStringChanged(previousViewString, _viewString);
}

void ViewPluginSamplerAction::setViewWidget(QWidget* widget)
{
    if (widget == _viewWidget)
        return;

    auto previousWidget = _viewWidget;

    _viewWidget = widget;

    emit viewWidgetChanged(previousWidget, _viewWidget);
}

void ViewPluginSamplerAction::drawToolTip()
{
    Q_ASSERT(_viewPlugin);

    if (!_viewPlugin)
        return;

    switch (getViewingMode())
    {
        case ViewingMode::None:
            _toolTipLabel.setText("");
            break;

        case ViewingMode::Tooltip:
            _toolTipLabel.setText(_viewString);
            break;

        case ViewingMode::Windowed:
            _toolTipLabel.setText(_sampleScopePlugin && _sampleScopePlugin->getVisibleAction().isChecked() ? "" : "<i>Press <b>F4</b> for sample information</i>");
            break;
    }

    _toolTipLabel.setVisible(getEnabledAction().isChecked() && !_viewString.isEmpty() && canView() && getSamplingMode() == SamplingMode::FocusRegion);
    _toolTipLabel.adjustSize();

    moveToolTipLabel();
}

void ViewPluginSamplerAction::moveToolTipLabel()
{
    auto parentWidget   = &_viewPlugin->getWidget();
    auto targetWidget   = _pixelSelectionAction->getTargetWidget();
    auto globalPosition = _viewPlugin->getWidget().mapFromGlobal(QCursor::pos()) + QPoint(12, 12);

    if (globalPosition.x() + _toolTipLabel.width() > targetWidget->width())
        globalPosition.setX(parentWidget->width() - _toolTipLabel.width());

    if (globalPosition.x() < 0)
        globalPosition.setX(0);

    if (globalPosition.y() + _toolTipLabel.height() > targetWidget->height())
        globalPosition.setY(parentWidget->height() - (_toolTipLabel.height() + targetWidget->y()));

    if (globalPosition.y() < 0)
        globalPosition.setY(0);

    _toolTipLabel.move(globalPosition);
}

bool ViewPluginSamplerAction::eventFilter(QObject* target, QEvent* event)
{
    Q_ASSERT(_viewPlugin && _pixelSelectionAction && _samplerPixelSelectionAction);

    if (!_enabledAction.isChecked() || !_viewPlugin || !_pixelSelectionAction || !_samplerPixelSelectionAction || !_samplerPixelSelectionAction->isEnabled())
        return HorizontalGroupAction::eventFilter(target, event);

    if (target == &_viewPlugin->getWidget()) {
        switch (event->type())
        {
            case QEvent::MouseMove:
            {
                _sampleContextDirty = true;
                moveToolTipLabel();
                break;
            }

            case QEvent::MouseButtonPress:
            {
				_samplerPixelSelectionAction->getPixelSelectionTool()->setEnabled(false);
                _toolTipLabel.hide();
                break;
            }

            case QEvent::MouseButtonRelease:
            case QEvent::Enter:
            {
                const auto enabled = getSamplingMode() == SamplingMode::FocusRegion && getEnabledAction().isChecked() && canView();

                _samplerPixelSelectionAction->getPixelSelectionTool()->setEnabled(enabled);
                _toolTipLabel.setVisible(enabled);

                break;
            }

            case QEvent::Leave:
            {
                _samplerPixelSelectionAction->getPixelSelectionTool()->setEnabled(false);
                _toolTipLabel.hide();
                break;
            }

        default:
            break;
        }
    }

    return HorizontalGroupAction::eventFilter(target, event);
}

void ViewPluginSamplerAction::updateReadOnly()
{
	switch (_viewGeneratorType) {
	    case ViewGeneratorType::HTML:
	    {
	        setEnabled(_htmlViewGeneratorFunction ? true : false);
	        break;
	    }

        case ViewGeneratorType::Widget:
        {
            setEnabled(_widgetViewGeneratorFunction ? true : false);
            break;
        }
	}
}

QWidget* ViewPluginSamplerAction::getTargetWidget() const
{
    if (!_samplerPixelSelectionAction)
        return nullptr;

    return _samplerPixelSelectionAction->getTargetWidget();
}

void ViewPluginSamplerAction::openSampleWindow()
{
    drawToolTip();

    if (getViewingMode() != ViewingMode::Windowed)
        return;

    if (_sampleScopePlugin) {
        _sampleScopePlugin->getVisibleAction().setChecked(true);
    }
    else {
        _sampleScopePlugin = mv::plugins().requestViewPluginFloated("Sample scope");

        if (auto sourcePluginPickerAction = dynamic_cast<PluginPickerAction*>(_sampleScopePlugin->findChildByPath("Source plugin"))) {
            sourcePluginPickerAction->setCurrentPlugin(_viewPlugin);
            sourcePluginPickerAction->setEnabled(false);
        }
    }
}

void ViewPluginSamplerAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _highlightFocusedElementsAction.fromParentVariantMap(variantMap);
    _restrictNumberOfElementsAction.fromParentVariantMap(variantMap);
    _maximumNumberOfElementsAction.fromParentVariantMap(variantMap);
    _lazyUpdateIntervalAction.fromParentVariantMap(variantMap);
    _samplingModeAction.fromParentVariantMap(variantMap);
    _viewingModeAction.fromParentVariantMap(variantMap);
}

QVariantMap ViewPluginSamplerAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _highlightFocusedElementsAction.insertIntoVariantMap(variantMap);
    _restrictNumberOfElementsAction.insertIntoVariantMap(variantMap);
    _maximumNumberOfElementsAction.insertIntoVariantMap(variantMap);
    _lazyUpdateIntervalAction.insertIntoVariantMap(variantMap);
    _samplingModeAction.insertIntoVariantMap(variantMap);
    _viewingModeAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
}
