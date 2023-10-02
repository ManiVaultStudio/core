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

#include <QPointer>

namespace hdps {
    class ProjectMetaAction;
}

namespace hdps::gui {

class SplashScreenWidget;

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
     * Construct with \p parent object and optional parameter to control whether the user can close the splash screen widget with a close tool button
     * @param parent Pointer to parent object
     * @param mayCloseSplashScreenWidget Whether the user can close the splash screen widget with a close tool button
     */
    SplashScreenAction(QObject* parent, bool mayCloseSplashScreenWidget = false);

    /**
     * Destructor
     * A destructor is mandatory here because of the SplashScreenAction#_splashScreenDialog QScopePointer
     * https://doc.qt.io/qt-6/qscopedpointer.html#forward-declared-pointers
     */
    ~SplashScreenAction();

protected:

    /**
     * Get may close splash screen widget
     * @return Whether the user can close the splash screen widget with a close tool button
     */
    bool getMayCloseSplashScreenWidget() const;

public:

    /**
     * Get project meta action
     * @return Pointer to project meta action, nullptr if not available
     */
    ProjectMetaAction* getProjectMetaAction();

    /**
     * Get project meta action to \p projectMetaAction
     * @param projectMetaAction Pointer to project meta action
     */
    void setProjectMetaAction(ProjectMetaAction* projectMetaAction);

protected:

    /** Shows the splash screen and animates its opacity and position */
    void showSplashScreenWidget();

    /** Animates the splash screen widget's opacity and position and afterwards closes it */
    void closeSplashScreenWidget();

public: // Serialization

    /**
     * Load splash screen action from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save splash screen action to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

private:
    
    /** https://doc.qt.io/qt-6/qscopedpointer.html#forward-declared-pointers */
    Q_DISABLE_COPY(SplashScreenAction)

public: // Action getters

    const ToggleAction& getEnabledAction() const { return _enabledAction; }
    const TriggerAction& getOpenAction() const { return _openAction; }
    const TriggerAction& getCloseAction() const { return _closeAction; }
    const VerticalGroupAction& getEditAction() const { return _editAction; }
    const ImageAction& getProjectImageAction() const { return _projectImageAction; }
    const ImageAction& getAffiliateLogosImageAction() const { return _affiliateLogosImageAction; }
    const TaskAction& getTaskAction() const { return _taskAction; }

    ToggleAction& getEnabledAction() { return _enabledAction; }
    TriggerAction& getOpenAction() { return _openAction; }
    TriggerAction& getCloseAction() { return _closeAction; }
    VerticalGroupAction& getEditAction() { return _editAction; }
    ImageAction& getProjectImageAction() { return _projectImageAction; }
    ImageAction& getAffiliateLogosImageAction() { return _affiliateLogosImageAction; }
    TaskAction& getTaskAction() { return _taskAction; }

private:
    bool                            _mayCloseSplashScreenWidget;    /** Whether the user can close the splash screen widget with a close tool button */
    ProjectMetaAction*              _projectMetaAction;             /** Pointer to project meta action (used by the splash screen widget to display project information) */
    ToggleAction                    _enabledAction;                 /** Action to toggle the splash screen on/off */
    ImageAction                     _projectImageAction;            /** Image action for the project image */
    ImageAction                     _affiliateLogosImageAction;     /** Image action for the affiliate logo's image */
    VerticalGroupAction             _editAction;                    /** Vertical group action for editing the splash screen */
    TriggerAction                   _openAction;                    /** Trigger action to show the splash screen */
    TriggerAction                   _closeAction;                   /** Trigger action to manually close the splash screen */
    TaskAction                      _taskAction;                    /** Task action for showing load progress */
    QPointer<SplashScreenWidget>    _splashScreenWidget;            /** Splash screen dialog */

    friend class SplashScreenWidget;
    friend class ProjectMetaAction;
};

}
