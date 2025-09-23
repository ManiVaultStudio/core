// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/TaskAction.h"
#include "actions/ToggleAction.h"
#include "actions/TriggerAction.h"
#include "actions/StringAction.h"
#include "actions/VerticalGroupAction.h"

#include "Task.h"

#include <QTimer>

namespace mv {
    class ProjectMetaAction;
}

namespace mv::gui {

class SplashScreenWidget;

/**
 * Splash screen action class
 *
 * For configuring and displaying a splash screen.
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
         * Construct from \p type, \p iconName, \p message and color
         * @param type Alert type
         * @param iconName Name of the icon (FontAwesome web fonts)
         * @param message Error message
         * @param color Icon and error message color
         */
        Alert(const Type& type, const QString& iconName, const QString& message, const QColor& color = Qt::black):
            _type(type),
            _iconName(iconName),
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
         * Get alert icon name
         * @return Alert icon name (FontAwesome web fonts)
         */
        QString getIconName() const;

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

    public: // Static constructors for convenience

        static Alert info(const QString& message);      /** Create information alert */
        static Alert debug(const QString& message);     /** Create debug alert */
        static Alert warning(const QString& message);   /** Create warning alert */

    private:
        Type        _type;      /** Alert type */
        QString     _iconName;  /** Alert icon name */
        QString     _message;   /** Alert message */
        QColor      _color;     /** Icon and alert message color */
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

    /**
     * Get HTML content
     * @return HTML content, empty string if not available
     */
    QString getHtml() const;

    /**
     * Convert \p pixmap to base64 string
     * @param pixmap Pixmap to convert
     * @return Base64 representation of the pixmap, empty string if not available
     */
    static QString pixmapToBase64(const QPixmap& pixmap);

public:

    /**
     * Get project meta action
     * @return Pointer to project meta action, nullptr if not available
     */
    ProjectMetaAction* getProjectMetaAction() const;

    /**
     * Get project meta action to \p projectMetaAction
     * @param projectMetaAction Pointer to project meta action
     */
    void setProjectMetaAction(ProjectMetaAction* projectMetaAction);

protected:

    /** Shows the splash screen widget */
    void showSplashScreenWidget();

    /** Animates the splash screen widget's opacity and position and afterwards closes it */
    void closeSplashScreenWidget();

    /** Establishes whether project information should be shown or not */
    bool shouldDisplayProjectInfo() const;

    /**
     * Get HTML from template (located in resources)
     * @return HTML content from template, empty string if not available
     */
    QString getHtmlFromTemplate() const;

    /**
     * Set startup task to \p startupTask
     * @param startupTask Pointer to startup task
     */
    void setStartupTask(Task* startupTask);

public: // Serialization

    /**
     * Load splash screen action from variant
     * @param variantMap Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save splash screen action to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    const ToggleAction& getEnabledAction() const { return _enabledAction; }
    const ToggleAction& getOverrideAction() const { return _overrideAction; }
    const StringAction& getHtmlOverrideAction() const { return _htmlOverrideAction; }
    const VerticalGroupAction& getEditAction() const { return _editAction; }
	const TriggerAction& getOpenAction() const { return _openAction; }
	const TriggerAction& getTestAction() const { return _testAction; }
    const TriggerAction& getCloseAction() const { return _closeAction; }
    const TaskAction& getTaskAction() const { return _taskAction; }

    ToggleAction& getEnabledAction() { return _enabledAction; }
    ToggleAction& getOverrideAction() { return _overrideAction; }
    StringAction& getHtmlOverrideAction() { return _htmlOverrideAction; }
    VerticalGroupAction& getEditAction() { return _editAction; }
	TriggerAction& getOpenAction() { return _openAction; }
	TriggerAction& getTestAction() { return _testAction; }
    TriggerAction& getCloseAction() { return _closeAction; }
    TaskAction& getTaskAction() { return _taskAction; }

private:
    bool                                _mayCloseSplashScreenWidget;    /** Whether the user can close the splash screen widget with a close tool button */
    ProjectMetaAction*                  _projectMetaAction;             /** Shared pointer to project meta action (used by the splash screen widget to display project information) */
    ToggleAction                        _enabledAction;                 /** Action to setEnabled the splash screen on/off */
    ToggleAction                        _overrideAction;                /** Toggle action to override the default ManiVault splash screen */
    StringAction                        _htmlOverrideAction;            /** String action for custom html content */
    VerticalGroupAction                 _editAction;                    /** Vertical group action for editing the splash screen */
    TriggerAction                       _openAction;                    /** Trigger action to show the splash screen */
    TriggerAction                       _testAction;                    /** Trigger action to briefly show the splash screen for testing purposes */
    TriggerAction                       _closeAction;                   /** Trigger action to manually close the splash screen */
    TaskAction                          _taskAction;                    /** Task action for showing load progress */
    QPointer<SplashScreenWidget>        _splashScreenWidget;            /** Splash screen dialog */
    Alerts                              _alerts;                        /** Alerts that will be displayed on the splash screen widget */
    Task                                _simulateStartupTask;           /** Test task to simulate load progress */
    QTimer                              _simulateTimer;                 /** Timer to simulate load progress */

    friend class mv::Application;
};

}
