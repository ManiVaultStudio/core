#include "ToolbarAction.h"

#include <QCoreApplication>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

namespace hdps {

namespace gui {

ToolbarAction::ToolbarAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _items()
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Horizontal);
}

void ToolbarAction::addAction(WidgetAction* action, std::int32_t priority /*= 1*/)
{
    _items << Item(action, priority);

    std::sort(_items.begin(), _items.end());
}

ToolbarAction::HorizontalWidget::HorizontalWidget(QWidget* parent, ToolbarAction* toolbarAction) :
    QWidget(parent),
    _toolbarAction(toolbarAction),
    _resizeTimer(),
    _mainLayout(),
    _toolbarLayout(),
    _toolbarWidget(),
    _offScreenWidget(),
    _offScreenLayout(),
    _statefulItems()
{
    // Set resize timer interval and only timeout once
    _resizeTimer.setInterval(RESIZE_TIMER_INTERVAL);
    _resizeTimer.setSingleShot(true);

    // Handle resize timer timeout
    connect(&_resizeTimer, &QTimer::timeout, this, &HorizontalWidget::computeLayout);

    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);

    _offScreenLayout.setMargin(0);

    // Create stateful item for each toolbar action
    for (auto& item : _toolbarAction->_items)
        _statefulItems << SharedStatefulItem::create(this, _toolbarAction->_items.indexOf(item), item.getAction(), &_toolbarLayout, &_offScreenLayout);

    // Apply toolbar layout to toolbar widget
    _offScreenWidget.setLayout(&_offScreenLayout);

    _toolbarWidget.setLayout(&_toolbarLayout);

    // Add toolbar widget to main layout
    _mainLayout.addWidget(&_toolbarWidget);
    _mainLayout.addStretch(1);

    // Apply main layout
    setLayout(&_mainLayout);

    // Configure main layout
    _mainLayout.setMargin(0);

    _offScreenWidget.show();
}

void ToolbarAction::HorizontalWidget::resizeEvent(QResizeEvent* resizeEvent)
{
    // Prevent resize timer timeout
    if (_resizeTimer.isActive())
        _resizeTimer.stop();

    // Start resize timer
    _resizeTimer.start();
}

void ToolbarAction::HorizontalWidget::computeLayout()
{
    qDebug() << "Compute toolbar widget layout" << width();

    QVector<StatefulItem::ItemState> itemStates;

    itemStates.resize(_toolbarAction->_items.count());

    // Initialize collapsed
    std::fill(itemStates.begin(), itemStates.end(), StatefulItem::Collapsed);

    const auto getWidth = [this, &itemStates]() -> std::int32_t {
        std::int32_t width = (_toolbarAction->_items.count() - 1) * _toolbarLayout.spacing();

        for (auto statefulItem : _statefulItems)
            width += statefulItem->getWidth(itemStates[statefulItem->getIndex()]);

        return width;
    };

    // Establish stateful item states
    for (auto statefulItem : _statefulItems) {
        auto cachedItemStates = itemStates;

        itemStates[statefulItem->getIndex()] = StatefulItem::Standard;

        if (getWidth() > width()) {
            itemStates = cachedItemStates;
            break;
        }
    }

    // Assign new state(s)
    for (auto statefulItem : _statefulItems)
        statefulItem->setState(itemStates[statefulItem->getIndex()]);

    qDebug() << itemStates;
}

