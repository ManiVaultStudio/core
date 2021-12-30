#pragma once

#include "WidgetAction.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>

class QWidget;

namespace hdps {

namespace gui {

/**
 * Toolbar action class
 *
 * Action-aware toolbar
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

protected:

    class Item
    {
    public:
        Item(WidgetAction* action, std::int32_t priority);

        bool operator==(Item other) {
            return _action == other._action;
        }

        WidgetAction* getAction();
        std::int32_t getPriority() const;

    protected:
        WidgetAction*   _action;
        std::int32_t    _priority;
    };

public:

    /** Toolbar widget for horizontal docking */
    class HorizontalWidget : public QWidget
    {
    protected:

        /**
         * 
         */
        class StatefulItem {
        public:

            /** Describes the item state configurations */
            enum ItemState {
                None,
                Collapsed,
                Standard
            };

        public:
            StatefulItem(QWidget* parent, std::int32_t index, Item& item, QHBoxLayout* targetLayout, QHBoxLayout* offscreenLayout);

            Item& getItem();

            std::int32_t getIndex() const;

            void setState(const ItemState& state);

            QWidget* getWidget(const ItemState& itemState);

            std::int32_t getWidth() const;

            std::int32_t getWidth(const ItemState& state) const;

            std::int32_t getPriority() const;

            bool operator<(StatefulItem other) {
                return getPriority() < other.getPriority();
            }

        protected:

            void insertWidget(QWidget* widget, std::function<void()> finished = std::function<void()>());
            void removeWidget(QWidget* widget, std::function<void()> finished = std::function<void()>());

        protected:
            std::int32_t    _index;                 /**  */
            Item&           _item;                  /**  */
            ItemState       _state;                 /**  */
            QWidget*        _collapsedWidget;       /**  */
            QWidget*        _standardWidget;        /**  */
            QHBoxLayout*    _targetLayout;          /**  */
            QHBoxLayout*    _offscreenLayout;       /**  */

            /** Animation duration */
            static constexpr std::int32_t ANIMATION_DURATION = 250;
        };

        using SharedStatefulItem = QSharedPointer<StatefulItem>;

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

        /** Get preferred size */
        QSize sizeHint() const override {
            return QSize(0, 10);
        }

        /** Get minimum size hint */
        QSize minimumSizeHint() const override {
            return sizeHint();
        }

    private:

        /** Computes the layout in response to a resizing of the widget */
        void computeLayout();

    protected:
        ToolbarAction*                  _toolbarAction;                         /** Pointer to toolbar action */
        QTimer                          _resizeTimer;                           /** Timer which prevents unnecessary resize handler calls */
        QHBoxLayout                     _mainLayout;                            /** Horizontal main layout */
        QHBoxLayout                     _toolbarLayout;                         /** Horizontal toolbar layout for items */
        QWidget                         _toolbarWidget;                         /** Toolbar widget */

        QWidget                         _offScreenWidget;                       /** Offscreen widget */
        QHBoxLayout                     _offScreenLayout;                       /** Offscreen layout */

        QVector<SharedStatefulItem>     _statefulItems;                         

        /** Default resize timer interval */
        static constexpr std::int32_t RESIZE_TIMER_INTERVAL = 25;

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

    void addAction(WidgetAction* action, std::int32_t priority = 1);

signals:

protected:
    QVector<Item>  _items;

    friend class HorizontalWidget;
};

}
}
