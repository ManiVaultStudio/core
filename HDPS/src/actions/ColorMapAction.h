#pragma once

#include "WidgetAction.h"
#include "OptionAction.h"
#include "ColorMapSettingsAction.h"
#include "util/ColorMap.h"
#include "util/ColorMapModel.h"
#include "util/ColorMapFilterModel.h"

namespace hdps {

namespace gui {

/**
 * Color map action class
 *
 * Color map selection and settings action
 *
 * @author Thomas Kroes
 */
class ColorMapAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations (a color map combobox always exists) */
    enum WidgetFlag {
        Settings        = 0x00001,      /** Widgets have a settings popup to adjust range and other settings */
        EditRange       = 0x00002,      /** Users are allowed to change the color map range */
        ResetPushButton = 0x00004,      /** There is a button to reset the color map */

        Basic   = Settings | EditRange,
        All     = Settings | EditRange | ResetPushButton
    };

public:

    /** Combobox widget class for display of a color map */
    class ComboBoxWidget : public OptionAction::ComboBoxWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param optionAction Pointer to option action
         * @param colorMapAction Pointer to color map action
         */
        ComboBoxWidget(QWidget* parent, OptionAction* optionAction, ColorMapAction* colorMapAction);

        /**
         * Paint event to override default paint
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent) override;

    protected:
        ColorMapAction*   _colorMapAction;      /** Pointer to color map action */

        friend class ColorMapAction;
    };

protected:

    /**
     * Get widget representation of the color map action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param colorMapType Type of color map (1D/2D)
     * @param colorMap Current color map
     * @param defaultColorMap Default color map
     */
    ColorMapAction(QObject* parent, const QString& title = "", const util::ColorMap::Type& colorMapType = util::ColorMap::Type::OneDimensional, const QString& colorMap = "RdYlBu", const QString& defaultColorMap = "RdYlBu");

    /**
     * Initialize the color map action
     * @param colorMap Current color map
     * @param defaultColorMap Default color map
     */
    void initialize(const QString& colorMap = "", const QString& defaultColorMap = "");

    /** Determines whether the current color can be reset to its default */
    bool isResettable() const override;

    /** Reset the current color to the default color */
    void reset() override;

    /** Gets the current color map type */
    util::ColorMap::Type getColorMapType() const;

    /** Sets the current color map type */
    void setColorMapType(const util::ColorMap::Type& colorMapType);

public: // Option action wrappers

    /** Gets the current color map name */
    QString getColorMap() const;

    /** Gets the current color map */
    QImage getColorMapImage() const;

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

public: // Action getters

    OptionAction& getCurrentColorMapAction() { return _currentColorMapAction; }
    ColorMapSettingsAction& getSettingsAction() { return _settingsAction; }

signals:

    /**
     * Signals that the current color map type changed
     * @param colorMapType Current color map type
     */
    void typeChanged(const util::ColorMap::Type& colorMapType);

    /**
     * Signals that the current color map image changed
     * @param image Current color map image
     */
    void imageChanged(const QImage& image);

protected:
    OptionAction                _currentColorMapAction;     /** Current color map selection action */
    util::ColorMapFilterModel   _colorMapFilterModel;       /** The filtered color map model (contains either 1D or 2D color maps) */
    ColorMapSettingsAction      _settingsAction;            /** Color map settings action */
};

}
}
