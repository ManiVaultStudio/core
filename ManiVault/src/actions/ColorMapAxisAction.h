// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "DecimalRangeAction.h"
#include "ToggleAction.h"
#include "TriggerAction.h"

namespace mv::gui {

class ColorMapSettingsAction;

/**
 * Color map axis action class
 *
 * Action class for configuring the color map along an axis (horizontal/vertical)
 *
 * @author Thomas Kroes
 */
class ColorMapAxisAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map axis action */
    class Widget : public mv::gui::WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapAxisAction Pointer to color map axis action
         */
        Widget(QWidget* parent, ColorMapAxisAction* colorMapAxisAction);
    };

    /**
     * Get widget representation of the color map axis action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

protected:

    /**
     * Constructor
     * @param colorMapSettingsAction Reference to color map settings action
     * @param title Axis title
     */
    Q_INVOKABLE ColorMapAxisAction(ColorMapSettingsAction& colorMapSettingsAction, const QString& title);

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    DecimalRangeAction& getRangeAction() { return _rangeAction; }
    ToggleAction& getMirrorAction() { return _mirrorAction; }

protected:
    DecimalRangeAction  _rangeAction;       /** Range action */
    ToggleAction        _mirrorAction;      /** Mirror along the axis action */

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}

Q_DECLARE_METATYPE(mv::gui::ColorMapAxisAction)

inline const auto colorMapAxisActionMetaTypeId = qRegisterMetaType<mv::gui::ColorMapAxisAction*>("ColorMapAxisAction");
