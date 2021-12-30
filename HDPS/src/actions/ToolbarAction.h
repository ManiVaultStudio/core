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

    /** Describes the item state configurations */
    enum ItemState {
        Collapsed,
        Standard
    };

public:

    /** Toolbar widget for horizontal docking */
    class HorizontalWidget : public QWidget
    {
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

        std::int32_t getWidgetIndex(std::int32_t itemIndex, const ItemState& itemState);

        QWidget* getWidget(std::int32_t itemIndex, const ItemState& itemState);

        std::int32_t getWidgetWidth(std::int32_t itemIndex, const ItemState& itemState) const;

    protected:
        ToolbarAction*      _toolbarAction;                         /** Pointer to toolbar action */
        QTimer              _resizeTimer;                           /** Timer which prevents unnecessary resize handler calls */
        QHBoxLayout         _mainLayout;                            /** Horizontal main layout */
        QHBoxLayout         _toolbarLayout;                         /** Horizontal toolbar layout for items */
        QWidget             _toolbarWidget;                         /** Toolbar widget */

        QWidget             _offScreenWidget;                       /** Offscreen widget */
        QVBoxLayout         _offScreenLayout;                       /** Offscreen layout */
        QHBoxLayout         _offScreenWidgetsLayout;                /** Offscreen widgets layout */
        QHBoxLayout         _offScreenCollapsedWidgetsLayout;       /** Offscreen collapsed widgets layout */

        QList<QWidget*>     _widgets;
        QVector<ItemState>  _itemStates;                    

        /** Default resize timer interval */
        static constexpr std::int32_t RESIZE_TIMER_INTERVAL = 50;

        friend class ToolbarAction;
    };

protected:

    /**
     * Get widget representation of the string action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;


protected:

    class Item
    {
        public:
            Item(WidgetAction* action, std::int32_t priority);

            bool operator<(Item other) {
                return _priority < other._priority;
            }

            WidgetAction* getAction();

        protected:
            WidgetAction*   _action;
            std::int32_t    _priority;
            std::uint32_t   _widths[2];
    };

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
    QList<Item>  _items;

    friend class HorizontalWidget;
};

}
}
