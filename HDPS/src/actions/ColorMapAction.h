#pragma once

#include "OptionAction.h"

namespace hdps {

namespace gui {

/**
 * Color map action class
 *
 * Extended option action for color map selection and management
 *
 * @author Thomas Kroes
 */
class ColorMapAction : protected OptionAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorMap Color map
     * @param defaultColorMap Default color map
     */
    ColorMapAction(QObject* parent, const QString& title = "", const QString& colorMap = "", const QString& defaultColorMap = "");

    /**
     * Initialize the color map action
     * @param colorMap Color map
     * @param defaultColorMap Default color map
     */
    void initialize(const QString& colorMap = "", const QString& defaultString = "");

signals:

protected:
    QString     _colorMap;              /** Current color map */
    QString     _defaultColorMap;       /** Default color map */
};

}
}
