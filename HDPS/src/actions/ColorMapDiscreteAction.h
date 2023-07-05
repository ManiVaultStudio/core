// SPDX-License-Identifier: LGPL-3.0-or-later 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"
#include "IntegralAction.h"
#include "ToggleAction.h"

namespace hdps::gui {

class ColorMapAction;

/**
 * Color map discrete action class
 *
 * Action class for discrete color map settings
 *
 * @author Thomas Kroes
 */
class ColorMapDiscreteAction final : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map discrete action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapDiscreteAction Pointer to color map discrete action
         */
        Widget(QWidget* parent, ColorMapDiscreteAction* colorMapDiscreteAction);
    };

    /**
     * Get widget representation of the color map discrete action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

protected:

    /**
     * Constructor
     * @param colorMapAction Reference to owning color map action
     */
<<<<<<< HEAD
    Q_INVOKABLE ColorMapDiscreteAction(ColorMapSettingsAction& colorMapSettingsAction);
=======
    ColorMapDiscreteAction(ColorMapAction& colorMapAction);
>>>>>>> origin/master

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

    IntegralAction& getNumberOfStepsAction() { return _numberOfStepsAction; }
    ToggleAction& getDiscretizeAlphaAction() { return _discretizeAlphaAction; }

protected:
    

    /** Only color map settings action may instantiate this class */
    friend class ColorMapAction;
};

}

Q_DECLARE_METATYPE(hdps::gui::ColorMapDiscreteAction)

inline const auto colorMapDiscreteActionMetaTypeId = qRegisterMetaType<hdps::gui::ColorMapDiscreteAction*>("ColorMapDiscreteAction");
