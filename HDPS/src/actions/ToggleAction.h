#pragma once

#include "WidgetAction.h"

class QCheckBox;
class QPushButton;

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
        CheckBox            = 0x00001,      /** The widget includes a check box */
        Button              = 0x00002,      /** The widget includes a (toggle) push button */
        ResetPushButton     = 0x00004,      /** The widget includes a reset push button */

        Basic                       = CheckBox,
        CheckBoxAndResetPushButton  = CheckBox | ResetPushButton,
        ButtonAndPushButton         = Button | ResetPushButton
    };

public:

    /**
     * Check box widget class for toggle action
     */
    class CheckBoxWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toggleAction Pointer to toggle action
         */
        CheckBoxWidget(QWidget* parent, ToggleAction* toggleAction);

    public:

        /** Gets the check box widget */
        QCheckBox* getCheckBox() { return _checkBox; }

    protected:
        QCheckBox*      _checkBox;      /** Check box widget */

        friend class ToggleAction;
    };

    /**
     * Push button widget class for toggle action
     */
    class PushButtonWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toggleAction Pointer to toggle action
         */
        PushButtonWidget(QWidget* parent, ToggleAction* checkAction);

    public:

        /** Gets the push button widget */
        QPushButton* getPushButton() { return _pushButton; }

    protected:
        QPushButton*    _pushButton;        /** Push button widget */

        friend class ToggleAction;
    };

protected:

    /**
     * Get widget representation of the toggle action
     * @param parent Pointer to parent widget
     * @param state Widget state
     */
    QWidget* getWidget(QWidget* parent, const WidgetActionWidget::State& state = WidgetActionWidget::State::Standard) override;

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

    /** Determines whether the action can be reset */
    bool isResettable() const override;

    /** Reset to default */
    void reset() override;

    /**
     * Creates a check box widget to interact with the toggle action
     * @param parent Pointer to parent widget
     * @return Check box widget
     */
    CheckBoxWidget* createCheckBoxWidget(QWidget* parent) { return new CheckBoxWidget(parent, this);  };

    /**
     * Creates a push button widget to interact with the toggle action
     * @param parent Pointer to parent widget
     * @return Push button widget
     */
    PushButtonWidget* createPushButtonWidget(QWidget* parent) { return new PushButtonWidget(parent, this); };

signals:

    /**
     * Signals that the default toggled changed
     * @param defaultToggled Default toggled that changed
     */
    void defaultToggledChanged(const bool& defaultToggled);

protected:
    bool    _defaultToggled;        /** Whether toggled by default */
};

}
}
