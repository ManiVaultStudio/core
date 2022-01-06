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
 * Toolbar action class which selectively collapses items based on the available screen space
 *
 * @author Thomas Kroes
 */
class ResponsiveToolbarAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        Horizontal  = 0x00001,  /** Toolbar is horizontally docked */
        Vertical    = 0x00002,  /** Toolbar is vertically docked */

        Default = Horizontal
    };

public:

    /** Describes the item state configurations */
    enum class ItemState {
        Undefined,      /** Item state is not defined */
        Collapsed,      /** Item is in a collapsed state (accessible through a tool button) */
        Standard,       /** Item is in a standard state */
        Hidden          /** Item is hidden */
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

            friend class ResponsiveToolbarAction;
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
        HiddenItemsAction(ResponsiveToolbarAction& responsiveToolbarAction);

        /**
         * Get responsive toolbar action
         * @return Reference to responsive toolbar action
         */
        ResponsiveToolbarAction& getResponsiveToolbarAction();

    protected:
        ResponsiveToolbarAction&    _responsiveToolbarAction;   /** Reference to responsive toolbar action */

        friend class ResponsiveToolbarAction;
    };

    /**
     * Toolbar widget class
     *
     * Toolbar widget base class for responsive toolbar action
     *
     * @author Thomas Kroes
     */
    class ToolbarWidget : public QWidget
    {
    public:

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
             * @param toolbarWidget Pointer to toolbar widget
             * @param index Index
             * @param action Reference to action
             * @param action Reference to action
             */
            StatefulItem(ToolbarWidget* toolbarWidget, std::int32_t index, WidgetAction& action, std::int32_t priority);

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
             * Respond to target object events
             * @param target Object of which an event occurred
             * @param event The event that took place
             */
            bool eventFilter(QObject* target, QEvent* event) override;

            /**
             * Smaller than operator
             * @param other Stateful item to compare with
             * @return Whether the other item is smaller than ours (in terms of the visibility priority)
             */
            bool operator<(StatefulItem other) {
                return getPriority() < other.getPriority();
            }

            /** Show the item */
            void show();

            /** Hide the item */
            void hide();

            /**
             * Set visibility
             * @param visible Visibility
             */
            void setVisibility(bool visible);

        protected:
            ToolbarWidget*      _toolbarWidget;         /** Pointer to owning toolbar widget */
            std::int32_t        _index;                 /** Position index in the toolbar */
            WidgetAction&       _action;                /** Reference to action */
            std::int32_t        _priority;              /** Visibility priority */
            ItemState           _state;                 /** State of the item */
            QWidget             _widget;                /** Main widget */
            QHBoxLayout         _widgetLayout;          /** Main widget layout */
            FadeableWidget      _collapsedWidget;       /** Fadeable collapsed widget */
            FadeableWidget      _standardWidget;        /** Fadeable standard widget */
            QVariantAnimation   _sizeAnimation;         /** Animation to control the size of the widget */
        };

        using SharedStatefulItem = QSharedPointer<ToolbarWidget::StatefulItem>;

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
                Spacer,         /** Divide with a small space */
                Hidden          /** Spacer is hidden */
            };

        public:

            /**
             * Constructor
             * @param toolbarWidget Pointer to toolbar widget
             * @param type Spacer type
             */
            SpacerWidget(ToolbarWidget* toolbarWidget, const Type& type = Type::Divider);

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
            ToolbarWidget*      _toolbarWidget;         /** Reference to owning horizontal widget */
            Type                _type;                  /** Spacer type */
            QHBoxLayout         _layout;                /** Main layout */
            QFrame              _verticalLine;          /** Vertical line */
            FadeableWidget      _fadeableWidget;        /** For fading in/out the vertical line */
            QVariantAnimation   _sizeAnimation;         /** For animating the size of the spacer */
        };

        using SharedSpacerWidget = QSharedPointer<ToolbarWidget::SpacerWidget>;

    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param responsiveToolbarAction Reference to responsive toolbar action
         */
        ToolbarWidget(QWidget* parent, ResponsiveToolbarAction& responsiveToolbarAction) :
            QWidget(parent),
            _responsiveToolbarAction(responsiveToolbarAction),
            _statefulItems(),
            _spacerWidgets()
        {
        }

    public:

        /**
         * Get whether animations are enabled or not
         * @return Whether animations are enabled or not
         */
        bool getEnableAnimation() const
        {
            return _responsiveToolbarAction.getEnableAnimation();
        }

        /**
         * Computes the layout of all items (decides which items are collapsed/standard)
         * @param statefulItem Pointer to stateful item
         */
        virtual void computeLayout(StatefulItem* statefulItem = nullptr)
        {
        }

        /**
         * Get stateful items
         * @return Stateful items
         */
        QVector<ToolbarWidget::SharedStatefulItem> getStatefulItems()
        {
            return _statefulItems;
        }

    protected:
        ResponsiveToolbarAction&        _responsiveToolbarAction;   /** Reference to responsive toolbar action */
        QVector<SharedStatefulItem>     _statefulItems;             /** All stateful items */
        QVector<SharedSpacerWidget>     _spacerWidgets;             /** All spacer widgets */
    };

    /**
     * Horizontal widget class
     *
     * Horizontal toolbar widget for responsive toolbar action
     *
     * @author Thomas Kroes
     */
    class HorizontalWidget : public ToolbarWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param responsiveToolbarAction Reference to toolbar action
         */
        HorizontalWidget(QWidget* parent, ResponsiveToolbarAction& responsiveToolbarAction);

        /**
         * Invoked when the widget is resized
         * @param resizeEvent Pointer to resize event
         */
        void resizeEvent(QResizeEvent* resizeEvent) override;

        /**
         * Computes the layout of all items (decides which items are collapsed/standard)
         * @param statefulItem Pointer to stateful item
         */
        void computeLayout(StatefulItem* statefulItem = nullptr) override;

        /**
         * Set item states
         * @param itemStates States of the items
         */
        void setItemStates(const QVector<ItemState>& itemStates);

    protected:
        QTimer                          _resizeTimer;               /** Timer which prevents unnecessary resize handler calls */
        QHBoxLayout                     _mainLayout;                /** Horizontal main layout */
        QHBoxLayout                     _toolbarLayout;             /** Horizontal toolbar layout for items */
        QWidget                         _toolbarWidget;             /** Toolbar widget */

        friend class ResponsiveToolbarAction;
        friend class StatefulItem;
    };

    /**
     * Vertical widget class
     *
     * Vertical toolbar widget for responsive toolbar action
     *
     * @author Thomas Kroes
     */
    class VerticalWidget : public ToolbarWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param responsiveToolbarAction Reference to toolbar action
         */
        VerticalWidget(QWidget* parent, ResponsiveToolbarAction& responsiveToolbarAction);

    protected:
        QVBoxLayout     _layout;        /** Main vertical layout */

        friend class ResponsiveToolbarAction;
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
    ResponsiveToolbarAction(QObject* parent, const QString& title = "");

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

    static constexpr std::int32_t RESIZE_TIMER_INTERVAL = 50;       /** Default resize timer interval */
    static constexpr std::int32_t ANIMATION_DURATION    = 300;      /** Animation duration */
};

}
}
