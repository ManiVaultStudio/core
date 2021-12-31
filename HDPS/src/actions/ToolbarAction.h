#pragma once

#include "WidgetAction.h"

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

            /** Describes the item state configurations */
            enum ItemState {
                None,
                Collapsed,
                Standard
            };

        public:
            class FadeableWidget : public QWidget {
            public:
                FadeableWidget(QWidget* parent, QWidget* target) :
                    QWidget(parent),
                    _target(target),
                    _layout(),
                    _opacityEffect(),
                    _opacityAnimation(&_opacityEffect, "opacity")
                {
                    Q_ASSERT(parent != nullptr);
                    Q_ASSERT(_target != nullptr);

                    _target->setObjectName("TargetWidget");

                    //setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

                    parent->installEventFilter(this);

                    _layout.setMargin(0);
                    _layout.setSizeConstraint(QLayout::SetFixedSize);
                    _layout.addWidget(target);
                    //_layout.addStretch(1);

                    setLayout(&_layout);

                    _target->setGraphicsEffect(&_opacityEffect);
                    //_target->setAttribute(Qt::WA_TranslucentBackground, true);
                    
                    _opacityEffect.setOpacity(0);

                    _opacityAnimation.setDuration(ANIMATION_DURATION);
                    _opacityAnimation.setEasingCurve(QEasingCurve::InOutQuad);
                }

                void setOpacity(float opacity)
                {
                    _opacityEffect.setOpacity(opacity);
                }

                void fadeIn(std::int32_t delay = 0, std::function<void()> finished = std::function<void()>())
                {
                    _opacityAnimation.setStartValue(0.0f);
                    _opacityAnimation.setEndValue(1.0f);

                    if (finished)
                        connect(&_opacityAnimation, &QPropertyAnimation::finished, finished);

                    connect(&_opacityAnimation, &QPropertyAnimation::valueChanged, _target, &QWidget::updateGeometry);

                    QTimer::singleShot(delay, [this]() {
                        _opacityAnimation.start();
                    });
                }

                void fadeOut(std::int32_t delay = 0, std::function<void()> finished = std::function<void()>())
                {
                    _opacityAnimation.setStartValue(1.0f);
                    _opacityAnimation.setEndValue(0.0f);

                    if (finished)
                        connect(&_opacityAnimation, &QPropertyAnimation::finished, finished);

                    connect(&_opacityAnimation, &QPropertyAnimation::valueChanged, _target, &QWidget::updateGeometry);

                    QTimer::singleShot(delay, [this]() {
                        _opacityAnimation.start();
                    });
                }

                bool eventFilter(QObject* target, QEvent* event) override
                {
                    switch (event->type())
                    {
                        case QEvent::Resize:
                        {
                            update();
                            //_target->setFixedSize(static_cast<QResizeEvent*>(event)->size());

                            break;
                        }

                        case QEvent::Move:
                        {
                            update();
                            //move(static_cast<QMoveEvent*>(event)->pos());
                            //move(100, 0);
                            updateGeometry();
                            break;
                        }

                        default:
                            break;
                    }

                    return QObject::eventFilter(target, event);
                }

            protected:
                QWidget*                _target;
                QHBoxLayout             _layout;
                QGraphicsOpacityEffect  _opacityEffect;
                QPropertyAnimation      _opacityAnimation;             /**  */
            };

        public:
            StatefulItem(QWidget* parent, std::int32_t index, Item& item);

            Item& getItem();

            std::int32_t getIndex() const;

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
            void swapWidget(const ItemState& stateA, const ItemState& stateB);

        protected:
            std::int32_t        _index;                     /**  */
            Item&               _item;                      /**  */
            ItemState           _state;                     /**  */
            QWidget             _widget;                    /**  */
            QHBoxLayout         _widgetLayout;              /**  */
            FadeableWidget      _collapsedWidget;           /**  */
            FadeableWidget      _standardWidget;            /**  */
            QVariantAnimation   _sizeAnimation;             /**  */

            /** Animation duration */
            static constexpr std::int32_t ANIMATION_DURATION = 350;
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
        QVector<SharedStatefulItem>     _statefulItems;                         

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
