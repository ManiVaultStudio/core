// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "VerticalGroupAction.h"
#include "ToggleAction.h"
#include "DecimalAction.h"
#include "IntegralAction.h"
#include "PixelSelectionAction.h"

#include "widgets/OverlayWidget.h"

#include <QVariantMap>
#include <QLabel>
#include <QTimer>

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui {

/**
 * View plugin sampler action class
 *
 * Provides sampling support in view plugins
 *
 * In order to achieve both normal element selection and sampling, this class
 * uses two pointers to pixel selection actions and switches between them
 * conditionally:
 * - Normal pixel selection action
 * - Sampling pixel selection action (enabled when selection is idle)
 *
 * In addition, this class can display a tooltip generated elsewhere with a
 * tooltip generator function, see ViewPluginSamplerAction::setTooltipGeneratorFunction(...)
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ViewPluginSamplerAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /** Context with which the tooltip is created */
    using SampleContext = QVariantMap;

    /** Tooltip generator function which is called periodically when the mouse moves in the view (returns an HTML formatted string) */
    using ToolTipGeneratorFunction = std::function<QString(const SampleContext&)>;

    /**
     * Construct with pointer to \p parent object and title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ViewPluginSamplerAction(QObject* parent, const QString& title);

    /**
     * Initializes the action and enables tooltip display
     * @param viewPlugin Pointer to view plugin (may not be nullptr)
     * @param pixelSelectionAction Pointer to pixel selection action to use (may not be nullptr)
     * @param samplerPixelSelectionAction Pointer to sampler pixel selection action to use (may not be nullptr)
     */
    void initialize(plugin::ViewPlugin* viewPlugin, PixelSelectionAction* pixelSelectionAction, PixelSelectionAction* samplerPixelSelectionAction);

    /**
     * Sets the tooltip generator function to \p toolTipGeneratorFunction
     * @param toolTipGeneratorFunction Tooltip generator function
     */
    void setTooltipGeneratorFunction(const ToolTipGeneratorFunction& toolTipGeneratorFunction);

    /**
     * Get sample context
     * @return Sample variant map
     */
    SampleContext getSampleContext() const;

    /**
     * Set the sample context and flag request an update
     * @param sampleContext Sample context
     */
    void setSampleContext(const SampleContext& sampleContext);

    /**
     * Get tooltip HTML string
     * @return HTML formatted string
     */
    QString getToolTipHtmlString() const;

private:

    /**
     * Set tooltip HTML string to \p toolTipHtmlString
     * @param toolTipHtmlString HTML formatted string
     */
    void setToolTipHtmlString(const QString& toolTipHtmlString);

    /** Draws the tooltip near the cursor */
    void drawToolTip();

    /** Moves the tooltip label to the cursor */
    void moveToolTipLabel();

    /**
     * Respond to \p target object events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

public: // Serialization

    /**
     * Load view plugin from variant
     * @param Variant representation of the view plugin
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save view plugin to variant
     * @return Variant representation of the view plugin
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    ToggleAction& getEnabledAction() { return _enabledAction; }
    ToggleAction& getHighlightFocusedElementsAction() { return _highlightFocusedElementsAction; }
    ToggleAction& getRestrictNumberOfElementsAction() { return _restrictNumberOfElementsAction; }
    IntegralAction& getMaximumNumberOfElementsAction() { return _maximumNumberOfElementsAction; }
    IntegralAction& getToolTipLazyUpdateIntervalAction() { return _sampleContextLazyUpdateIntervalAction; }

signals:

    /**
     * Signals that the tooltip HTML string changed from \p previousToolTipHtmlString to \p currentToolTipHtmlString
     * @param previousToolTipHtmlString Previous tooltip HTML string
     * @param currentToolTipHtmlString Current tooltip HTML string
     */
    void toolTipHtmlStringChanged(const QString& previousToolTipHtmlString, const QString& currentToolTipHtmlString);

    /** Signals that a new sample context is required */
    void sampleContextRequested();

private:
    plugin::ViewPlugin*             _viewPlugin;                                /** Pointer to view plugin for which to show the tooltips */
    bool                            _isInitialized;                             /** Boolean determining whether the sampler is initialized or not */
    PixelSelectionAction*           _pixelSelectionAction;                      /** Pointer to pixel selection tool to use */
    PixelSelectionAction*           _samplerPixelSelectionAction;               /** Pointer to sampler pixel selection tool to use */
    ToolTipGeneratorFunction        _toolTipGeneratorFunction;                  /** Tooltip generator function which is called periodically when the mouse moves in the view (returns an HTML formatted string) */
    ToggleAction                    _enabledAction;                             /** Action to toggle computation on/off */
    ToggleAction                    _highlightFocusedElementsAction;            /** Action to toggle focus elements highlighting */
    VerticalGroupAction             _settingsAction;                            /** Additional vertical group action for settings */
    ToggleAction                    _restrictNumberOfElementsAction;            /** Action to toggle the restriction of the maximum number of elements in the focus region */
    IntegralAction                  _maximumNumberOfElementsAction;             /** Action to restrict the maximum number of elements in the focus region */
    IntegralAction                  _sampleContextLazyUpdateIntervalAction;     /** Action to control the size of the tooltip lazy update timer interval */
    SampleContext                   _sampleContext;                             /** Context for the tooltip */
    QTimer                          _sampleContextLazyUpdateTimer;              /** Lazily (periodically) updates the sample context tooltip string */
    bool                            _sampleContextDirty;                        /** Indicates that the sample context is dirty */
    QString                         _toolTipHtmlString;                         /** HTML tooltip string */
    std::unique_ptr<OverlayWidget>  _toolTipOverlayWidget;                      /** Overlay widget for the tooltip */
    QLabel                          _toolTipLabel;                              /** The text label which contains the actual tooltip text */
    
};

}

Q_DECLARE_METATYPE(mv::gui::ViewPluginSamplerAction)

inline const auto viewPluginSamplerActionMetaTypeId = qRegisterMetaType<mv::gui::ViewPluginSamplerAction*>("mv::gui::ViewPluginSamplerAction");
