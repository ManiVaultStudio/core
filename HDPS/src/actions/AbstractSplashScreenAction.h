// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/HorizontalGroupAction.h"
#include "actions/ToggleAction.h"
#include "actions/IntegralAction.h"
#include "actions/ColorAction.h"
#include "actions/TriggerAction.h"
#include "actions/TaskAction.h"

#include "widgets/SplashScreenDialog.h"

namespace hdps::gui {

/**
 * Abstract splash screen action class
 *
 * Base action class for configuring and displaying a splash screen.
 *
 * @author Thomas Kroes
 */
class AbstractSplashScreenAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    AbstractSplashScreenAction(QObject* parent);

    /** Get an instance of a splash screen dialog */
    virtual SplashScreenDialog* getSplashScreenDialog() = 0;

protected:

    /** Open screen dialog and set AbstractSplashScreenAction#_splashScreenDialog accordingly */
    void openSplashScreenDialog();

    /** Close screen dialog pointed to by AbstractSplashScreenAction#_splashScreenDialog if it exists */
    void closeSplashScreenDialog();

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

public: // Action getters

    const ToggleAction& getEnabledAction() const { return _enabledAction; }
    const ToggleAction& getCloseManuallyAction() const { return _closeManuallyAction; }
    const IntegralAction& getDurationAction() const { return _durationAction; }
    const IntegralAction& getAnimationDurationAction() const { return _animationDurationAction; }
    const IntegralAction& getAnimationPanAmountAction() const { return _animationPanAmountAction; }
    const ColorAction& getBackgroundColorAction() const { return _backgroundColorAction; }
    const TriggerAction& getOpenAction() const { return _openAction; }
    const TriggerAction& getCloseAction() const { return _closeAction; }
    const TaskAction& getTaskAction() const { return _taskAction; }

    ToggleAction& getEnabledAction() { return _enabledAction; }
    ToggleAction& getCloseManuallyAction() { return _closeManuallyAction; }
    IntegralAction& getDurationAction() { return _durationAction; }
    IntegralAction& getAnimationDurationAction() { return _animationDurationAction; }
    IntegralAction& getAnimationPanAmountAction() { return _animationPanAmountAction; }
    ColorAction& getBackgroundColorAction() { return _backgroundColorAction; }
    TriggerAction& getOpenAction() { return _openAction; }
    TriggerAction& getCloseAction() { return _closeAction; }
    TaskAction& getTaskAction() { return _taskAction; }

private:
    ToggleAction                        _enabledAction;                 /** Action to toggle the splash screen on/off */
    ToggleAction                        _closeManuallyAction;           /** Action to toggle whether the splash screen has to be closed manually */
    IntegralAction                      _durationAction;                /** Action to control the display duration */
    IntegralAction                      _animationDurationAction;       /** Action to control the duration of the fade in/out animations */
    IntegralAction                      _animationPanAmountAction;      /** Action to control the amount of up/down panning of animations */
    ColorAction                         _backgroundColorAction;         /** Action to control the background color of the splash screen */
    TriggerAction                       _openAction;                    /** Trigger action to show the splash screen */
    TriggerAction                       _closeAction;                   /** Trigger action to manually close the splash screen */
    TaskAction                          _taskAction;                    /** Task action for showing load progress */
    QScopedPointer<SplashScreenDialog>  _splashScreenDialog;            /** Splash screen dialog */
};

}
