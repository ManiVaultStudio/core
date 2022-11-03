#pragma once

#include "WidgetAction.h"

#include <QCheckBox>
#include <QPushButton>

namespace hdps {

namespace gui {

/**
 * Toggle action class
 *
 * Toggle action with check/toggle button UI
 *
 * @author Thomas Kroes
 */
class ToggleAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        /** Push button options */
        Icon                = 0x00001,          /** Enable push button icon */
        Text                = 0x00002,          /** Enable push button text */

        CheckBox            = 0x00004,          /** The widget includes a check box */
        PushButton          = 0x00008 | Text,   /** The widget includes a toggle push button with text */

        /** Push button configurations */
        PushButtonIcon          = (PushButton & ~Text) | Icon,  /** Push button with icon only */
        PushButtonText          = PushButton,                   /** Push button with text only */
        PushButtonIconText      = PushButton | Icon,            /** Push button with icon and text */

        Default = CheckBox
    };

public:

    /** Check box widget class for toggle action */
    class CheckBoxWidget : public QCheckBox
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toggleAction Pointer to toggle action
         */
        CheckBoxWidget(QWidget* parent, ToggleAction* toggleAction);

    protected:
        ToggleAction*   _toggleAction;      /** Pointer to toggle action */

        friend class ToggleAction;
    };

    /** Push button widget class for toggle action */
    class PushButtonWidget : public QPushButton
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toggleAction Pointer to toggle action
         * @param widgetFlags Widget flags
         */
        PushButtonWidget(QWidget* parent, ToggleAction* toggleAction, const std::int32_t& widgetFlags);

    protected:
        ToggleAction*   _toggleAction;      /** Pointer to toggle action */

        friend class ToggleAction;
    };

protected:

    /**
     * Get widget representation of the toggle action
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
     * @param defaultToggled Default toggled
     */
    ToggleAction(QObject* parent, const QString& title = "", const bool& toggled = false, const bool& defaultToggled = false);

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /**
     * Initialize the toggle action
     * @param toggled Toggled
     * @param defaultToggled Default toggled
     */
    void initialize(const bool& toggled = false, const bool& defaultToggled = false);

    /** Gets default toggled */
    bool getDefaultToggled() const;

    /**
     * Sets default toggled value
     * @param defaultToggled Default toggled
     */
    void setDefaultToggled(const bool& defaultToggled);

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

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

protected:  // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    WidgetAction* getPublicCopy() const override;

signals:

    /**
     * Signals that the default toggled changed
     * @param defaultToggled Default toggled that changed
     */
    void defaultToggledChanged(const bool& defaultToggled);

    /**
     * Signals that the indeterminate value changed
     * @param indeterminate Whether the toggle action is in an indeterminate state
     */
    void indeterminateChanged(bool indeterminate);

protected:
    bool    _defaultToggled;        /** Whether toggled by default */
    bool    _indeterminate;         /** Whether the toggle action is in an indeterminate state */
};

}
}
