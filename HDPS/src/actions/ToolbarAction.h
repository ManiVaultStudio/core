#pragma once

#include "WidgetAction.h"

#include "widgets/FadeableWidget.h"

#include <QTimer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QMoveEvent>

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

public:

    /** Describes the item state configurations */
    enum class ItemState {
        Undefined,
        Collapsed,
        Standard
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
        class StatefulItem : public QObject {

        public:
            StatefulItem(QWidget* parent, std::int32_t index, Item& item);

            Item& getItem();

            std::int32_t getIndex() const;

            ItemState getState() const;

            void setState(const ItemState& state);

            QWidget* getWidget();

            QWidget* getWidget(const ItemState& itemState);

            std::int32_t getWidth() const;

            std::int32_t getWidth(const ItemState& state) const;

            std::int32_t getPriority() const;

            bool operator<(StatefulItem other) {
                return getPriority() < other.getPriority();
            }

        protected:
            std::int32_t        _index;             /**  */
            Item&               _item;              /**  */
            ItemState           _state;             /**  */
            QWidget             _widget;            /**  */
            QHBoxLayout         _widgetLayout;      /**  */
            FadeableWidget      _collapsedWidget;   /**  */
            FadeableWidget      _standardWidget;    /**  */
            QVariantAnimation   _sizeAnimation;     /**  */
        };

        using SharedStatefulItem = QSharedPointer<StatefulItem>;

        class SpacerWidget : public QWidget {
        public:
            enum class Type {
                Undefined,
                Divider,
                Spacer
            };

        public:
            SpacerWidget(const Type& type = Type::Divider) :
                QWidget(),
                _type(Type::Undefined),
                _layout(),
                _verticalLine(),
                _fadeableWidget(this, &_verticalLine),
                _sizeAnimation()
            {
                _verticalLine.setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
                _verticalLine.setFrameShape(QFrame::VLine);
                _verticalLine.setFrameShadow(QFrame::Sunken);

                _layout.setMargin(0);
                _layout.setSpacing(2);
                _layout.setAlignment(Qt::AlignCenter);
                _layout.addWidget(&_verticalLine);

                setLayout(&_layout);

                _fadeableWidget.setOpacity(0.0f);

                _sizeAnimation.setEasingCurve(QEasingCurve::InOutQuad);
            }

            static Type getType(const ItemState& itemStateLeft, const ItemState& itemStateRight)
            {
                return itemStateLeft == ItemState::Collapsed && itemStateRight == ItemState::Collapsed ? Type::Spacer : Type::Divider;
            }

            static Type getType(const StatefulItem* stateWidgetLeft, const StatefulItem* stateWidgetRight)
            {
                return getType(stateWidgetLeft->getState(), stateWidgetRight->getState());
            }

            void setType(const Type& type)
            {
                if (type == _type)
                    return;

                switch (type)
                {
                    case Type::Divider:
                    {
                        _fadeableWidget.fadeIn(ANIMATION_DURATION, 0);

                        _sizeAnimation.setStartValue(getWidth(Type::Spacer));
                        _sizeAnimation.setEndValue(getWidth(Type::Divider));

                        break;
                    }

                    case Type::Spacer:
                    {
                        if (_type != Type::Undefined)
                            _fadeableWidget.fadeOut(ANIMATION_DURATION / 2, 0);

                        _sizeAnimation.setStartValue(getWidth(Type::Divider));
                        _sizeAnimation.setEndValue(getWidth(Type::Spacer));

                        break;
                    }

                    default:
                        break;
                }

                connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this](const QVariant& value) {
                    setFixedWidth(value.toFloat());
                });

                _sizeAnimation.setDuration(_type == Type::Undefined ? 0 : ANIMATION_DURATION);
                _sizeAnimation.start();

                _type = type;
            }

            static std::int32_t getWidth(const Type& type)
            {
                switch (type)
                {
                    case Type::Undefined:       return 0;
                    case Type::Divider:         return 20;
                    case Type::Spacer:          return 4;
                }

                return 0;
            }

        protected:
            Type                _type;              /**  */
            QHBoxLayout         _layout;            /**  */
            QFrame              _verticalLine;      /**  */
            FadeableWidget      _fadeableWidget;    /**  */
            QVariantAnimation   _sizeAnimation;     /**  */
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
        QVector<SharedStatefulItem>     _statefulItems;     
        QVector<SharedSpacerWidget>     _spacerWidgets;     /**  */

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

    void addAction(WidgetAction* action, std::int32_t priority = 1);

signals:

protected:
    QVector<Item>  _items;

    friend class HorizontalWidget;
};

}
}
