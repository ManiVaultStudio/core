#pragma once

#include "actions/WidgetAction.h"
#include "actions/DecimalAction.h"

using namespace hdps::gui;

/**
 * Window/level settings action class
 *
 * Action class for window/level settings
 *
 * @author Thomas Kroes
 */
class WindowLevelAction : public WidgetAction
{
Q_OBJECT

public:

    /** Widget class for settings action */
    class Widget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param windowLevelAction Pointer to window level action
         */
        Widget(QWidget* parent, WindowLevelAction* windowLevelAction);

    protected:
        friend class WindowLevelAction;
    };

protected:

    /**
     * Get widget representation of the window & level action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

public:

    /** 
     * Constructor
     * @param parent Pointer to parent object
     */
    WindowLevelAction(QObject* parent);

public: /** Action getters */

    DecimalAction& getWindowAction() { return _windowAction; }
    DecimalAction& getLevelAction() { return _levelAction; }

signals:

    /** Signals the window/level changed */
    void changed(WindowLevelAction& windowLevelAction);

protected:
    DecimalAction   _windowAction;      /** Window action */
    DecimalAction   _levelAction;       /** Level action */
};
