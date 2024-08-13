// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "VerticalGroupAction.h"
#include "DecimalAction.h"

namespace mv::gui {

/**
 * View tooltip action class
 *
 * For displaying mouse-aware content in a tooltip on a view
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ViewTooltipAction : public VerticalGroupAction
{
public:

    /** Tooltip generator function which is called periodically when the mouse moves in the view (returns an HTML formatted string) */
    using TooltipGeneratorFunction = std::function<QString(Dataset<DatasetImpl>, Dataset<DatasetImpl>)>;

    /**
     * Construct with pointer to \p parent object and title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ViewTooltipAction(QObject* parent, const QString& title);

    /**
     * Initializes the action
     * @param viewWidget Pointer to view widget (may not be nullptr)
     * @param tooltipGeneratorFunction Function for generating the tooltip
     */
    void initialize(QWidget* viewWidget, const TooltipGeneratorFunction& tooltipGeneratorFunction);

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override final;

private:

    void updateTooltip();

public: // Action getters

    DecimalAction& getRoiSizeAction() { return _roiSizeAction; }

private:
    QWidget*                        _viewWidget;                    /** Pointer to view widget of which the mouse movements should be tracked */
    TooltipGeneratorFunction        _tooltipGeneratorFunction;      /** Tooltip generator function which is called periodically when the mouse moves in the view (returns an HTML formatted string) */
    DecimalAction                   _roiSizeAction;                 /** Action to control the size of the ROI (in pixels) */
};

}

Q_DECLARE_METATYPE(mv::gui::ViewTooltipAction)

inline const auto viewTooltipActionMetaTypeId = qRegisterMetaType<mv::gui::ViewTooltipAction*>("mv::gui::ViewTooltipAction");
