// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/VerticalGroupAction.h"
#include "actions/ToggleAction.h"
#include "actions/IntegralAction.h"
#include "actions/TriggerAction.h"
#include "actions/TaskAction.h"
#include "actions/ImageAction.h"

#include "Task.h"

#include "widgets/SplashScreenDialog.h"

namespace hdps {
    class ProjectMetaAction;
}

namespace hdps::gui {


/**
 * Splash screen action class
 *
 * Action class for configuring and displaying a splash screen.
 *
 * @author Thomas Kroes
 */
class SplashScreenAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent object
     * @param parent Pointer to parent object
     */
    SplashScreenAction(QObject* parent, bool mayClose = false);

    bool getMayClose() const;

    ProjectMetaAction* getProjectMetaAction();
    void setProjectMetaAction(ProjectMetaAction* projectMetaAction);

    void showSplashScreenDialog();
    void closeSplashScreenDialog();

public: // Serialization

    /**
     * Load splash screen from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save splash screen to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    const ToggleAction& getEnabledAction() const { return _enabledAction; }
    const ToggleAction& getCloseManuallyAction() const { return _closeManuallyAction; }
    const IntegralAction& getDurationAction() const { return _durationAction; }
    const IntegralAction& getAnimationDurationAction() const { return _animationDurationAction; }
    const IntegralAction& getAnimationPanAmountAction() const { return _animationPanAmountAction; }
    const TriggerAction& getOpenAction() const { return _openAction; }
    const TriggerAction& getCloseAction() const { return _closeAction; }
    const VerticalGroupAction& getEditAction() const { return _editAction; }
    const ImageAction& getProjectImageAction() const { return _projectImageAction; }
    const ImageAction& getAffiliateLogosImageAction() const { return _affiliateLogosImageAction; }
    const TaskAction& getTaskAction() const { return _taskAction; }

    ToggleAction& getEnabledAction() { return _enabledAction; }
    ToggleAction& getCloseManuallyAction() { return _closeManuallyAction; }
    IntegralAction& getDurationAction() { return _durationAction; }
    IntegralAction& getAnimationDurationAction() { return _animationDurationAction; }
    IntegralAction& getAnimationPanAmountAction() { return _animationPanAmountAction; }
    TriggerAction& getOpenAction() { return _openAction; }
    TriggerAction& getCloseAction() { return _closeAction; }
    VerticalGroupAction& getEditAction() { return _editAction; }
    ImageAction& getProjectImageAction() { return _projectImageAction; }
    ImageAction& getAffiliateLogosImageAction() { return _affiliateLogosImageAction; }
    TaskAction& getTaskAction() { return _taskAction; }

private:
    bool                                    _mayClose;
    ProjectMetaAction*                      _projectMetaAction;
    ToggleAction                            _enabledAction;                 /** Action to toggle the splash screen on/off */
    ToggleAction                            _closeManuallyAction;           /** Action to toggle whether the splash screen has to be closed manually */
    IntegralAction                          _durationAction;                /** Action to control the display duration */
    IntegralAction                          _animationDurationAction;       /** Action to control the duration of the fade in/out animations */
    IntegralAction                          _animationPanAmountAction;      /** Action to control the amount of up/down panning of animations */
    ImageAction                             _projectImageAction;            /** Image action for the project image */
    ImageAction                             _affiliateLogosImageAction;     /** Image action for the affiliate logo's image */
    VerticalGroupAction                     _editAction;                    /** Vertical group action for editing the splash screen */
    TriggerAction                           _openAction;                    /** Trigger action to show the splash screen */
    TriggerAction                           _closeAction;                   /** Trigger action to manually close the splash screen */
    TaskAction                              _taskAction;                    /** Task action for showing load progress */
    std::unique_ptr<SplashScreenDialog>     _splashScreenDialog;            /** Splash screen dialog */
};

}
