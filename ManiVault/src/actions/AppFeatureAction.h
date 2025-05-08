// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "ToggleAction.h"
#include "VerticalGroupAction.h"

namespace mv::gui {

/**
 * App feature action class
 *
 * Base vertical group action class for grouping app feature settings
 *
 * Note: This action is developed for internal use only
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT AppFeatureAction : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /** Enum for the prefix type */
    enum class PrefixType
    {
        Enabled,        /** Prefix for the enabled type */
        UserHasOpted,   /** Prefix for the user has opted type */
    };

protected:

    /** Dialog for giving consent for a specific app feature */
    class CORE_EXPORT ConsentDialog final : public QDialog
    {
    protected:

        /**
         * Construct a dialog with \p parent
         * @param appFeatureAction Pointer to owning app feature action
         * @param parent Pointer to parent widget
         */
        ConsentDialog(AppFeatureAction* appFeatureAction, QWidget* parent = nullptr);

        /** Get preferred size */
        QSize sizeHint() const override;

        /** Get minimum size hint*/
        QSize minimumSizeHint() const override {
            return sizeHint();
        }

    private:
        QVBoxLayout     _layout;                    /** Main layout */
        QLabel          _notificationLabel;         /** Notification label */
        QHBoxLayout     _buttonsLayout;             /** Bottom buttons layout */
        QPushButton     _acceptPushButton;          /** Opt in of automated error reporting when clicked */
        QPushButton     _cancelPushButton;          /** Abort consent process */
        QPushButton     _decideLaterPushButton;     /** Exit the dialog and show it at next startup */

        friend class AppFeatureAction;
    };

public:

    /**
     * Construct with pointer to \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE AppFeatureAction(QObject* parent, const QString& title);

    /**
     * Add \p action to the group
     * @param action Pointer to action to add
     * @param widgetFlags Action widget flags (default flags if -1)
     * @param widgetConfigurationFunction When set, overrides the standard widget configuration function in the widget action
     * @param load Currently not used
     */
    void addAction(WidgetAction* action, std::int32_t widgetFlags = -1, WidgetConfigurationFunction widgetConfigurationFunction = WidgetConfigurationFunction(), bool load = true) override;

    /**
     * Get the HTML resource file location
     * @return Location of the app feature description HTML resource file
     */
    QString getResourceLocation() const;

protected:

    /**
     * Get the settings prefix for the given \p prefixType
     * @param prefixType Prefix type to get the settings prefix for
     * @return Settings prefix
     */
    QString getSettingsPrefix(const PrefixType& prefixType) const;

    /**
     * Get whether a setting is available
     * @param prefixType Prefix type to check
     * @return Pointer to the settings action
     */
    bool hasSetting(const PrefixType& prefixType) const;

    /**
     * Load the description from \p resourceName
     * @param resourceLocation Location of the resource to load
     */
    void loadDescriptionFromResource(const QString& resourceLocation);

private: // Settings

    /**
     * Get whether the feature is on or off from settings
     * @return Boolean determining whether the feature is on or off (off if the setting is not found)
     */
    bool getEnabledFromSettings() const;

    /**
     * Get whether the user has opted for the feature or not from settings
     * @return Boolean determining whether the user has opted for the feature or not (off if the setting is not found)
     */
    bool getUserHasOptedFromSettings() const;

public: // Action getters

    const ToggleAction& getEnabledAction() const { return _enabledAction; }
    const StringAction& getSummaryAction() const { return _summaryAction; }
    const ToggleAction& getUserHasOptedAction() const { return _userHasOptedAction; }
    const StringAction& getDescriptionAction() const { return _descriptionAction; }

protected: // Action getters

    ToggleAction& getEnabledAction() { return _enabledAction; }
    StringAction& getSummaryAction() { return _summaryAction; }
    ToggleAction& getUserHasOptedAction() { return _userHasOptedAction; }
    StringAction& getDescriptionAction() { return _descriptionAction; }

private:
    ToggleAction            _enabledAction;         /** Determines whether the app feature is enabled or not */
    StringAction            _summaryAction;         /** Short one-liner that describes the app feature */
    ToggleAction            _userHasOptedAction;    /** Determines if the user has given permission to use the app feature */
    StringAction            _descriptionAction;     /** App feature description */
    VerticalGroupAction     _settingsAction;        /** App feature settings */
    QString                 _resourceLocation;      /** Location of the resource to load */

    friend class AppFeaturesSettingsAction; // Allow access to private members
};

}

Q_DECLARE_METATYPE(mv::gui::AppFeatureAction)

inline const auto appFeatureActionMetaTypeId = qRegisterMetaType<mv::gui::AppFeatureAction*>("mv::gui::AppFeatureAction");
