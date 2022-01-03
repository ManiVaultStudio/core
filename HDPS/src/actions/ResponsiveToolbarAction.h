#pragma once

#include "WidgetAction.h"

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
 * Toolbar action class which adjust the visual representation of items based on the available screen space (width)
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
        Vertical    = 0x00002   /** Toolbar is vertically docked */
    };

public:

    /** Describes the item state configurations */
    enum class ItemState {
        Undefined,      /** Item state is not defined */
        Collapsed,      /** Item is in a collapsed state (accessible through a tool button) */
        Standard        /** Item is in a standard state */
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

    /** Toolbar widget for horizontal docking */
    class HorizontalWidget : public QWidget
    {
    protected:

        /**
         * Stateful item class
         *
         * Selectively shows either a standard or collapsed widget of an action
         *
         * @author Thomas Kroes
         */
        class StatefulItem : public QObject {

        public:
            /**
             * Constructor
             * @param parent Pointer to parent widget
             * @param index Index
             * @param item Reference to toolbar item
             */
            StatefulItem(QWidget* parent, std::int32_t index, Item& item);

            /**
             * Get item
             * @return Reference to toolbar item
             */
            Item& getItem();

            /**
             * Get index
             * @return Index
             */
            std::int32_t getIndex() const;

            /**
             * Get state
             * @return Item state
             */
            ItemState getState() const;

            /**
             * Set state
             * @parem state Item state
             */
            void setState(const ItemState& state);

            /**
             * Get widget
             * @return Pointer to stateful item widget
             */
            QWidget* getWidget();

            /**
             * Get widget
             * @param itemState State for which to fetch the widget
             * @return Pointer to stateful item widget
             */
            QWidget* getWidget(const ItemState& state);

            /**
             * Get width
             * @return Width of the stateful item widget
             */
            std::int32_t getWidth() const;

            /**
             * Get width
             * @param state State for which to fetch the width
             * @return Width
             */
            std::int32_t getWidth(const ItemState& state) const;

            /**
             * Get priority
             * @return Priority
             */
            std::int32_t getPriority() const;

            /**
             * Smaller than operator
             * @param other Stateful item to compare with
             * @return Whether the other item is smaller than ours (in terms of the visibility priority)
             */
            bool operator<(StatefulItem other) {
                return getPriority() < other.getPriority();
            }

        protected:
            std::int32_t        _index;             /** Position index in the toolbar */
            Item&               _item;              /** Reference to toolbar item */
            ItemState           _state;             /** State of the item */
            QWidget             _widget;            /** Main widget */
            QHBoxLayout         _widgetLayout;      /** Main widget layout */
            FadeableWidget      _collapsedWidget;   /** Fadeable collapsed widget */
            FadeableWidget      _standardWidget;    /** Fadeable standard widget */
            QVariantAnimation   _sizeAnimation;     /** Animation to control the size of the widget */
        };

        using SharedStatefulItem = QSharedPointer<StatefulItem>;

        /**
         * Spacer widget
         *
         * Widget class for visual separation of toolbar items
         *
         * @author Thomas Kroes
         */
        class SpacerWidget : public QWidget {
        public:

            /** Describes the spacer types */
            enum class Type {
                Undefined,      /** No spacer type defined */
                Divider,        /** Divide with a line */
                Spacer          /** Divide with a small space */
            };

        public:
            SpacerWidget(const Type& type = Type::Divider);

            /**
             * Get spacer type based on the state of the item left and right of the spacer
             * @param itemStateLeft State of the item on the left
             * @param itemStateRight State of the item on the right
             * @return Spacer type
             */
            static Type getType(const ItemState& itemStateLeft, const ItemState& itemStateRight);

            /**
             * Get spacer type based on the state of the item left and right of the spacer
             * @param statefulItemLeft Pointer to stateful item on the left
             * @param statefulItemRight Pointer to stateful item on the right
             * @return Spacer type
             */
            static Type getType(const StatefulItem* statefulItemLeft, const StatefulItem* statefulItemRight);

            /**
             * Set spacer type
             * @param type Type of spacer
             */
            void setType(const Type& type);

            /**
             * Get spacer width for the specified type
             * @param type Type of spacer
             * @return Width for spacer type
             */
            static std::int32_t getWidth(const Type& type);

        protected:
            Type                _type;              /** Spacer type */
            QHBoxLayout         _layout;            /** Main layout */
            QFrame              _verticalLine;      /** Vertical line */
            FadeableWidget      _fadeableWidget;    /** For fading in/out the vertical line */
            QVariantAnimation   _sizeAnimation;     /** For animating the size of the spacer */
        };

        using SharedSpacerWidget = QSharedPointer<SpacerWidget>;

    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param toolbarAction Pointer to toolbar action
         */
        HorizontalWidget(QWidget* parent, ToolbarAction* toolbarAction);

        /**
         * Invoked when the widget is resized
         * @param resizeEvent Pointer to resize event
         */
        void resizeEvent(QResizeEvent* resizeEvent) override;

    private:

        /** Computes the layout in response to a resizing of the widget */
        void computeLayout();

        /**
         * Set item states
         * @param itemStates States of the items
         */
        void setItemStates(const QVector<ItemState>& itemStates);

    protected:
        ToolbarAction*                  _toolbarAction;     /** Pointer to toolbar action */
        QTimer                          _resizeTimer;       /** Timer which prevents unnecessary resize handler calls */
        QHBoxLayout                     _mainLayout;        /** Horizontal main layout */
        QHBoxLayout                     _toolbarLayout;     /** Horizontal toolbar layout for items */
        QWidget                         _toolbarWidget;     /** Toolbar widget */
        QVector<SharedStatefulItem>     _statefulItems;     /** All stateful items */
        QVector<SharedSpacerWidget>     _spacerWidgets;     /** All spacer widgets */

        static constexpr std::int32_t RESIZE_TIMER_INTERVAL = 50;       /** Default resize timer interval */
        static constexpr std::int32_t ANIMATION_DURATION    = 500;      /** Animation duration */

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

protected:
    QVector<Item>  _items;          /** Toolbar items */

    friend class HorizontalWidget;
};

}
}
