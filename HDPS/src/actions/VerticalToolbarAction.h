#pragma once

#include "ToolbarAction.h"

#include <QTimer>

namespace hdps::gui {

/**
 * Vertical toolbar action class
 *
 * Widget action for displaying actions in a vertical toolbar
 *
 * @author Thomas Kroes
 */
class VerticalToolbarAction : public ToolbarAction
{
    Q_OBJECT

public: // Widgets

    /** Widget class for the horizontal toolbar */
    class Widget final : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param verticalToolbarAction Pointer to vertical toolbar action that creates the widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        Widget(QWidget* parent, VerticalToolbarAction* verticalToolbarAction, const std::int32_t& widgetFlags);

        /**
         * Respond to \p target object events
         * @param target Object of which an event occurred
         * @param event The event that took place
         */
        bool eventFilter(QObject* target, QEvent* event) override;

    private:

        void setActionWidgets();
        void updateLayout();

    protected:
        VerticalToolbarAction* _verticalToolbarAction;      /** Pointer to vertical toolbar action that creates the widget */
        QHBoxLayout             _layout;                    /** Main layout */
        QHBoxLayout             _toolbarLayout;             /** Toolbar layout */
        QWidget                 _toolbarWidget;             /** Toolbar widget */
        QTimer                  _timer;                     /** Timer to periodically update the layout */

        friend class VerticalToolbarAction;
    };

protected: // Widgets

    /**
     * Get widget representation of the vertical toolbar action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this, widgetFlags);
    }

public:

    /**
     * Construct with \p parent object and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param alignment Item alignment
     */
    Q_INVOKABLE VerticalToolbarAction(QObject* parent, const QString& title, const Qt::AlignmentFlag& alignment = Qt::AlignmentFlag::AlignLeft);
};

}

Q_DECLARE_METATYPE(hdps::gui::VerticalToolbarAction)

inline const auto verticalToolbarActionActionMetaTypeId = qRegisterMetaType<hdps::gui::VerticalToolbarAction*>("hdps::gui::VerticalToolbarAction");