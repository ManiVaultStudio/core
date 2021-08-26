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
class ColorMapAction : public OptionAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorMap Current color map
     * @param defaultColorMap Default color map
     */
    ColorMapAction(QObject* parent, const QString& title = "", const QString& colorMap = "", const QString& defaultColorMap = "");

    /**
     * Initialize the color map action
     * @param colorMap Current color map
     * @param defaultColorMap Default color map
     */
    void initialize(const QString& colorMap = "", const QString& defaultColorMap = "");

public: // Option action wrappers

    /** Gets the current color map name */
    QString getColorMap() const;

    /**
     * Sets the current color map name
     * @param colorMap Name of the current color map
     */
    void setColorMap(const QString& colorMap);

    /** Gets the default color map name */
    QString getDefaultColorMap() const;

    /**
     * Sets the default color map name
     * @param defaultColorMap Name of the default color map
     */
    void setDefaultColorMap(const QString& defaultColorMap);

signals:

    /**
     * Signals that the current color map name changed
     * @param colorMap Current color map name
     */
    void colorMapChanged(const QString& colorMap);

    /**
     * Signals that the default color map name changed
     * @param defaultColorMap Default color map name
     */
    void defaultColorMapChanged(const QString& defaultColorMap);
};

}
}
