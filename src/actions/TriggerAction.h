// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include <QPushButton>

namespace mv::gui {

/**
 * Trigger action class
 *
 * Action class for trigger
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT TriggerAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        /** Push button options */
        Icon = 0x00001,          /** Enable push button icon */
        Text = 0x00002,          /** Enable push button text */

        /** Push button configurations */
        IconText = Icon | Text              /** Push button with icon and text */
    };

public:

    /** Push button widget class for trigger action */
    class CORE_EXPORT PushButtonWidget : public QPushButton {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param triggerAction Pointer to trigger action
         * @param widgetFlags Widget flags
         */
        PushButtonWidget(QWidget* parent, TriggerAction* triggerAction, const std::int32_t& widgetFlags);

        /**
         * Override this event to make the push button square (in case only an icon is used)
         * @param event Pointer to resize event
         */
        void resizeEvent(QResizeEvent* event) override;
        
        /**
         * Override paint event to fix icon alignment issues
         * @param event Pointer to paint event
         */
        void paintEvent(QPaintEvent* event) override;
        
    protected:
        TriggerAction*  _triggerAction;     /** Pointer to trigger action */
        std::int32_t    _widgetFlags;       /** Widget flags */

        friend class TriggerAction;
    };

protected:

    /**
     * Get widget representation of the trigger action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Icon

    /**
     * Set the icon by \p iconName and use the default icon font and version
     * @param iconName Name of the icon
     */
    void setIconByName(const QString& iconName) override;

    /**
     * Set the icon by \p iconName and possibly override the default \p iconFontName and \p iconFontVersion
     * @param iconName Name of the icon
     * @param iconFontName Name of the icon font
     * @param iconFontVersion Version of the icon font
     */
    void setIconByName(const QString& iconName, const QString& iconFontName, const util::Version& iconFontVersion) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE TriggerAction(QObject* parent, const QString& title);

private:

    /** Invoked when this trigger action is triggered (needed to prevent circular calls of trigger()) */
    void selfTriggered();

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::TriggerAction)

inline const auto triggerActionMetaTypeId = qRegisterMetaType<mv::gui::TriggerAction*>("mv::gui::TriggerAction");
