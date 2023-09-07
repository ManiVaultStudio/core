// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "VerticalGroupAction.h"
#include "ToggleAction.h"
#include "IntegralAction.h"
#include "TriggerAction.h"
#include "GroupAction.h"
#include "ColorAction.h"
#include "ImageAction.h"

#include <QDialog>
#include <QPropertyAnimation>

class QMainWindow;

namespace hdps {
    class Project;
}

namespace hdps::gui {

/**
 * Splash screen action class
 *
 * Action class for configuring and displaying a project splash screen
 *
 * @author Thomas Kroes
 */
class ProjectSplashScreenAction : public HorizontalGroupAction
{
    Q_OBJECT
    
protected:

    /** Splash screen dialog class */
    class Dialog final : public QDialog {
    public:

        enum class AnimationState {
            Idle,
            FadingIn,
            FadingOut
        };

        /**
         * Construct owning \p projectSplashScreenAction and \p parent widget
         * @param projectSplashScreenAction Owning project splash screen action
         * @param parent Pointer to parent widget
         */
        Dialog(ProjectSplashScreenAction& projectSplashScreenAction, QWidget* parent = nullptr);

        /** Open the dialog */
        void open() override;

        /** Fade the dialog in */
        void fadeIn();

        /** Fade the dialog out and accept */
        void fadeOut();

        /**
         * Override paint event to draw the logo in the background
         * @param paintEvent Pointer to paint event
         */
        void paintEvent(QPaintEvent* paintEvent);

    private:

        /**
         * Get main window (for centering the splash screen w.r.t. the main window)
         * @return Pointer to main window
         */
        static QMainWindow* getMainWindow();

        /** Create layout with close tool button on the right */
        void createTopContent();

        /** Create layout with project image and accompanying text in the center */
        void createCenterContent();

        /** Create layout with affiliate logo's at the bottom */
        void createBottomContent();

    private:
        ProjectSplashScreenAction&  _projectSplashScreenAction;     /** Owning project splash screen action */
        QPropertyAnimation          _opacityAnimation;              /** Property animation for controlling window opacity */
        QPropertyAnimation          _positionAnimation;             /** Property animation for controlling window position */
        QPixmap                     _backgroundImage;               /** Background image */
        QVBoxLayout                 _mainLayout;                    /** Main vertical layout */
        QToolButton                 _closeToolButton;               /** Button for forcefully closing the splash screen */
        AnimationState              _animationState;                /** Animation state */

        friend class ProjectSplashScreenAction;
    };

protected:

    /**
     * Construct with \p parent and \p project
     * @param parent Pointer to parent object
     * @param project Reference to project which owns this action
     */
    ProjectSplashScreenAction(QObject* parent, const Project& project);

    /**
     * Get parent project
     * @return Reference to parent project
     */
    const Project& getProject() const;

public: // Serialization

    /**
     * Load project from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save project to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

public:
    const ToggleAction& getEnabledAction() const { return _enabledAction; }
    const ToggleAction& getCloseManuallyAction() const { return _closeManuallyAction; }
    const IntegralAction& getDurationAction() const { return _durationAction; }
    const IntegralAction& getAnimationDurationAction() const { return _animationDurationAction; }
    const IntegralAction& getAnimationPanAmountAction() const { return _animationPanAmountAction; }
    const ColorAction& getBackgroundColorAction() const { return _backgroundColorAction; }
    const GroupAction& getEditAction() const { return _editAction; }
    const TriggerAction& getShowSplashScreenAction() const { return _showSplashScreenAction; }
    const ImageAction& getProjectImageAction() const { return _projectImageAction; }
    const ImageAction& getAffiliateLogosImageAction() const { return _affiliateLogosImageAction; }

    ToggleAction& getEnabledAction() { return _enabledAction; }
    ToggleAction& getCloseManuallyAction() { return _closeManuallyAction; }
    IntegralAction& getDurationAction() { return _durationAction; }
    IntegralAction& getAnimationDurationAction() { return _animationDurationAction; }
    IntegralAction& getAnimationPanAmountAction() { return _animationPanAmountAction; }
    ColorAction& getBackgroundColorAction() { return _backgroundColorAction; }
    GroupAction& getEditAction() { return _editAction; }
    TriggerAction& getShowSplashScreenAction() { return _showSplashScreenAction; }
    ImageAction& getProjectImageAction() { return _projectImageAction; }
    ImageAction& getAffiliateLogosImageAction() { return _affiliateLogosImageAction; }
    
private:
    const Project&      _project;                       /** Reference to project which owns this action */
    ToggleAction        _enabledAction;                 /** Action to toggle the splash screen on/off */
    ToggleAction        _closeManuallyAction;           /** Action to toggle whether the splash screen has to be closed manually */
    IntegralAction      _durationAction;                /** Action to control the display duration */
    IntegralAction      _animationDurationAction;       /** Action to control the duration of the fade in/out animations */
    IntegralAction      _animationPanAmountAction;      /** Action to control the amount of up/down panning of animations */
    ColorAction         _backgroundColorAction;         /** Action to control the background color of the splash screen */
    VerticalGroupAction _editAction;                    /** Vertical group action for editing the splash screen */
    TriggerAction       _showSplashScreenAction;        /** Trigger action to show the splash screen */
    ImageAction         _projectImageAction;            /** Image action for the project image */
    ImageAction         _affiliateLogosImageAction;     /** Image action for the affiliate logo's image */
    Dialog              _splashScreenDialog;            /** Splash screen dialog */

    friend class hdps::Project;
};

}
