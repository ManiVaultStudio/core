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
 * tooltip generator function, see ViewPluginSamplerAction::setViewGeneratorFunction(...)
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ViewPluginSamplerAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /** Viewing modes */
    enum class ViewingMode {
        None,       /** Do not show shamples */
        Tooltip,    /** Force tooltip view (regardless of the size of the tooltip) */
        Windowed    /** Force sample scope view plugin based view */
    };

    /** Context with which the tooltip is created */
    using SampleContext = QVariantMap;

    /** View generator function which is called periodically when the mouse moves in the view (should return an HTML formatted string) */
    using ViewGeneratorFunction = std::function<QString(const SampleContext&)>;

    /**
     * Construct with pointer to \p parent object and title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param viewingMode Viewing mode
     */
    Q_INVOKABLE ViewPluginSamplerAction(QObject* parent, const QString& title, const ViewingMode& viewingMode = ViewingMode::None);

    /**
     * Initializes the action and enables tooltip display
     * @param viewPlugin Pointer to view plugin (may not be nullptr)
     * @param pixelSelectionAction Pointer to pixel selection action to use (may not be nullptr)
     * @param samplerPixelSelectionAction Pointer to sampler pixel selection action to use (may not be nullptr)
     */
    void initialize(plugin::ViewPlugin* viewPlugin, PixelSelectionAction* pixelSelectionAction, PixelSelectionAction* samplerPixelSelectionAction);

    /**
     * Get viewing mode
     * @return Viewing mode
     */
    ViewingMode getViewingMode() const;

    /**
     * Set viewing mode to \p viewingMode
     * @param viewingMode Viewing mode
     */
    void setViewingMode(const ViewingMode& viewingMode);

    /**
     * Whether this sampler can be viewed, two criteria must be met:
     * - ViewPluginSamplerAction#_viewingMode must be either Mode::Tooltip or Mode::Windowed
     * - ViewPluginSamplerAction#_viewGeneratorFunction must be valid
     * @return 
     */
    bool canView() const;

    /**
     * Sets the view generator function to \p viewGeneratorFunction
     * @param viewGeneratorFunction View generator function
     */
    void setViewGeneratorFunction(const ViewGeneratorFunction& viewGeneratorFunction);

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
     * Get view string
     * @return HTML formatted string
     */
    QString getViewString() const;

private:

    /**
     * Set view string to \p viewString
     * @param viewString HTML formatted string
     */
    void setViewString(const QString& viewString);

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

    /** Updates the action read-only status based on if it can view sample information */
    void updateReadOnly();

    /**
     * Get pointer to target widget
     * @return Pointer to target widget (maybe nullptr)
     */
    QWidget* getTargetWidget() const;

    /** Creates a sample scope plugin instance and shows it on the screen */
    void openSampleWindow();

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
    IntegralAction& getLazyUpdateIntervalAction() { return _lazyUpdateIntervalAction; }
    OptionAction& getViewingModeAction() { return _viewingModeAction; }

signals:

    /**
     * Signals that the viewing mode changed from \p previousViewingMode to \p currentViewingMode
     * @param previousViewingMode Previous viewing mode
     * @param currentViewingMode Current viewing mode
     */
    void viewingModeChanged(const ViewingMode& previousViewingMode, const ViewingMode& currentViewingMode);

    /**
     * Signals that the view HTML string changed from \p previousViewString to \p currentViewString
     * @param previousViewString Previous view HTML string
     * @param currentViewString Current view HTML string
     */
    void viewStringChanged(const QString& previousToolTipHtmlString, const QString& currentToolTipHtmlString);

    /** Signals that a new sample context is required */
    void sampleContextRequested();

private:
    plugin::ViewPlugin*             _viewPlugin;                                /** Pointer to view plugin for which to show the tooltips */
    bool                            _isInitialized;                             /** Boolean determining whether the sampler is initialized or not */
    PixelSelectionAction*           _pixelSelectionAction;                      /** Pointer to pixel selection tool to use */
    PixelSelectionAction*           _samplerPixelSelectionAction;               /** Pointer to sampler pixel selection tool to use */
    ViewGeneratorFunction           _viewGeneratorFunction;                     /** View generator function which is called periodically when the mouse moves in the view (should return an HTML formatted string) */
    ToggleAction                    _enabledAction;                             /** Action to toggle computation on/off */
    ToggleAction                    _highlightFocusedElementsAction;            /** Action to toggle focus elements highlighting */
    VerticalGroupAction             _settingsAction;                            /** Additional vertical group action for settings */
    ToggleAction                    _restrictNumberOfElementsAction;            /** Action to toggle the restriction of the maximum number of elements in the focus region */
    IntegralAction                  _maximumNumberOfElementsAction;             /** Action to restrict the maximum number of elements in the focus region */
    IntegralAction                  _lazyUpdateIntervalAction;                  /** Action to control the view update timer interval */
    OptionAction                    _viewingModeAction;                         /** Action to control the viewing mode */
    SampleContext                   _sampleContext;                             /** Context for the tooltip */
    QTimer                          _sampleContextLazyUpdateTimer;              /** Lazily (periodically) updates the sample context tooltip string */
    bool                            _sampleContextDirty;                        /** Indicates that the sample context is dirty */
    QString                         _viewString;                                /** HTML-formatted view string */
    std::unique_ptr<OverlayWidget>  _toolTipOverlayWidget;                      /** Overlay widget for the tooltip */
    QLabel                          _toolTipLabel;                              /** The text label which contains the actual tooltip text */
    TriggerAction                   _openSampleScopeWindow;                     /** Opens a sample scope window */
    plugin::ViewPlugin*             _sampleScopePlugin;                         /** Pointer to sample scope plugin (maybe nullptr) */
};

}

Q_DECLARE_METATYPE(mv::gui::ViewPluginSamplerAction)

inline const auto viewPluginSamplerActionMetaTypeId = qRegisterMetaType<mv::gui::ViewPluginSamplerAction*>("mv::gui::ViewPluginSamplerAction");
