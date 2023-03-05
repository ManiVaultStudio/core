#pragma once

#include "ToolbarAction.h"

namespace hdps::gui {

/**
 * Horizontal toolbar action class
 *
 * Widget action for displaying actions in a horizontal toolbar
 *
 * @author Thomas Kroes
 */
class HorizontalToolbarAction : public ToolbarAction
{
    Q_OBJECT

public: // Widgets

    /** Widget class for the horizontal toolbar */
    class ToolbarWidget final : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param horizontalToolbarAction Pointer to horizontal toolbar action that creates the widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        ToolbarWidget(QWidget* parent, HorizontalToolbarAction* horizontalToolbarAction, const std::int32_t& widgetFlags);

        /**
         * Respond to \p target object events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:

        virtual void setActionWidgets() final;

    protected:
        HorizontalToolbarAction*    _horizontalToolbarAction;   /** Pointer to horizontal toolbar action that creates the widget */
        QHBoxLayout                 _layout;                    /** Main layout */

        friend class HorizontalToolbarAction;
    };

protected: // Widgets

    /**
     * Get widget representation of the group action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new ToolbarWidget(parent, this, widgetFlags);
    }

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    HorizontalToolbarAction(QObject* parent, const QString& title = "Horizontal Toolbar");

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;
};

}