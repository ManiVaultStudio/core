// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "WidgetAction.h"

#include <QCheckBox>
#include <QPushButton>
#include <QLabel>

namespace mv::gui {

/**
 * Toggle action class
 *
 * Toggle action with check/setEnabled button UI
 *
 * @author Thomas Kroes
 */
class CORE_EXPORT ToggleAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        /** Push button options */
        Icon                = 0x00001,          /** Enable push button icon */
        Text                = 0x00002,          /** Enable push button text */

        CheckBox            = 0x00004,          /** The widget includes a checkbox */
        PushButton          = 0x00008 | Text,   /** The widget includes a setEnabled push button with text */
        ToggleImage         = 0x00010,          /** Widget uses an image that changes based on toggle state */

        /** Push button configurations */
        PushButtonIcon          = (PushButton & ~Text) | Icon,  /** Push button with icon only */
        PushButtonText          = PushButton,                   /** Push button with text only */
        PushButtonIconText      = PushButton | Icon,            /** Push button with icon and text */

        Default = CheckBox
    };

public:

    /** Check box widget class for toggle action */
    class CORE_EXPORT CheckBoxWidget : public QCheckBox
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toggleAction Pointer to toggle action
         */
        CheckBoxWidget(QWidget* parent, ToggleAction* toggleAction);

        /**
         * Respond to \p target \p event
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    protected:
        ToggleAction*   _toggleAction;      /** Pointer to toggle action */

        friend class ToggleAction;
    };

    /** Push button widget class for toggle action */
    class CORE_EXPORT PushButtonWidget : public QPushButton
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toggleAction Pointer to toggle action
         * @param widgetFlags Widget flags
         */
        PushButtonWidget(QWidget* parent, ToggleAction* toggleAction, const std::int32_t& widgetFlags);

        /**
         * Override this event to make the push button square (in case only an icon is used)
         * @param event Pointer to resize event
         */
        void resizeEvent(QResizeEvent* event) override;

    protected:
        ToggleAction*   _toggleAction;      /** Pointer to toggle action */
        std::int32_t    _widgetFlags;       /** Widget flags */

        friend class ToggleAction;
    };

    /** Toggle image label widget class for toggle action */
    class CORE_EXPORT ToggleImageLabelWidget : public QLabel
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toggleAction Pointer to toggle action
         */
        ToggleImageLabelWidget(QWidget* parent, ToggleAction* toggleAction);

        /**
         * Invoked when the mouse button is pressed
         * @param event Pointer to mouse event that occurred
         */
        void mousePressEvent(QMouseEvent* event) override;

    protected:
        ToggleAction* _toggleAction;      /** Pointer to toggle action */

        friend class ToggleAction;
    };

protected:

    /**
     * Get widget representation of the setEnabled action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param toggled Toggled
     */
    Q_INVOKABLE ToggleAction(QObject* parent, const QString& title, bool toggled = false);

    /**
     * Overrides the base class setChecked()
     * @param checked Checked status
     */
    void setChecked(bool checked);

    /**
     * Get indeterminate state
     * @return Whether the toggle action is in an indeterminate state
     */
    bool getIndeterminate() const;

    /**
     * Set indeterminate state
     * @param indeterminate Whether the toggle action is in an indeterminate state
     */
    void setIndeterminate(bool indeterminate);

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

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

signals:

    /**
     * Signals that the indeterminate value changed
     * @param indeterminate Whether the toggle action is in an indeterminate state
     */
    void indeterminateChanged(bool indeterminate);

protected:
    bool    _indeterminate;     /** Whether the setEnabled action is in an indeterminate state */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::ToggleAction)

inline const auto toggleActionMetaTypeId = qRegisterMetaType<mv::gui::ToggleAction*>("mv::gui::ToggleAction");
