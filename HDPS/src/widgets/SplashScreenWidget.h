// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFrame>

namespace hdps::gui {

class SplashScreenAction;

/**
 * Splash screen dialog base class
 *
 * Dialog class for displaying a splash screen
 * 
 * The dialog has not content by default, use SplashScreenDialog::setWidget() to assign the splash screen content
 *
 * @author Thomas Kroes
 */
class SplashScreenWidget : public QWidget
{
public:

    /**
     * Construct with \p parent widget and default configuration
     * @param parent Pointer to parent widget
     * @param autoHideTimeout Splash screen auto-hide trigger duration (active when duration > 0)
     * @param panAmount The amount of panning in the y-direction during show and hide
     * @param animationDuration The duration of the animations in milliseconds
     */
    SplashScreenWidget(SplashScreenAction& splashScreenAction, QWidget* parent = nullptr);

    /**
     * Invoked when the dialog is painted
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent);

    void showAnimated();
    void closeAnimated();

private:

    void createHeader();
    void createBody();
    void createFooter();

    bool shouldDisplayProjectInfo() const;

    static QString getCopyrightTooltip();

private:
    SplashScreenAction& _splashScreenAction;
    QPixmap             _logoImage;             /** Logo image */
    QPixmap             _backgroundImage;       /** Background image */
    QFrame              _roundedFrame;
    QVBoxLayout         _frameLayout;
    QToolButton         _closeToolButton;       /** Button for forcefully closing the splash screen */

    static const std::uint32_t fixedWidth           = 640;
    static const std::uint32_t fixedHeight          = 480;
    static const std::uint32_t margin               = 40;
    static const std::uint32_t logoSize             = 150;
    static const std::uint32_t footerImagesHeight   = 40;
    static const std::uint32_t frameRadius          = 7;
    static const std::uint32_t animationDuration    = 300;
    static const std::uint32_t panAmount            = 15;
};

}
