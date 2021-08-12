#pragma once

#include "WidgetAction.h"

class QWidget;
class QHBoxLayout;
class ColorPickerPushButton;
class QPushButton;

namespace hdps {

namespace gui {

/**
 * Color widget action class
 *
 * Stores a color and creates widgets to interact with it
 *
 * @author Thomas Kroes
 */
class ColorAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Color color picker push button class for color action
     */
    class ColorPickerPushButtonWidget : public WidgetActionWidget {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorAction Pointer to color action
         */
        ColorPickerPushButtonWidget(QWidget* parent, ColorAction* colorAction);

    public:

        /** Return widget layout */
        QHBoxLayout* getLayout() { return _layout; }

        /** Return color picker push button */
        ColorPickerPushButton* getColorPickerPushButton() { return _colorPickerPushButton; }

    protected:
        QHBoxLayout*            _layout;                    /** Widget layout */
        ColorPickerPushButton*  _colorPickerPushButton;     /** Color picker push button */

        friend class ColorAction;
    };

protected:

    /**
     * Get widget representation of the color action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override {
        return new ColorAction::ColorPickerPushButtonWidget(parent, this);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param color Initial color
     * @param defaultColor Default color
     */
    ColorAction(QObject* parent, const QString& title = "", const QColor& color = DEFAULT_COLOR, const QColor& defaultColor = DEFAULT_COLOR);

    /**
     * Initialize the color action
     * @param color Initial color
     * @param defaultColor Default color
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
    bool canReset() const;

    /** Reset the current color to the default color */
    void reset();

    /*
    ColorAction& operator= (const ColorAction& other)
    {
        WidgetAction::operator=(other);

        _color          = other._color;
        _defaultColor   = other._defaultColor;

        return *this;
    }
    */

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
};

}
}
