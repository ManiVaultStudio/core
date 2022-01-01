#include "ToolbarAction.h"

#include <QCoreApplication>

#include <cmath>

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
    _statefulItems()
{
    // Set resize timer interval and only timeout once
    _resizeTimer.setInterval(RESIZE_TIMER_INTERVAL);
    _resizeTimer.setSingleShot(true);

    // Handle resize timer timeout
    connect(&_resizeTimer, &QTimer::timeout, this, &HorizontalWidget::computeLayout);

    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);

    // Create stateful item for each toolbar action
    for (auto& item : _toolbarAction->_items)
        _statefulItems << SharedStatefulItem::create(this, _toolbarAction->_items.indexOf(item), item);

    for (auto statefulItem : _statefulItems)
        _toolbarLayout.addWidget(statefulItem->getWidget());

    _toolbarWidget.setLayout(&_toolbarLayout);

    // Add toolbar widget to main layout
    _mainLayout.addWidget(&_toolbarWidget);
    _mainLayout.addStretch(1);

    // Apply main layout
    setLayout(&_mainLayout);

    // Configure main layout
    _mainLayout.setMargin(0);
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

ToolbarAction::HorizontalWidget::StatefulItem::StatefulItem(QWidget* parent, std::int32_t index, Item& item) :
    _index(index),
    _item(item),
    _state(None),
    _widget(),
    _collapsedWidget(&_widget, _item.getAction()->createCollapsedWidget(&_widget)),
    _standardWidget(&_widget, _item.getAction()->createWidget(&_widget)),
    _sizeAnimation(&_widget)
{
    _sizeAnimation.setDuration(ANIMATION_DURATION);
    _sizeAnimation.setStartValue(0.0f);
    _sizeAnimation.setEndValue(1.0f);
    _sizeAnimation.setEasingCurve(QEasingCurve::InOutQuad);

    _widget.setLayout(&_widgetLayout);
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
            swapWidget(Standard, Collapsed);

            break;
        }

        case Standard:
        {
            swapWidget(Collapsed, Standard);

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
            return &_collapsedWidget;

        case Standard:
            return &_standardWidget;
    }

    return nullptr;
}

QWidget* ToolbarAction::HorizontalWidget::StatefulItem::getWidget()
{
    return &_widget;
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

void ToolbarAction::HorizontalWidget::StatefulItem::swapWidget(const ItemState& stateA, const ItemState& stateB)
{
    const auto widthBegin   = static_cast<float>(getWidth(stateA));
    const auto widthEnd     = static_cast<float>(getWidth(stateB));

    const auto widthLerp = [widthBegin, widthEnd](float norm) {
        return widthBegin + norm * (widthEnd - widthBegin);
    };

    switch (stateB)
    {
        case Collapsed:
        {
            _standardWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);
            _standardWidget.fadeOut();

            _collapsedWidget.raise();
            _collapsedWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);
            _collapsedWidget.fadeIn(ANIMATION_DURATION);

            break;
        }

        case Standard:
        {
            _collapsedWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);
            _collapsedWidget.fadeOut();

            _standardWidget.raise();
            _standardWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);
            _standardWidget.fadeIn(ANIMATION_DURATION);

            break;
        }
    }

    connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this, widthLerp, stateB](const QVariant& value) {
        _widget.setFixedWidth(widthLerp(value.toFloat()));
    });

    _sizeAnimation.start();
}

}
}
