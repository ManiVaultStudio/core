#pragma once

#include "StartPageContentWidget.h"

#include <widgets/OverlayWidget.h>

#include <QGridLayout>
#include <QLabel>

/**
 * Start page widget class
 *
 * Widget class which show various project-related operations.
 *
 * @author Thomas Kroes
 */
class StartPageWidget : public hdps::gui::OverlayWidget
{
public:

    /**
     * Header widget class
     *
     * Widget class for header with logo
     *
     * @author Thomas Kroes
     */
    class HeaderWidget : public QWidget
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         */
        HeaderWidget(QWidget* parent = nullptr);

    protected:
        QVBoxLayout     _layout;        /** Main layout */
        QLabel          _headerLabel;   /** Header label */
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     */
    StartPageWidget(QWidget* parent = nullptr);

    void paintEvent(QPaintEvent* paintEvent);

    /**
     * Set the widget background color role
     * @param widget Pointer to widget to apply the background color to
     * @param colorRole Background color role
     */
    static void setWidgetBackgroundColorRole(QWidget* widget, const QPalette::ColorRole& colorRole);

protected:
    QHBoxLayout             _layout;                    /** Main layout */
    QVBoxLayout             _centerColumnLayout;        /** Layout for the center column */
    StartPageContentWidget  _startPageContentWidget;    /** Widget which contains all the actions */
    QPixmap                 _backgroundImage;           /** Background image */
};
