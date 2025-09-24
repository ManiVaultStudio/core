// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
#include "Task.h"

#include "util/SplashScreenBridge.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QWebEngineView>
#include <QWebChannel>

namespace mv::gui {

class SplashScreenAction;

/**
 * Splash screen widget class
 *
 * For displaying a splash screen using an mv::gui::SplashScreenAction
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT SplashScreenWidget : public QWidget
{
public:

    /**
     * Construct with reference to owning \p splashScreenAction and optional pointer to \p parent widget
     * @param splashScreenAction Reference to owning splashScreenAction
     * @param parent Pointer to parent widget
     */
    SplashScreenWidget(SplashScreenAction& splashScreenAction, QWidget* parent = nullptr);

    /** Destructor */
    ~SplashScreenWidget() override;

    /**
     * Override the show event to initialize the widget once, wraps QWidget::showEvent()
     * @param event Pointer to show event
     */
    void showEvent(QShowEvent* event) override;

    /**
     * Override the hide event to stop the event processing timer, wraps QWidget::hideEvent()
     * @param event Pointer to hide event
     */
    void hideEvent(QHideEvent* event) override;

    /** Show the splash screen widget animated, wraps QWidget::show() */
    void showAnimated();

    /** Animate the splash screen widget and close it, wraps QWidget::close() */
    void closeAnimated();

private: // Splash screen content

    /** Get the ManiVault copyright notice tooltip string */
    static QString getCopyrightNoticeTooltip();

    /** Connect to the current task signals */
    void connectToCurrentTask();

    /** Disconnect from the current task signals */
    void disconnectFromCurrentTask();

private:
    bool                        _initialized{ false };  /** Whether the widget has been initialized */
    SplashScreenAction&         _splashScreenAction;    /** Reference to owning splashScreenAction */
    QPixmap                     _logoImage;             /** Logo image */
    QPixmap                     _backgroundImage;       /** Background image */
    QFrame                      _roundedFrame;          /** Frame with rounded edges */
    QVBoxLayout                 _roundedFrameLayout;    /** Layout for the rounded frame */
    QToolButton                 _closeToolButton;       /** Button for forcefully closing the splash screen */
    QWebEngineView              _webEngineView;         /** Web engine view for displaying the HTML content */
    QWebChannel                 _webChannel;            /** Web channel for communication between C++ and JavaScript */
    util::SplashScreenBridge    _splashScreenBridge;    /** Bridge for communication between C++ and JavaScript */
    QGraphicsDropShadowEffect   _dropShadowEffect;      /** For adding a drop shadow to the splash screen widget */
    QTimer                      _processEventsTimer;    /** Timer to keep the splash screen widget somewhat responsive */
    Task*                       _currentTask;           /** Current task being displayed on the splash screen */

    static const std::uint32_t fixedWidth           = 640;      /** Widget fixed width */
    static const std::uint32_t fixedHeight          = 480;      /** Widget fixed height */
    static const std::uint32_t frameRadius          = 7;        /** CSS radius of SplashScreenWidget#_roundedFrame */
    static const std::uint32_t animationDuration    = 250;      /** Duration of the show and close animations */
    static const std::uint32_t panAmount            = 25;       /** The amount of up/down panning to do during the animations */
    static const std::uint32_t shadowMargin         = 20;       /** Extra margins allocated for drop shadow */
    static const std::uint32_t maxBlurRadius        = 20.0;     /** Maximum blur radius */
};

}
