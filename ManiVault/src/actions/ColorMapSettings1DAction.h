// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

namespace mv::gui {

class ColorMapAction;

/**
 * Color map settings one-dimensional action class
 *
 * Widget action class for one-dimensional color map settings
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ColorMapSettings1DAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Widget class for one-dimensional color map settings action */
    class CORE_EXPORT Widget : public WidgetActionWidget {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapSettings1DAction Pointer to one-dimensional color map settings action
         */
        Widget(QWidget* parent, ColorMapSettings1DAction* colorMapSettings1DAction);
    };

    /**
     * Get widget representation of the one-dimensional color map settings action
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
    Q_INVOKABLE ColorMapSettings1DAction(QObject* parent, const QString& title);

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }

protected:
    ColorMapAction&     _colorMapAction;    /** Reference to color map action */

    /** Only color map action may instantiate this class */
    friend class ColorMapAction;
};

}

Q_DECLARE_METATYPE(mv::gui::ColorMapSettings1DAction)

inline const auto colorMapSettings1DActionMetaTypeId = qRegisterMetaType<mv::gui::ColorMapSettings1DAction*>("mv::gui::ColorMapSettings1DAction");
