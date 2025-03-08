// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/ImageAction.h"
#include "actions/TaskAction.h"
#include "actions/ToggleAction.h"
#include "actions/TriggerAction.h"
#include "actions/VerticalGroupAction.h"

#include <QPointer>

namespace mv {
    class ProjectMetaAction;
}

namespace mv::gui {

class SplashScreenWidget;

/**
 * Splash screen action class
 *
 * Action class for configuring and displaying a splash screen.
 *
 * Note: This action is developed for internal use only
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT SplashScreenAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /** Basic class to add an alert on the splash screen */
    class CORE_EXPORT Alert
    {
    public:
        enum class Type {
            Info,
            Debug,
            Warning
        };

    public:
        /**
         * Construct from \p icon and \p message
         * @param icon Icon
         * @param message Error message
         * @param color Icon and error message color
         */
        Alert(const Type& type, const QIcon& icon, const QString& message, const QColor& color = Qt::black):
            _type(type),
            _icon(icon),
            _message(message),
            _color(color)
        {
        }

        /**
         * Get alert type
         * @return Alert type
         */
        Type getType() const;

        /**
         * Get alert icon
         * @return Alert icon
         */
        QIcon getIcon() const;

        /**
         * Get alert message
         * @return Alert message
         */
        QString getMessage() const;

        /**
         * Get alert color
         * @return Alert color
         */
        QColor getColor() const;

        /**
         * Get label representation of the alert icon
         * @param parent Pointer to parent widget (if any)
         * @return Pointer to label
         */
        QLabel* getIconLabel(QWidget* parent = nullptr) const;

        /**
         * Get label representation of the alert message
         * @param parent Pointer to parent widget (if any)
         * @return Pointer to label
         */
        QLabel* getMessageLabel(QWidget* parent = nullptr) const;

        static Alert info(const QString& message);      /** Get warning error message */
        static Alert debug(const QString& message);     /** Get warning error message */
        static Alert warning(const QString& message);   /** Get warning error message */

    private:
        Type        _type;      /** Error type */
        QIcon       _icon;      /** Error icon */
        QString     _message;   /** Error message */
        QColor      _color;     /** Icon and error message color */
    };

    using Alerts = QVector<Alert>;

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
    ~SplashScreenAction() override = default;

    /**
     * Add \p alert to the splash screen
     * @param alert Alert to add
     */
    void addAlert(const Alert& alert);

    /**
     * Get alerts
     * @return Vector of alerts
     */
    Alerts getAlerts() const;

    /**
     * Get may close splash screen widget
     * @return Whether the user can close the splash screen widget with a close tool button
     */
    bool getMayCloseSplashScreenWidget() const;

    /**
     * Set may close splash screen widget
     * @param mayCloseSplashScreenWidget Whether the user can close the splash screen widget with a close tool button
     */
    void setMayCloseSplashScreenWidget(bool mayCloseSplashScreenWidget);

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
    ToggleAction                    _enabledAction;                 /** Action to setEnabled the splash screen on/off */
    ImageAction                     _projectImageAction;            /** Image action for the project image */
    ImageAction                     _affiliateLogosImageAction;     /** Image action for the affiliate logo's image */
    VerticalGroupAction             _editAction;                    /** Vertical group action for editing the splash screen */
    TriggerAction                   _openAction;                    /** Trigger action to show the splash screen */
    TriggerAction                   _closeAction;                   /** Trigger action to manually close the splash screen */
    TaskAction                      _taskAction;                    /** Task action for showing load progress */
    QPointer<SplashScreenWidget>    _splashScreenWidget;            /** Splash screen dialog */
    Alerts                          _alerts;                        /** Alerts that will be displayed on the splash screen widget */
};

}
