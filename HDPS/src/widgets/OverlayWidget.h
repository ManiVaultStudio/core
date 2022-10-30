#pragma once

#include <QWidget>
#include <QGraphicsOpacityEffect>
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
    OverlayWidget(QWidget* parent, const QIcon& icon, const QString& title, const QString& description = "");

    /**
     * Set overlay parameters
     * @param icon Icon
     * @param title Title of the overlay
     * @param description Overlay description
     */
    void set(const QIcon& icon, const QString& title, const QString& description = "");

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Setups the layout etc. */
    void initialize();

private:
    QGraphicsOpacityEffect*     _opacityEffect;         /** Effect for modulating opacity */
    QLabel                      _iconLabel;             /** Label for displaying the icon */
    QLabel                      _titleLabel;            /** Label for displaying the title */
    QLabel                      _descriptionLabel;      /** Label for displaying the description */
};

}
}
