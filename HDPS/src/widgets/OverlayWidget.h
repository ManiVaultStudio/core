#pragma once

#include "util/WidgetFader.h"

#include <QWidget>
#include <QLabel>

namespace hdps
{

namespace gui
{

/**
 * Overlay widget class
 *
 * Overlays the parent widget with a widget and synchronizes with the parent widget geometry.
 * It contains an icon, a title and a description.
 *  
 * @author Thomas Kroes
 */
class OverlayWidget : public QWidget
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    OverlayWidget(QWidget* parent);

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param icon Icon
     * @param title Title of the overlay
     * @param description Overlay description
     */
    OverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description = "", const QColor backgroundColor = Qt::lightGray, const QColor textColor = Qt::black);
    
    /**
     * Set overlay parameters
     * @param icon Icon
     * @param title Title of the overlay
     * @param description Overlay description
     */
    void set(const QIcon& icon, const QString& title, const QString& description = "");

    /**
     * Set overlay colors
     * @param backgroundColor Background color of the widget
     * @param textColor Text color of the widget
     */
    void setColors(const QColor backgroundColor, const QColor textColor);

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /** Override the show member to fade the opacity over time */
    void show();

    /** Override the hide member to fade the opacity over time */
    void hide();

private:

    /** Setups the layout etc. */
    void initialize();

private:
    util::WidgetFader   _widgetFader;           /** Widget fader for the overlay widget */
    QLabel              _iconLabel;             /** Label for displaying the icon */
    QLabel              _titleLabel;            /** Label for displaying the title */
    QLabel              _descriptionLabel;      /** Label for displaying the description */
    QColor              _backgroundColor;       /** Background color of the widget */
    QColor              _textColor;             /** Text color of the widget */
};

}
}
