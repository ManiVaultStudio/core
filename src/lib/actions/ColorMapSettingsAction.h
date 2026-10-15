// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

namespace mv::gui {

class ColorMapAction;

/**
 * Color map settings action class
 *
 * Group action for color map settings
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ColorMapSettingsAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map settings action */
    class CORE_EXPORT Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapSettingsAction Pointer to clusters action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction, const std::int32_t& widgetFlags);
    };

    /**
     * Get widget representation of the color map settings action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    };

protected:

    /**
     * Constructor
     * @param colorMapAction Reference to color map action
     * @param title Title of the group action
     */
    Q_INVOKABLE ColorMapSettingsAction(ColorMapAction& colorMapAction, const QString& title);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }

protected:
    ColorMapAction&     _colorMapAction;        /** Reference to owning color map action */

    /** Only color map may instantiate this class */
    friend class ColorMapAction;
};

}

Q_DECLARE_METATYPE(mv::gui::ColorMapSettingsAction)

inline const auto colorMapSettingsActionMetaTypeId = qRegisterMetaType<mv::gui::ColorMapSettingsAction*>("mv::gui::ColorMapSettingsAction");
