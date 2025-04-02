// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/HorizontalToolbarAction.h>
#include <actions/TriggerAction.h>
#include <actions/DecimalAction.h>
#include <actions/HorizontalGroupAction.h>
#include <actions/DecimalRectangleAction.h>
#include <actions/DecimalPointAction.h>

#include <QObject>

namespace mv::gui
{

/**
 * Navigation action class
 *
 * Provides actions for navigating in a renderer (the business logic
 * should be handled elsewhere depending on the renderer).
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT NavigationAction : public HorizontalGroupAction
{
	Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE NavigationAction(QObject* parent, const QString& title);

	/**
     * Set whether shortcuts are enabled
     * @param shortcutsEnabled Boolean determining whether shortcuts are enabled
	 */
	void setShortcutsEnabled(bool shortcutsEnabled);

public: // Serialization

    /**
     * Load navigation action from variant map
     * @param variantMap Variant map representation of the navigation action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */

    QVariantMap toVariantMap() const override;

public: // Action getters

    TriggerAction& getZoomOutAction() { return _zoomOutAction; }
    DecimalAction& getZoomPercentageAction() { return _zoomPercentageAction; }
    TriggerAction& getZoomInAction() { return _zoomInAction; }
    TriggerAction& getZoomExtentsAction() { return _zoomExtentsAction; }
    TriggerAction& getZoomSelectionAction() { return _zoomSelectionAction; }
    TriggerAction& getZoomRegionAction() { return _zoomRegionAction; }
    DecimalRectangleAction& getZoomRectangleAction() { return _zoomRectangleAction; }
    DecimalPointAction& getZoomCenterAction() { return _zoomCenterAction; }
    DecimalAction& getZoomFactorAction() { return _zoomFactorAction; }

private:
    TriggerAction           _zoomOutAction;             /** Zoom out action */
    DecimalAction           _zoomPercentageAction;      /** Zoom action */
    TriggerAction           _zoomInAction;              /** Zoom in action */
    TriggerAction           _zoomExtentsAction;         /** Zoom extents action */
    TriggerAction           _zoomSelectionAction;       /** Zoom to selection extents action */
    TriggerAction           _zoomRegionAction;          /** Zoom to region action */
    DecimalRectangleAction  _zoomRectangleAction;       /** Rectangle action for setting the current zoom bounds */
    DecimalPointAction      _zoomCenterAction;          /** Zoom center action */
    DecimalAction           _zoomFactorAction;          /** Zoom factor action */
};

}
