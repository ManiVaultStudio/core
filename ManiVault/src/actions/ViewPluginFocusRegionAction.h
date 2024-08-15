// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "VerticalGroupAction.h"
#include "ToggleAction.h"
#include "DecimalAction.h"
#include "IntegralAction.h"

#include "widgets/OverlayWidget.h"

#include <QVariantMap>
#include <QLabel>
#include <QTimer>

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui {

/**
 * View plugin focus region summary action class
 *
 * For displaying a summary tooltip for a focus region in a target view plugin
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ViewPluginFocusRegionAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /** Context with which the tooltip is created */
    using ToolTipContext = QVariantMap;

    /** Tooltip generator function which is called periodically when the mouse moves in the view (returns an HTML formatted string) */
    using ToolTipGeneratorFunction = std::function<QString(const ToolTipContext&)>;

    /**
     * Construct with pointer to \p parent object and title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ViewPluginFocusRegionAction(QObject* parent, const QString& title);

    /**
     * Initializes the action and enables tooltip display
     * @param viewPlugin Pointer to view plugin (may not be nullptr)
     * @param toolTipGeneratorFunction Function for generating the tooltip
     */
    void initialize(plugin::ViewPlugin* viewPlugin, const ToolTipGeneratorFunction& toolTipGeneratorFunction);

    /**
     * Request an update of the current tool tip for \p
     * @param toolTipContext Context for the tooltip
     */
    void requestUpdate(const QVariantMap& toolTipContext);

    /**
     * Get tooltip context
     * @return Context variant map
     */
    QVariantMap getToolTipContext() const;

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
    DecimalAction& getSizeAction() { return _sizeAction; }

signals:

    /**
     * Signals that the tooltip HTML string changed from \p previousToolTipHtmlString to \p currentToolTipHtmlString
     * @param previousToolTipHtmlString Previous tooltip HTML string
     * @param currentToolTipHtmlString Current tooltip HTML string
     */
    void toolTipHtmlStringChanged(const QString& previousToolTipHtmlString, const QString& currentToolTipHtmlString);

private:
    plugin::ViewPlugin*             _viewPlugin;                    /** Pointer to view plugin for which to show the tooltips */
    ToolTipGeneratorFunction        _toolTipGeneratorFunction;      /** Tooltip generator function which is called periodically when the mouse moves in the view (returns an HTML formatted string) */
    ToggleAction                    _enabledAction;                 /** Action to toggle computation on/off */
    VerticalGroupAction             _settingsAction;                /** Additional vertical group action for settings */
    DecimalAction                   _sizeAction;                    /** Action to control the size of the focus region (in pixels) */
    IntegralAction                  _maximumNumberOfPointsAction;   /** Action to control the maximum number of points in the focus region */
    QVariantMap                     _toolTipContext;                /** Context for the tooltip */
    QString                         _toolTipHtmlString;             /** HTML tooltip string */
    std::unique_ptr<OverlayWidget>  _toolTipOverlayWidget;          /** Overlay widget for the tooltip */
    QLabel                          _toolTipLabel;                  /** The text label which contains the actual tooltip text */
    QTimer                          _updateTimer;                   /** Periodically updates the HTML tooltip string */
    bool                            _toolTipDirty;                  /** Indicates that the HTML tooltip string needs to be re-established */
};

}

Q_DECLARE_METATYPE(mv::gui::ViewPluginFocusRegionAction)

inline const auto viewPluginFocusRegionActionMetaTypeId = qRegisterMetaType<mv::gui::ViewPluginFocusRegionAction*>("mv::gui::ViewPluginFocusRegionAction");
