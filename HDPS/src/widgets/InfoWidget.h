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
     */
    InfoWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description = "", const QColor backgroundColor = Qt::lightGray, const QColor textColor = Qt::black);
    
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
     * @param backgroundColor Background color of the widget
     * @param textColor Text color of the widget
     */
    void setColors(const QColor backgroundColor, const QColor textColor);

private:

    /** Initializes the controls */
    void initialize();

private:
    QLabel      _iconLabel;             /** Label for displaying the icon */
    QLabel      _titleLabel;            /** Label for displaying the title */
    QLabel      _descriptionLabel;      /** Label for displaying the description */
    QColor      _backgroundColor;       /** Background color of the widget */
    QColor      _textColor;             /** Text color of the widget */
};

}
