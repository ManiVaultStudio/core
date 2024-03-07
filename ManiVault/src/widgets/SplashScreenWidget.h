// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QTimer>

namespace mv::gui {

class SplashScreenAction;

/**
 * Splash screen widget class
 *
 * Widget class for displaying a splash screen
 * 
 * This widget can either serve as:
 *  - an application splash screen widget (shown when no project is loaded at startup)
 *  - a project splash screen widget
 *    This only works when: a) the project is loaded from the command line i.e. HDPS.exe -p c:\project.mv
 *    b) the associated splash screen is enabled and c) SplashScreenAction#_projectMetaAction is valid.
 *
 * @author Thomas Kroes
 */
class SplashScreenWidget : public QWidget
{
public:

    /** Widget class for adding an external link on the splash screen widget */
    class ExternalLinkWidget : public QWidget {
    public:

        /**
         * Construct with \p fontAwesomeIconName, descriptive \p text, \p externalLink and pointer to \p parent widget
         * @param fontAwesomeIconName Name of the Font Awesome icon
         * @param text Descriptive text
         * @param externalLink URL of the external website
         * @param parent Pointer to parent widget
         */
        ExternalLinkWidget(const QString& fontAwesomeIconName, const QString& text, const QUrl& externalLink, QWidget* parent = nullptr);

        /**
         * Invoked when the mouse enters the widget
         * @param event Pointer to event that occurred
         */
        void enterEvent(QEnterEvent* event) override;

        /**
         * Invoked when the mouse leaves the widget
         * @param event Pointer to event that occurred
         */
        void leaveEvent(QEvent* event) override;
        
    private:
        
        /** Updates the CSS style */
        void updateStyle();

    private:
        const QUrl&     _externalLink;      /** URL of the website to visit when the widget is pressed */
    };

public:

    /**
     * Construct with reference to owning \p splashScreenAction and optional pointer to \p parent widget
     * @param splashScreenAction Reference to owning splashScreenAction
     * @param parent Pointer to parent widget
     */
    SplashScreenWidget(SplashScreenAction& splashScreenAction, QWidget* parent = nullptr);

    /** Destructor */
    ~SplashScreenWidget();

    /**
     * Invoked when the dialog is painted (used to draw the background)
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent);

    /** Show the splash screen widget animated, wraps QWidget::show() */
    void showAnimated();

    /** Animate the splash screen widget and close it, wraps QWidget::close() */
    void closeAnimated();

private: // Splash screen content

    /** Create toolbar with close button (if appropriate)*/
    void createToolbar();

    /** Create two-column body content */
    void createBody();

    /** Create footer content with some images and when necessary a progress bar */
    void createFooter();

    /** Establishes whether project information should be shown or not */
    bool shouldDisplayProjectInfo() const;

    /** Get the ManiVault copyright notice tooltip string */
    static QString getCopyrightNoticeTooltip();

private:
    SplashScreenAction&         _splashScreenAction;    /** Reference to owning splashScreenAction */
    QPixmap                     _logoImage;             /** Logo image */
    QPixmap                     _backgroundImage;       /** Background image */
    QFrame                      _roundedFrame;          /** Frame with rounded edges */
    QVBoxLayout                 _roundedFrameLayout;    /** Layout for the rounded frame */
    QToolButton                 _closeToolButton;       /** Button for forcefully closing the splash screen */
    QGraphicsDropShadowEffect   _dropShadowEffect;      /** For adding a drop shadow to the splash screen widget */
    QTimer                      _processEventsTimer;    /** Timer to keep the splash screen widget somewhat responsive */

    static const std::uint32_t fixedWidth           = 640;      /** Widget fixed width */
    static const std::uint32_t fixedHeight          = 480;      /** Widget fixed height */
    static const std::uint32_t margin               = 40;       /** Widget layout margins */
    static const std::uint32_t logoSize             = 150;      /** Body content left-column logo maximum size */
    static const std::uint32_t footerImagesHeight   = 40;       /** Maximum size of the footer content images */
    static const std::uint32_t frameRadius          = 7;        /** CSS radius of SplashScreenWidget#_roundedFrame */
    static const std::uint32_t animationDuration    = 300;      /** Duration of the show and close animations */
    static const std::uint32_t panAmount            = 25;       /** The amount of up/down panning to do during the animations */
    static const std::uint32_t shadowMargin         = 20;       /** Extra margins allocated for drop shadow */
    static const std::uint32_t maxBlurRadius        = 20.0;     /** Maximum blur radius */
};

}
