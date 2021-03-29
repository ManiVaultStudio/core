#pragma once

#include <QPushButton>
#include <QColorDialog>

/**
 * Color picker push button class
 *
 * A push button widget class for picking, storing and display a color
 *
 * @author Thomas Kroes
 */
class ColorPickerPushButton : public QPushButton
{
    Q_OBJECT

    /** Ways to display color string */
    enum class TextMode {
        NoText,         /** Show no color text information */
        RGB,            /** Show color in RGB format */
        Hexadecimal,    /** Show color in hexadecimal format */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    ColorPickerPushButton(QWidget* parent = nullptr);

public: // Getters/setters

    /** Gets/sets the current color */
    QColor getColor() const;
    void setColor(const QColor& color);

    /** Gets/sets the text mode */
    TextMode getTextMode() const;
    void setTextMode(const TextMode& textMode);

    /**
     * Override paint event for drawing color square and color text
     * @param paintEvent Paint event that occurred
     */
    void paintEvent(QPaintEvent* paintEvent) override;

signals:

    /** Signals that the current color has changed
     * @param color Currently selected color
     */
    void colorChanged(const QColor& color);

private:
    QColorDialog    _colorDialog;       /** Qt color picker dialog */
    QColor          _color;             /** Current color */
    TextMode        _textMode;          /** If, and how to the render the color as a string */
};