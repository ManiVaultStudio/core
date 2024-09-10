// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginSamplerAction.h"

#include "ViewPlugin.h"

#include "util/Exception.h"

#include <QEvent>

#include "PluginPickerAction.h"

using namespace mv::util;

namespace mv::gui {

ViewPluginSamplerAction::ViewPluginSamplerAction(QObject* parent, const QString& title, const ViewingMode& viewingMode /*= ViewingMode::None*/) :
    HorizontalGroupAction(parent, title),
    _viewPlugin(nullptr),
    _viewingMode(ViewingMode::None),
    _isInitialized(false),
    _pixelSelectionAction(nullptr),
    _samplerPixelSelectionAction(nullptr),
    _enabledAction(this, "Enabled", true),
    _highlightFocusedElementsAction(this, "Highlight focused elements", true),
    _settingsAction(this, "Settings"),
    _restrictNumberOfElementsAction(this, "Restrict number of elements", false),
    _maximumNumberOfElementsAction(this, "Max. number of elements", 0, 1000, 100),
    _lazyUpdateIntervalAction(this, "Lazy update interval", 10, 1000, 100),
    _openSampleScopeWindow(this, "Open sample scope window"),
    _sampleScopePlugin(nullptr)
{
    setShowLabels(false);

    addAction(&_enabledAction);
    addAction(&_settingsAction);

    _settingsAction.addAction(&_highlightFocusedElementsAction);
    _settingsAction.addAction(&_restrictNumberOfElementsAction);
    _settingsAction.addAction(&_maximumNumberOfElementsAction);
    _settingsAction.addAction(&_lazyUpdateIntervalAction);

    _enabledAction.setStretch(1);

    _settingsAction.setIconByName("cog");
    _settingsAction.setConfigurationFlag(ConfigurationFlag::ForceCollapsedInGroup);
    _settingsAction.setPopupSizeHint(QSize(500, 0));

    _enabledAction.setToolTip("Toggle focus region visibility");
    _highlightFocusedElementsAction.setToolTip("Toggle highlighting of focused elements");
    _settingsAction.setToolTip("Additional focus region settings");
    _maximumNumberOfElementsAction.setToolTip("Puts a cap on the amount of points captured by the focus region");
    _openSampleScopeWindow.setToolTip("Open a sample scope window to inspect the samples");

    const auto updateSettingsActionReadOnly = [this]() -> void {
        _settingsAction.setEnabled(_enabledAction.isChecked());

        if (_samplerPixelSelectionAction)
            _samplerPixelSelectionAction->getPixelSelectionTool()->setEnabled(_enabledAction.isChecked());
    };

    updateSettingsActionReadOnly();

    connect(&_enabledAction, &ToggleAction::toggled, this, updateSettingsActionReadOnly);

    const auto updateMaximumNumberOfElementsAction = [this]() -> void {
        _maximumNumberOfElementsAction.setEnabled(_restrictNumberOfElementsAction.isChecked());
    };

    updateMaximumNumberOfElementsAction();

    connect(&_restrictNumberOfElementsAction, &ToggleAction::toggled, this, updateMaximumNumberOfElementsAction);

    const auto updateSampleContextLazyUpdateTimerInterval = [this]() -> void {
        _sampleContextLazyUpdateTimer.setInterval(_lazyUpdateIntervalAction.getValue());
    };

    updateSampleContextLazyUpdateTimerInterval();

    connect(&_lazyUpdateIntervalAction, &IntegralAction::valueChanged, this, updateSampleContextLazyUpdateTimerInterval);

    connect(&_sampleContextLazyUpdateTimer, &QTimer::timeout, this, [this]() -> void {
        if (!_sampleContextDirty || !_viewGeneratorFunction || getViewingMode() == ViewingMode::None)
            return;

        emit sampleContextRequested();

        _sampleContextDirty = false;
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

        if (getTargetWidget())
            getTargetWidget()->removeEventFilter(this);

        _viewPlugin                     = viewPlugin;
        _pixelSelectionAction           = pixelSelectionAction;
        _samplerPixelSelectionAction    = samplerPixelSelectionAction;
        _toolTipOverlayWidget           = std::make_unique<OverlayWidget>(&_viewPlugin->getWidget());

        _toolTipOverlayWidget->setAttribute(Qt::WA_TransparentForMouseEvents);

        getTargetWidget()->installEventFilter(this);

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
        //_toolTipLabel.setWordWrap(true);

        _sampleContextLazyUpdateTimer.start();

        _viewPlugin->getWidget().addAction(&_openSampleScopeWindow);

        _openSampleScopeWindow.setShortcut(QKeySequence(Qt::Key_F4));

        connect(&_openSampleScopeWindow, &TriggerAction::triggered, this, [this]() -> void {
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
        });

        _isInitialized = true;
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
    return _viewingMode;
}

void ViewPluginSamplerAction::setViewingMode(const ViewingMode& viewingMode)
{
    if (viewingMode == _viewingMode)
        return;

    const auto previousViewingMode = _viewingMode;

    _viewingMode = viewingMode;

    emit viewingModeChanged(previousViewingMode, _viewingMode);
}

bool ViewPluginSamplerAction::canView() const
{
    return _viewGeneratorFunction && _viewingMode != ViewingMode::None;
}

void ViewPluginSamplerAction::setViewGeneratorFunction(const ViewGeneratorFunction& viewGeneratorFunction)
{
    _viewGeneratorFunction = viewGeneratorFunction;

    setViewingMode(viewGeneratorFunction ? ViewingMode::Windowed : ViewingMode::None);
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
        setViewString(_viewGeneratorFunction(_sampleContext));
}

QString ViewPluginSamplerAction::getViewString() const
{
    return _viewString;
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

    _toolTipLabel.setVisible(!_viewString.isEmpty());
    _toolTipLabel.adjustSize();

    moveToolTipLabel();
}

void ViewPluginSamplerAction::moveToolTipLabel()
{
    auto parentWidget   = &_viewPlugin->getWidget();
    auto targetWidget   = _pixelSelectionAction->getTargetWidget();
    auto globalPosition = _viewPlugin->getWidget().mapFromGlobal(parentWidget->cursor().pos());

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

            case QEvent::Enter:
            {

                _samplerPixelSelectionAction->getPixelSelectionTool()->setEnabled(getEnabledAction().isChecked());
                break;
            }

            case QEvent::Leave:
            {
                _samplerPixelSelectionAction->getPixelSelectionTool()->setEnabled(false);
                break;
            }

        default:
            break;
        }
    }

    return HorizontalGroupAction::eventFilter(target, event);
}

QWidget* ViewPluginSamplerAction::getTargetWidget() const
{
    if (!_samplerPixelSelectionAction)
        return nullptr;

    return _samplerPixelSelectionAction->getTargetWidget();
}

void ViewPluginSamplerAction::fromVariantMap(const QVariantMap& variantMap)
{
    HorizontalGroupAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _highlightFocusedElementsAction.fromParentVariantMap(variantMap);
    _restrictNumberOfElementsAction.fromParentVariantMap(variantMap);
    _maximumNumberOfElementsAction.fromParentVariantMap(variantMap);
    _lazyUpdateIntervalAction.fromParentVariantMap(variantMap);
}

QVariantMap ViewPluginSamplerAction::toVariantMap() const
{
    auto variantMap = HorizontalGroupAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _highlightFocusedElementsAction.insertIntoVariantMap(variantMap);
    _restrictNumberOfElementsAction.insertIntoVariantMap(variantMap);
    _maximumNumberOfElementsAction.insertIntoVariantMap(variantMap);
    _lazyUpdateIntervalAction.insertIntoVariantMap(variantMap);

    return variantMap;
}
}
