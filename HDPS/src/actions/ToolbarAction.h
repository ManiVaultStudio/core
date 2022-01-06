#pragma once

#include "WidgetAction.h"
#include "ToolbarWidget.h"

#include "widgets/FadeableWidget.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

namespace hdps {

namespace gui {

/**
 * Responsive toolbar action class
 *
 * Toolbar action class which selectively collapses items based on the available screen space
 *
 * @author Thomas Kroes
 */
class ToolbarAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Horizontal  = 0x00001,  /** Toolbar is horizontally docked */
        Vertical    = 0x00002,  /** Toolbar is vertically docked */

        Default = Horizontal
    };

protected:

    /**
     * Toolbar item
     *
     * Item with visibility priority
     *
     * @author Thomas Kroes
     */
    class Item
    {
    public:

        /**
         * Constructor
         * @param action Pointer to item action
         * @param priority Visibility priority
         */
        Item(WidgetAction* action, std::int32_t priority);

        /**
         * Equality operator
         * @param other Item to compare to
         * @return Whether the other item matches our item
         */
        bool operator==(Item other) {
            return _action == other._action;
        }

        /** Get associated action */
        WidgetAction* getAction();

        /** Get visibility priority */
        std::int32_t getPriority() const;

    protected:
        WidgetAction*   _action;        /** Pointer to associated action */
        std::int32_t    _priority;      /** Visibility priority */
    };

public:

    /**
     * Hidden items action class
     *
     * Shows hidden items in a popup
     *
     * @author Thomas Kroes
     */
    class HiddenItemsAction : public WidgetAction
    {
    protected:

        /** Widget for hidden items action */
        class Widget : public WidgetActionWidget
        {
        protected:

            /**
             * Constructor
             * @param parent Pointer to parent widget
             * @param hiddenItemsAction Pointer to hidden items action
             * @param widgetFlags Widget flags for the configuration of the widget (type)
             */
            Widget(QWidget* parent, HiddenItemsAction* hiddenItemsAction, const std::int32_t& widgetFlags);

        protected:
            HiddenItemsAction*      _hiddenItemsAction;     /** Pointer to hidden items action */
            QHBoxLayout             _layout;                /** Main vertical layout */

            friend class ToolbarAction;
        };

    protected:

        /**
         * Get widget representation of the hidden items action
         * @param parent Pointer to parent widget
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
            return new Widget(parent, this, widgetFlags);
        };

    protected:

        /**
         * Constructor
         * @param responsiveToolbarAction Reference to responsive toolbar action
         */
        HiddenItemsAction(ToolbarAction& responsiveToolbarAction);

        /**
         * Get responsive toolbar action
         * @return Reference to responsive toolbar action
         */
        ToolbarAction& getResponsiveToolbarAction();

    protected:
        ToolbarAction&    _responsiveToolbarAction;   /** Reference to responsive toolbar action */

        friend class ToolbarAction;
    };

protected:

    /**
     * Get widget representation of the string action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    ToolbarAction(QObject* parent, const QString& title = "");

    /**
     * Add widget action to the responsive toolbar
     * @param action Pointer to widget action to add
     * @param priority Visibility of the added action (smaller means the item is collapsed more likely when there is limited toolbar space)
     */
    void addAction(WidgetAction* action, std::int32_t priority = 1);

    /**
     * Get whether animations are enabled or not
     * @return Whether animations are enabled or not
     */
    bool getEnableAnimation() const;

    /**
     * Set whether animations are enabled or not
     * @param enableAnimation Whether animations are enabled or not
     */
    void setEnableAnimation(bool enableAnimation);

    /**
     * Get hidden items action
     * @return Reference to hidden items action
     */
    HiddenItemsAction& getHiddenItemsAction();

protected:
    QVector<Item>       _items;                 /** Toolbar items */
    bool                _enableAnimation;       /** Whether animations are enabled or not */
    HiddenItemsAction   _hiddenItemsAction;     /** Hidden items action */
};

}
}
