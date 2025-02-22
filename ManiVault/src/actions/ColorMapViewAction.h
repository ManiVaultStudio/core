// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

namespace mv::gui {

class ColorMapAction;

/**
 * Color map view action class
 *
 * Action class for color map display in a widget
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ColorMapViewAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for color map view action */
    class CORE_EXPORT Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapViewAction Pointer to color map view action
         */
        Widget(QWidget* parent, ColorMapViewAction* colorMapViewAction);
    };

    /**
     * Get widget representation of the color map view action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

protected:

    /**
     * Constructor
     * @param colorMapAction Reference to color map action
     */
    Q_INVOKABLE ColorMapViewAction(ColorMapAction& colorMapAction);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }

protected:
    ColorMapAction&     _colorMapAction;    /** Reference to color map action */

    /** Only color map settings action classes may instantiate this class */
    friend class ColorMapSettings1DAction;
    friend class ColorMapSettings2DAction;
};

}

Q_DECLARE_METATYPE(mv::gui::ColorMapViewAction)

inline const auto colorMapViewActionMetaTypeId = qRegisterMetaType<mv::gui::ColorMapViewAction*>("ColorMapViewAction");
