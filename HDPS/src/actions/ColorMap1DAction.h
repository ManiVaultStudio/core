#pragma once

#include "ColorMapAction.h"

namespace hdps::gui {

/**
 * Color map 1D action class
 *
 * One-dimensional color map action
 *
 * @author Thomas Kroes
 */
class ColorMap1DAction : public ColorMapAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorMap Current color map
     */
    Q_INVOKABLE ColorMap1DAction(QObject* parent, const QString& title, const QString& colorMap = "RdYlBu");
};

}

Q_DECLARE_METATYPE(hdps::gui::ColorMap1DAction)

inline const auto colorMap1DActionMetaTypeId = qRegisterMetaType<hdps::gui::ColorMap1DAction*>("hdps::gui::ColorMap1DAction");
