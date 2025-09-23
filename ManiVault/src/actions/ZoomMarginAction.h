// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <actions/VerticalGroupAction.h>
#include <actions/OptionAction.h>
#include <actions/DecimalAction.h>

#include <QObject>

namespace mv::gui
{

/**
 * Zoom margin action class
 *
 * Provides interface for setting the zoom margins
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ZoomMarginAction : public VerticalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ZoomMarginAction(QObject* parent, const QString& title);

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

    OptionAction& getZoomMarginTypeAction() { return _marginTypeAction; }
    DecimalAction& getZoomMarginScreenAction() { return _zoomMarginScreenAction; }
    DecimalAction& getZoomMarginDataAction() { return _zoomMarginDataAction; }

private:
    OptionAction    _marginTypeAction;          /** For setting the zoom margin type */
    DecimalAction   _zoomMarginScreenAction;    /** Zoom margin relative to widget (in screen pixels) */
    DecimalAction   _zoomMarginDataAction;      /** Zoom margin relative to data bounds (in percentages) */

    static const QStringList marginTypeOptions; /** Options for the margin type action */
};

}
