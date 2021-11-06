#pragma once

#include "actions/WidgetAction.h"
#include "actions/IntegralAction.h"

#include <QVBoxLayout>
#include <QColorDialog>

namespace hdps {

namespace gui {

/**
 * Color picker action class
 *
 * Note: for internal use by the color action only
 *
 * @author Thomas Kroes
 */
class ColorPickerAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Color picker widget class for color picker action */
    class Widget : public WidgetActionWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorPickerAction Pointer to color picker action
         */
        Widget(QWidget* parent, ColorPickerAction* colorPickerAction);

    protected:
        QVBoxLayout         _layout;                    /** Main layout */
        QColorDialog        _colorDialog;               /** Non-visible color dialog from which the color picker control is stolen */
        IntegralAction      _hueAction;                 /** Hue action */
        IntegralAction      _saturationAction;          /** Saturation action */
        IntegralAction      _lightnessAction;           /** Lightness action */
        bool                _updateColorPickerAction;   /** Whether the color picker action should be updated */

        friend class ColorPickerAction;
    };

protected:

    /**
     * Get widget representation of the color picker action
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
     * @param color Initial color
     */
    ColorPickerAction(QObject* parent, const QString& title = "", const QColor& color = DEFAULT_COLOR, const QColor& defaultColor = DEFAULT_COLOR);

public:

    /**
     * Initialize the color action
     * @param color Initial color
     */
    void initialize(const QColor& color = DEFAULT_COLOR, const QColor& defaultColor = DEFAULT_COLOR);

    /** Gets the current color */
    QColor getColor() const;

    /**
     * Sets the current color
     * @param color Current color
     */
    void setColor(const QColor& color);

    /** Gets the default color */
    QColor getDefaultColor() const;

    /** Sets the default color */
    void setDefaultColor(const QColor& defaultColor);

    /** Determines whether the current color can be reset to its default */
    bool isResettable() const override;

    /** Reset the current color to the default color */
    void reset() override;

signals:

    /**
     * Signals that the current color changed
     * @param color Current color that changed
     */
    void colorChanged(const QColor& color);

    /**
     * Signals that the default color changed
     * @param defaultColor Default color that changed
     */
    void defaultColorChanged(const QColor& defaultColor);

protected:
    QColor  _color;             /** Current color */
    QColor  _defaultColor;      /** Default color */

    /** Default default color */
    static const QColor DEFAULT_COLOR;

    friend class ColorAction;
};

}
}
