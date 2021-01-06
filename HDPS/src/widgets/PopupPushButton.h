#pragma once

#include <QPushButton>

namespace hdps
{

namespace gui
{

/**
 * Popup push button class
 *
 * Extended push button class that shows an aligned popup widget when clicked.
 *
 * @author Thomas Kroes
 */
class PopupPushButton : public QPushButton
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     */
    PopupPushButton(QWidget* parent);

public: // Getters/setters

    /** Get/set popup widget */
    QWidget* getPopupWidget();
    void setPopupWidget(QWidget* popupWidget);

    /** Get/set popup widget alignment */
    Qt::AlignmentFlag getPopupWidgetAlignment() const;
    void setPopupWidgetAlignment(const Qt::AlignmentFlag& alignment);

private:
    QWidget*            _popupWidget;       /** Popup widget to show when the push button is clicked */
    Qt::AlignmentFlag   _alignment;         /** Determines how the popup widget will be aligned, valid options: Left, Center, Right */
};

}
}