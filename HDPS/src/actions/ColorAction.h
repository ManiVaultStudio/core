#pragma once

#include "WidgetAction.h"
#include "ColorPickerAction.h"

#include <QHBoxLayout>
#include <QToolButton>

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

    /** Describes the widget flags */
    enum WidgetFlag {
        Picker          = 0x00001,      /** The widget includes a color picker */
        ResetPushButton = 0x00002,      /** The widget includes a reset push button */

        Basic   = Picker,
        All     = Picker | ResetPushButton
    };

public:

    /** Color picker push button class for color action */
    class PushButtonWidget : public WidgetActionWidget
    {
    protected:

        /** Extended tool button widget class with display of current color */
        class ToolButton : public QToolButton
        {
        public:

            /**
             * Constructor
             * @param parent Parent widget
             * @param colorPickerAction Reference to color picker action
             */
            ToolButton(QWidget* parent, ColorPickerAction& colorPickerAction);

            /**
             * Paint event to override default paint
             * @param paintEvent Pointer to paint event
             */
            void paintEvent(QPaintEvent* paintEvent) override;

        protected:
            ColorPickerAction&  _colorPickerAction;     /** Reference to color picker action */
        };

    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorAction Pointer to color action
         */
        PushButtonWidget(QWidget* parent, ColorAction* colorAction);

    public: // Action getters

        ColorPickerAction& getColorPickerAction() { return _colorPickerAction; }

    protected:
        QHBoxLayout         _layout;                /** Widget layout */
        ColorPickerAction   _colorPickerAction;     /** Color picker action */
        ToolButton          _toolButton;            /** Tool button for the popup */

        friend class ColorAction;
    };

protected:

    /**
     * Get widget representation of the color action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override;;

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
};

}
}
