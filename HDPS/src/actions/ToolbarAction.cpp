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
        _statefulItems << SharedStatefulItem::create(this, _toolbarAction->_items.indexOf(item), item, &_toolbarLayout, &_offScreenLayout);

    for (auto sortedStatefulItem : _statefulItems)
        qDebug() << "index" << sortedStatefulItem->getIndex() << sortedStatefulItem->getPriority();

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

    // Compute candidate configuration width
    const auto getWidth = [this, &itemStates]() -> std::int32_t {
        std::int32_t width = (_toolbarAction->_items.count() - 1) * _toolbarLayout.spacing();

        for (auto statefulItem : _statefulItems)
            width += statefulItem->getWidth(itemStates[statefulItem->getIndex()]);

        return width;
    };

    auto sortedStatefulItems = _statefulItems;

    // Sort stateful items in descending order
    std::sort(sortedStatefulItems.begin(), sortedStatefulItems.end(), [](auto a, auto b) {
        return a->getPriority() > b->getPriority();
    });

    for (auto sortedStatefulItem : sortedStatefulItems)
        qDebug() << "index" << sortedStatefulItem->getIndex() << sortedStatefulItem->getPriority();

    // Establish stateful item states
    for (auto sortedStatefulItem : sortedStatefulItems) {
        auto cachedItemStates = itemStates;

        itemStates[sortedStatefulItem->getIndex()] = StatefulItem::Standard;
        
        if (getWidth() > width()) {
            itemStates = cachedItemStates;
            break;
        }
    }

    // Assign new state(s)
    for (auto statefulItem : _statefulItems)
        statefulItem->setState(itemStates[statefulItem->getIndex()]);
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
    _priority(priority)
{
}

WidgetAction* ToolbarAction::Item::getAction()
{
    return _action;
}

std::int32_t ToolbarAction::Item::getPriority() const
{
    return _priority;
}

ToolbarAction::HorizontalWidget::StatefulItem::StatefulItem(QWidget* parent, std::int32_t index, Item& item, QHBoxLayout* targetLayout, QHBoxLayout* offscreenLayout) :
    _index(index),
    _item(item),
    _state(None),
    _collapsedWidget(nullptr),
    _standardWidget(nullptr),
    _targetLayout(targetLayout),
    _offscreenLayout(offscreenLayout)
{
    Q_ASSERT(_targetLayout != nullptr);
    Q_ASSERT(_offscreenLayout != nullptr);

    _collapsedWidget = _item.getAction()->createCollapsedWidget(parent);
    _standardWidget = _item.getAction()->createWidget(parent);

    _offscreenLayout->addWidget(_collapsedWidget);
    _offscreenLayout->addWidget(_standardWidget);
}

ToolbarAction::Item& ToolbarAction::HorizontalWidget::StatefulItem::getItem()
{
    return _item;
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
            removeWidget(_standardWidget, [this]() {
                insertWidget(_collapsedWidget);
            });

            break;
        }

        case Standard:
        {
            removeWidget(_collapsedWidget, [this]() {
                insertWidget(_standardWidget);
            });

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

std::int32_t ToolbarAction::HorizontalWidget::StatefulItem::getPriority() const
{
    return _item.getPriority();
}

void ToolbarAction::HorizontalWidget::StatefulItem::insertWidget(QWidget* widget, std::function<void()> finished /*= std::function<void()>()*/)
{
    // Create opacity effect for fade in/out
    auto opacityffect = new QGraphicsOpacityEffect(widget);

    // Set initial opacity
    opacityffect->setOpacity(0);

    // Apply graphics effect to widget
    widget->setGraphicsEffect(opacityffect);

    // Create an animation which controls the widget opacity
    auto animation = new QPropertyAnimation(opacityffect, "opacity");

    // Configure animation
    animation->setDuration(ANIMATION_DURATION);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    // Insert widget into target layout
    _targetLayout->insertWidget(_index, widget);

    animation->start();

    // Cleanup when finished
    connect(animation, &QPropertyAnimation::finished, [this, widget, opacityffect, animation, finished]() {
        delete opacityffect;
        delete animation;

        if (finished)
            finished();
    });
}

void ToolbarAction::HorizontalWidget::StatefulItem::removeWidget(QWidget* widget, std::function<void()> finished /*= std::function<void()>()*/)
{
    // Create opacity effect for fade in/out
    auto opacityffect = new QGraphicsOpacityEffect(widget);

    // Set initial opacity
    opacityffect->setOpacity(1);

    // Apply graphics effect to widget
    widget->setGraphicsEffect(opacityffect);

    // Create an animation which controls the widget opacity
    auto animation = new QPropertyAnimation(opacityffect, "opacity");

    // Configure animation
    animation->setDuration(ANIMATION_DURATION);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->setEasingCurve(QEasingCurve::InOutQuad);

    animation->start();

    // Cleanup when finished
    connect(animation, &QPropertyAnimation::finished, [this, widget, opacityffect, animation, finished]() {
        delete opacityffect;
        delete animation;

        // Remove widget from target layout and add to off screen layout
        _offscreenLayout->addWidget(widget);

        if (finished)
            finished();
    });
}

}
}
