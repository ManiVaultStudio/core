#pragma once

#include <QPushButton>

class QColorDialog;

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

public:

	/**
	 * Constructor
	 * @param parent Pointer to parent widget
	 */
	ColorPickerPushButton(QWidget* parent);

public: // Getters/setters

	/** Returns the currently selected color */
	QColor getColor() const;

	/**
	 * Sets the current color
	 * @param color Color
	 */
	void setColor(const QColor& color);

    /** Returns whether the RGB color code is shown */
    bool getShowText() const;

    /**
     * Sets whether the RGB color code is shown
     * @param showText Show color text
     */
    void setShowText(const bool& showText);

signals:

	/** Signals that the current color has changed
	 * @param currentColor Currently selected color
	 */
	void colorChanged(const QColor& color);

private:
	QColorDialog*	_colorDialog;		/** Qt color picker dialog */
    bool            _showText;          /** Whether to show RGB color code */
	
	static const QSize _iconSize;
};