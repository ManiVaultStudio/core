// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QDialog>
#include <QVBoxLayout>
#include <QToolButton>
#include <QPropertyAnimation>

namespace hdps::gui {

/**
 * Splash screen dialog base class
 *
 * Dialog class for displaying a splash screen
 * 
 * The dialog has not content by default, use SplashScreenDialog::setWidget() to assign the splash screen content
 *
 * @author Thomas Kroes
 */
class SplashScreenDialog : public QDialog
{
public:

    /** Animation is: */
    enum class AnimationState {
        Idle,           /** ...not doing anything */
        FadingIn,       /** ...fading in */
        FadingOut       /** ...fading out */
    };

public:

    /**
     * Construct with \p parent widget and default configuration
     * @param parent Pointer to parent widget
     * @param autoHideTimeout Splash screen auto-hide trigger duration (active when duration > 0)
     * @param panAmount The amount of panning in the y-direction during show and hide
     * @param animationDuration The duration of the animations in milliseconds
     */
    SplashScreenDialog(QWidget* parent, std::int32_t autoHideTimeout = DEFAULT_AUTO_HIDE_TIMEOUT, std::uint32_t panAmount = DEFAULT_PAN_AMOUNT, std::uint32_t animationDuration = DEFAULT_ANIMATION_DURATION);

    /**
     * Initializes the splash screen dialog with some configuration parameters
     * @param autoHideTimeout Splash screen auto-hide trigger duration (active when duration > 0)
     * @param panAmount The amount of panning in the y-direction during show and hide
     * @param animationDuration The duration of the animations in milliseconds
     */
    void initialize(std::int32_t autoHideTimeout = DEFAULT_AUTO_HIDE_TIMEOUT, std::uint32_t panAmount = DEFAULT_PAN_AMOUNT, std::uint32_t animationDuration = DEFAULT_ANIMATION_DURATION);

    /** Override open() for custom behavior */
    void open() override final;

    /** Override close() for custom behavior */
    void close();

    /** Override the accept in order to fade out the dialog  */
    void accept() override final;

    /** Override the reject in order to fade out the dialog  */
    void reject() override final;

    /**
     * Set background image to \p backgroundImage
     * @param backgroundImage Background image
     */
    void setBackgroundImage(const QPixmap& backgroundImage);

    /**
     * Invoked when the dialog is painted
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent);

private:

    /** Fade the dialog in */
    void fadeIn();

    /** Fade the dialog out and accept */
    void fadeOut();
    
private:
    QPropertyAnimation  _opacityAnimation;      /** Property animation for controlling window opacity */
    QPropertyAnimation  _positionAnimation;     /** Property animation for controlling window position */
    AnimationState      _animationState;        /** Animation state */
    std::int32_t        _autoHideTimeout;       /** Splash screen auto-hide trigger duration (active when duration > 0) */
    std::uint32_t       _panAmount;             /** The amount of panning in the y-direction during show and hide */
    std::uint32_t       _animationDuration;     /** The duration of the animations in milliseconds */
    QPixmap             _backgroundImage;       /** Background image */

    static const std::int32_t DEFAULT_AUTO_HIDE_TIMEOUT     = 0;
    static const std::uint32_t DEFAULT_PAN_AMOUNT           = 300;
    static const std::uint32_t DEFAULT_ANIMATION_DURATION   = 1000;

    friend class ProjectSplashScreenAction;
};

}