QWidget* ToolbarAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::Horizontal)
        layout->addWidget(new ToolbarAction::HorizontalWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

ToolbarAction::Item::Item(WidgetAction* action, std::int32_t priority) :
    _action(action),
    _priority(priority),
    _widths{0, 0}
{
}

WidgetAction* ToolbarAction::Item::getAction()
{
    return _action;
}

ToolbarAction::HorizontalWidget::StatefulItem::StatefulItem(QWidget* parent, std::int32_t index, WidgetAction* action, QHBoxLayout* targetLayout, QHBoxLayout* offscreenLayout) :
    _index(index),
    _action(action),
    _state(Collapsed),
    _collapsedWidget(nullptr),
    _standardWidget(nullptr),
    _targetLayout(targetLayout),
    _offscreenLayout(offscreenLayout)
{
    Q_ASSERT(_action != nullptr);
    Q_ASSERT(_targetLayout != nullptr);
    Q_ASSERT(_offscreenLayout != nullptr);

    _collapsedWidget    = _action->createCollapsedWidget(parent);
    _standardWidget     = _action->createWidget(parent);

    _offscreenLayout->addWidget(_collapsedWidget);
    _offscreenLayout->addWidget(_standardWidget);
}

std::int32_t ToolbarAction::HorizontalWidget::StatefulItem::getIndex() const
{
    return _index;
}

void ToolbarAction::HorizontalWidget::StatefulItem::setState(const ItemState& state)
{
    if (state == _state)
        return;

    _state = state;

    switch (state)
    {
        case Collapsed:
        {
            _targetLayout->insertWidget(_index, _collapsedWidget);
            _offscreenLayout->addWidget(_standardWidget);

            break;
        }

        case Standard:
        {
            _targetLayout->insertWidget(_index, _standardWidget);
            _offscreenLayout->addWidget(_collapsedWidget);

            break;
        }

        default:
            break;
    }
}

QWidget* ToolbarAction::HorizontalWidget::StatefulItem::getWidget(const ItemState& itemState)
{
    switch (itemState)
    {
        case Collapsed:
            return _collapsedWidget;

        case Standard:
            return _standardWidget;
    }

    return nullptr;
}

std::int32_t ToolbarAction::HorizontalWidget::StatefulItem::getWidth() const
{
    return const_cast<StatefulItem*>(this)->getWidget(_state)->sizeHint().width();
}

std::int32_t ToolbarAction::HorizontalWidget::StatefulItem::getWidth(const ItemState& state) const
{
    return const_cast<StatefulItem*>(this)->getWidget(state)->sizeHint().width();
}

}
}

/*
    const auto fadeItemWidget = [this](QWidget* widget, bool fadeIn, std::function<void()>& finished) {

        // Create opacity effect for fade in/out
        auto opacityffect = new QGraphicsOpacityEffect(widget);

        // Apply graphics effect to widget
        widget->setGraphicsEffect(opacityffect);

        auto fadeOutAnimation = new QPropertyAnimation(opacityffect, "opacity");

        fadeOutAnimation->setDuration(500);
        fadeOutAnimation->setStartValue(fadeIn ? 0 : 1);
        fadeOutAnimation->setEndValue(fadeIn ? 1 : 0);

        fadeOutAnimation->start();

        connect(fadeOutAnimation, &QPropertyAnimation::finished, this, [this, widget, opacityffect, fadeOutAnimation, &finished]() {
            delete opacityffect;
            delete fadeOutAnimation;

            finished();
        });
    };

    const auto setItemState = [this](std::int32_t itemIndex, QLayout* targetLayout) {

        // Get normal and collapsed widget
        auto itemCollapsedWidget    = getWidget(itemIndex, Collapsed);
        auto itemWidget             = getWidget(itemIndex, Standard);


    };

    const auto removeFromLayout = [this](QWidget* widget, QLayout* targetLayout) {
        auto fadeOutEffect = new QGraphicsOpacityEffect(widget);

        widget->setGraphicsEffect(fadeOutEffect);

        auto fadeOutAnimation = new QPropertyAnimation(fadeOutEffect, "opacity");

        fadeOutAnimation->setDuration(500);
        fadeOutAnimation->setStartValue(1);
        fadeOutAnimation->setEndValue(0);

        connect(fadeOutAnimation, &QPropertyAnimation::finished, this, [this, widget, fadeOutEffect, fadeOutAnimation, targetLayout]() {
            delete fadeOutEffect;
            delete fadeOutAnimation;

            targetLayout->addWidget(widget);
        });
    };

    for (std::int32_t itemIndex = 0; itemIndex < itemStates.count(); itemIndex++) {
        const auto itemState    = itemStates[itemIndex];
        const auto widgetIndex  = getWidgetIndex(itemIndex, itemState);
        const auto stateChanged = _itemStates.isEmpty() ? true : itemState != _itemStates[itemIndex];

        if (stateChanged) {
            switch (itemState)
            {
                case Collapsed:
                {
                    //addToLayout(getWidget(itemIndex, Standard), &_offScreenWidgetsLayout);
                    //removeFromLayout(getWidget(itemIndex, Standard), &_offScreenWidgetsLayout);

                    break;
                }

                case Standard:
                {
                    //removeFromLayout(getWidget(itemIndex, Collapsed), &_offScreenWidgetsLayout);


                    break;
                }

                default:
                    break;
            }
        }
    }
    */