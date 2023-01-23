#pragma once

#include <QWidget>
#include <QLabel>

namespace hdps::gui
{

/**
 * Info widget class
 *
 * Standard widget which displays an icon, title and description (used for instance as an overlay for other widgets).
 *  
 * @author Thomas Kroes
 */
class InfoWidget : public QWidget
{
public:

    /**
     * Construct info widget with \p parent
     * @param parent Pointer to parent widget
     */
    InfoWidget(QWidget* parent);

    /**
     * Construct info widget with \parent, \p icon, \p title, \p description, \p backgroundColor and \p textColor
     * @param parent Pointer to parent widget
     * @param icon Icon
     * @param title Title of the overlay
     * @param description Overlay description
     * @param foregroundColor Foreground color
     * @param backgroundColor Background color
     */
    InfoWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description = "", const QColor foregroundColor = Qt::black, const QColor backgroundColor = Qt::lightGray);
    
    /**
     * Set overlay parameters
     * @param icon Icon
     * @param title Title of the overlay
     * @param description Overlay description
     */
    void set(const QIcon& icon, const QString& title, const QString& description = "");

    /**
     * Set color
     * @param color Color of the widget
     */
    void setColor(const QColor color);

    /**
     * Set overlay colors
     * @param foregroundColor Foreground color
     * @param backgroundColor Background color
     */
    void setColors(const QColor foregroundColor = Qt::black, const QColor backgroundColor = Qt::lightGray);

    /**
     * Set foreground color to \p foregroundColor
     * @param foregroundColor Foreground color
     */
    void setForegroundColor(const QColor foregroundColor = Qt::black);

    /**
     * Set background color to \p backgroundColor
     * @param backgroundColor Background color
     */
    void setBackgroundColor(const QColor backgroundColor = Qt::lightGray);

private:

    /** Initializes the controls */
    void initialize();

private:
    QIcon       _icon;                  /** Icon */
    QLabel      _iconLabel;             /** Label for displaying the icon */
    QLabel      _titleLabel;            /** Label for displaying the title */
    QLabel      _descriptionLabel;      /** Label for displaying the description */
    QColor      _foregroundColor;       /** Foreground color of the widget */
    QColor      _backgroundColor;       /** Background color of the widget */
};

}
