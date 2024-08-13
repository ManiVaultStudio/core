// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "DecimalAction.h"

#include "widgets/OverlayWidget.h"

#include <QLabel>

namespace mv::plugin {
    class ViewPlugin;
}

namespace mv::gui {

/**
 * View plugin tooltip action class
 *
 * For displaying mouse-aware content in a tooltip on a view plugin
 *
 * By default the action is disabled (enabled after initialization)
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ViewPluginToolTipAction : public VerticalGroupAction
{
    Q_OBJECT

public:

    /** Tooltip generator function which is called periodically when the mouse moves in the view (returns an HTML formatted string) */
    using TooltipGeneratorFunction = std::function<QString(plugin::ViewPlugin*)>;

    /**
     * Construct with pointer to \p parent object and title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ViewPluginToolTipAction(QObject* parent, const QString& title);

    /**
     * Initializes the action and enables tooltip display
     * @param viewPlugin Pointer to view plugin (may not be nullptr)
     * @param tooltipGeneratorFunction Function for generating the tooltip
     */
    void initialize(plugin::ViewPlugin* viewPlugin, const TooltipGeneratorFunction& tooltipGeneratorFunction);

    /** Request an update of the current tool tip */
    void requestUpdate();

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

public: // Action getters

    DecimalAction& getRoiSizeAction() { return _roiSizeAction; }

signals:

    /**
     * Signals that the tooltip HTML string changed from \p previousToolTipHtmlString to \p currentToolTipHtmlString
     * @param previousToolTipHtmlString Previous tooltip HTML string
     * @param currentToolTipHtmlString Current tooltip HTML string
     */
    void toolTipHtmlStringChanged(const QString& previousToolTipHtmlString, const QString& currentToolTipHtmlString);

private:
    plugin::ViewPlugin*             _viewPlugin;                    /** Pointer to view plugin for which to show the tooltips */
    TooltipGeneratorFunction        _tooltipGeneratorFunction;      /** Tooltip generator function which is called periodically when the mouse moves in the view (returns an HTML formatted string) */
    DecimalAction                   _roiSizeAction;                 /** Action to control the size of the ROI (in pixels) */
    QString                         _toolTipHtmlString;             /** HTML tooltip string */
    std::unique_ptr<OverlayWidget>  _toolTipOverlayWidget;          /** Overlay widget for the tooltip */
    QLabel                          _toolTipLabel;                  /** The text label which contains the actual tooltip text */
};

}

Q_DECLARE_METATYPE(mv::gui::ViewPluginToolTipAction)

inline const auto viewPluginToolTipActionMetaTypeId = qRegisterMetaType<mv::gui::ViewPluginToolTipAction*>("mv::gui::ViewPluginToolTipAction");
