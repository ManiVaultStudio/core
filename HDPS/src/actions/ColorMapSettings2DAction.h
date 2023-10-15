// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ColorMapViewAction.h"

namespace mv::gui {

class ColorMapAction;

/**
 * Color map settings two-dimensional action class
 *
 * Widget action class for two-dimensional color map settings
 *
 * @author Thomas Kroes
 */
class ColorMapSettings2DAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for two-dimensional color map settings action */
    class Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapSettings2DAction Pointer to two-dimensional color map settings action
         */
        Widget(QWidget* parent, ColorMapSettings2DAction* colorMapSettings2DAction);

    protected:
        ColorMapViewAction  _colorMapViewAction;    /** Color map view action */
    };

    /**
     * Get widget representation of the two-dimensional color map settings action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

protected:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE ColorMapSettings2DAction(QObject* parent, const QString& title);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }

protected:
    ColorMapAction&     _colorMapAction;        /** Reference to color map action */

    /** Only color map action may instantiate this class */
    friend class ColorMapAction;
};

}

Q_DECLARE_METATYPE(mv::gui::ColorMapSettings2DAction)

inline const auto colorMapSettings2DActionMetaTypeId = qRegisterMetaType<mv::gui::ColorMapSettings2DAction*>("mv::gui::ColorMapSettings2DAction");
