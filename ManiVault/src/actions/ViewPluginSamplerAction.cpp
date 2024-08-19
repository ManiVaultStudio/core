// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginSamplerAction.h"

using namespace mv::util;

namespace mv::gui {

ViewPluginSamplerAction::ViewPluginSamplerAction(QObject* parent, const QString& title) :
    HorizontalGroupAction(parent, title),
    _viewPlugin(nullptr),
    _isInitialized(false),
    _pixelSelectionAction(nullptr),
    _samplerPixelSelectionAction(nullptr),
    _enabledAction(this, "Enabled", true),
    _highlightFocusedElementsAction(this, "Highlight focused elements", true),
    _settingsAction(this, "Settings"),
    _restrictNumberOfElementsAction(this, "Restrict number of elements", true),
    _maximumNumberOfElementsAction(this, "Max. number of elements", 0, 1000, 100),
    _sampleContextLazyUpdateIntervalAction(this, "Tooltip lazy update interval", 50, 1000, 100)
{
    setShowLabels(false);

    addAction(&_enabledAction);
    addAction(&_settingsAction);

    _settingsAction.addAction(&_highlightFocusedElementsAction);
    _settingsAction.addAction(&_restrictNumberOfElementsAction);
    _settingsAction.addAction(&_maximumNumberOfElementsAction);
    _settingsAction.addAction(&_sampleContextLazyUpdateIntervalAction);

    _enabledAction.setStretch(1);

    _settingsAction.setIconByName("cog");
    _settingsAction.setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    _settingsAction.setPopupSizeHint(QSize(500, 0));

    _enabledAction.setToolTip("Toggle focus region visibility");
    _highlightFocusedElementsAction.setToolTip("Toggle highlighting of focused elements");
    _settingsAction.setToolTip("Additional focus region settings");
    _maximumNumberOfElementsAction.setToolTip("Puts a cap on the amount of points captured by the focus region");

    const auto updateSettingsActionReadOnly = [this]() -> void {
        _settingsAction.setEnabled(_enabledAction.isChecked());
    };

    updateSettingsActionReadOnly();

    connect(&_enabledAction, &ToggleAction::toggled, this, updateSettingsActionReadOnly);

    //_sampleContextLazyUpdateTimer.setSingleShot(true);

    const auto updateSampleContextLazyUpdateTimerInterval = [this]() -> void {
        _sampleContextLazyUpdateTimer.setInterval(_sampleContextLazyUpdateIntervalAction.getValue());
    };

    updateSampleContextLazyUpdateTimerInterval();

    connect(&_sampleContextLazyUpdateIntervalAction, &IntegralAction::valueChanged, this, updateSampleContextLazyUpdateTimerInterval);

    connect(&_sampleContextLazyUpdateTimer, &QTimer::timeout, this, [this]() -> void {
        if (!_sampleContextDirty || !_toolTipGeneratorFunction)
            return;

        emit sampleContextRequested();

        //_sampleContextDirty = false;
    });
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

        if (_viewPlugin)
            _viewPlugin->getWidget().removeEventFilter(this);

        _viewPlugin                     = viewPlugin;
        _pixelSelectionAction           = pixelSelectionAction;
        _samplerPixelSelectionAction    = samplerPixelSelectionAction;
        _toolTipOverlayWidget           = std::make_unique<OverlayWidget>(&_viewPlugin->getWidget());

        _viewPlugin->getWidget().setMouseTracking(true);
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
        _toolTipLabel.setWordWrap(true);

        _sampleContextLazyUpdateTimer.start();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to initialize view plugin sampler action", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to initialize view plugin sampler action");
    }
}

void ViewPluginSamplerAction::setTooltipGeneratorFunction(const ToolTipGeneratorFunction& toolTipGeneratorFunction)
{
    _toolTipGeneratorFunction = toolTipGeneratorFunction;
}

QVariantMap ViewPluginSamplerAction::getSampleContext() const
{
    return _sampleContext;
}

void ViewPluginSamplerAction::setSampleContext(const SampleContext& sampleContext)
{
    if (sampleContext == _sampleContext)
        return;

    _sampleContext      = sampleContext;
    _sampleContextDirty = true;

    if (_enabledAction.isChecked())
        setToolTipHtmlString(_toolTipGeneratorFunction(_sampleContext));
}

QString ViewPluginSamplerAction::getToolTipHtmlString() const
{
    return _toolTipHtmlString;
}

void ViewPluginSamplerAction::setToolTipHtmlString(const QString& toolTipHtmlString)
{
    if (toolTipHtmlString == _toolTipHtmlString)
        return;

    const auto previousToolTipHtmlString = _toolTipHtmlString;

    _toolTipHtmlString = toolTipHtmlString;

    _toolTipLabel.setVisible(!_toolTipHtmlString.isEmpty());
    _toolTipLabel.setText(_toolTipHtmlString);
    _toolTipLabel.adjustSize();

    drawToolTip();

    emit toolTipHtmlStringChanged(previousToolTipHtmlString, _toolTipHtmlString);
}

void ViewPluginSamplerAction::drawToolTip()
{
    //if (!isChecked())
    //    return;

    Q_ASSERT(_viewPlugin);

    if (!_viewPlugin)
        return;

    moveToolTipLabel();
}

void ViewPluginSamplerAction::moveToolTipLabel()
{
    _toolTipLabel.move(_viewPlugin->getWidget().mapFromGlobal(_viewPlugin->getWidget().cursor().pos() + QPoint(10, 10)));
}

bool ViewPluginSamplerAction::eventFilter(QObject* target, QEvent* event)
{
    Q_ASSERT(_viewPlugin && _pixelSelectionAction && _samplerPixelSelectionAction);

    if (!_viewPlugin || !_pixelSelectionAction || !_samplerPixelSelectionAction)
        return HorizontalGroupAction::eventFilter(target, event);

    if (target == &_viewPlugin->getWidget()) {
        auto& selectionTypeAction = _pixelSelectionAction->getTypeAction();

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
                break;
            }

            case QEvent::MouseButtonRelease:
            {
                _samplerPixelSelectionAction->getPixelSelectionTool()->setEnabled(getEnabledAction().isChecked());
                break;
            }

        default:
            break;
        }
    }

    return HorizontalGroupAction::eventFilter(target, event);
}

void ViewPluginSamplerAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _highlightFocusedElementsAction.fromParentVariantMap(variantMap);
    _restrictNumberOfElementsAction.fromParentVariantMap(variantMap);
    _maximumNumberOfElementsAction.fromParentVariantMap(variantMap);
    _sampleContextLazyUpdateIntervalAction.fromParentVariantMap(variantMap);
}

QVariantMap ViewPluginSamplerAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _highlightFocusedElementsAction.insertIntoVariantMap(variantMap);
    _restrictNumberOfElementsAction.insertIntoVariantMap(variantMap);
    _maximumNumberOfElementsAction.insertIntoVariantMap(variantMap);
    _sampleContextLazyUpdateIntervalAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
}
