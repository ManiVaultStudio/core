#pragma once

#include "ColorMapAction.h"

namespace hdps::gui {

/**
 * Color map 2D action class
 *
 * Two-dimensional color map action
 *
 * @author Thomas Kroes
 */
class ColorMap2DAction : public ColorMapAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorMap Current color map
     */
    Q_INVOKABLE ColorMap2DAction(QObject* parent, const QString& title = "", const QString& colorMap = "example_a");
};

}

Q_DECLARE_METATYPE(hdps::gui::ColorMap2DAction)

inline const auto colorMap2DActionMetaTypeId = qRegisterMetaType<hdps::gui::ColorMap2DAction*>("hdps::gui::ColorMap2DAction");