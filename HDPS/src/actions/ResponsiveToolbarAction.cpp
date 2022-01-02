#include "ResponsiveToolbarAction.h"

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
    _statefulItems(),
    _spacerWidgets()
{
    // Set resize timer interval and only timeout once
    _resizeTimer.setInterval(RESIZE_TIMER_INTERVAL);
    _resizeTimer.setSingleShot(true);

    // Handle resize timer timeout
    connect(&_resizeTimer, &QTimer::timeout, this, &HorizontalWidget::computeLayout);

    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSpacing(0);
    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);

    // Create stateful item for each toolbar action
    for (auto& item : _toolbarAction->_items) {
        _statefulItems << SharedStatefulItem::create(this, _toolbarAction->_items.indexOf(item), item);

        if (_statefulItems.count() >= 2) {
            _spacerWidgets << SharedSpacerWidget::create();

            _toolbarLayout.addWidget(_spacerWidgets.last().get());
        }

        _toolbarLayout.addWidget(_statefulItems.last()->getWidget());
    }

    _toolbarWidget.setLayout(&_toolbarLayout);

    // Add toolbar widget to main layout
    _mainLayout.addWidget(&_toolbarWidget);
    _mainLayout.addStretch(1);

    // Apply main layout
    setLayout(&_mainLayout);

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

    QVector<ItemState> itemStates;

    itemStates.resize(_toolbarAction->_items.count());

    // Initialize collapsed
    std::fill(itemStates.begin(), itemStates.end(), ItemState::Collapsed);

    // Compute candidate configuration width
    const auto getWidth = [this, &itemStates]() -> std::int32_t {
        std::int32_t width = (_toolbarAction->_items.count() - 1) * _toolbarLayout.spacing();

        // Compute total width of spacer widgets
        for (auto spacerWidget : _spacerWidgets) {
            const auto spacerWidgetIndex    = _spacerWidgets.indexOf(spacerWidget);
            const auto itemStateLeft        = itemStates[spacerWidgetIndex];
            const auto itemStateRight       = itemStates[spacerWidgetIndex + 1];
            const auto spacerWidgetType     = SpacerWidget::getType(itemStateLeft, itemStateRight);
            const auto spacerWidgetWidth    = SpacerWidget::getWidth(spacerWidgetType);

            width += spacerWidgetWidth;
        }

        // Compute width of spacer items
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

        itemStates[sortedStatefulItem->getIndex()] = ItemState::Standard;

        if (getWidth() > width()) {
            itemStates = cachedItemStates;
            break;
        }
    }

    // Assign new item states
    setItemStates(itemStates);
}

void ToolbarAction::HorizontalWidget::setItemStates(const QVector<ItemState>& itemStates)
{
    // Assign new state(s) to stateful item(s)
    for (auto statefulItem : _statefulItems)
        statefulItem->setState(itemStates[statefulItem->getIndex()]);

    // Assign type to spacer widget(s)
    for (auto spacerWidget : _spacerWidgets) {
        const auto spacerWidgetIndex    = _spacerWidgets.indexOf(spacerWidget);
        const auto itemStateLeft        = itemStates[spacerWidgetIndex];
        const auto itemStateRight       = itemStates[spacerWidgetIndex + 1];
        const auto spacerWidgetType     = SpacerWidget::getType(itemStateLeft, itemStateRight);

        spacerWidget->setType(spacerWidgetType);
    }
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
    _state(ItemState::Undefined),
    _widget(),
    _collapsedWidget(&_widget, _item.getAction()->createCollapsedWidget(&_widget)),
    _standardWidget(&_widget, _item.getAction()->createWidget(&_widget)),
    _sizeAnimation(&_widget)
{
    _collapsedWidget.setOpacity(0.0f);
    _standardWidget.setOpacity(0.0f);

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

ToolbarAction::ItemState ToolbarAction::HorizontalWidget::StatefulItem::getState() const
{
    return _state;
}

void ToolbarAction::HorizontalWidget::StatefulItem::setState(const ItemState& state)
{
    if (state == _state)
        return;

    const auto widthBegin   = static_cast<float>(getWidth(_state == ItemState::Undefined ? state : _state));
    const auto widthEnd     = static_cast<float>(getWidth(state));

    const auto widthLerp = [widthBegin, widthEnd](float norm) {
        return widthBegin + norm * (widthEnd - widthBegin);
    };

    switch (state)
    {
        case ItemState::Collapsed:
        {
            _standardWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);

            if (_state != ItemState::Undefined)
                _standardWidget.fadeOut(ANIMATION_DURATION);

            _collapsedWidget.raise();
            _collapsedWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);
            _collapsedWidget.fadeIn(ANIMATION_DURATION, _state == ItemState::Undefined ? 0 : ANIMATION_DURATION);

            break;
        }

        case ItemState::Standard:
        {
            _collapsedWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);

            if (_state != ItemState::Undefined)
                _collapsedWidget.fadeOut(ANIMATION_DURATION);

            _standardWidget.raise();
            _standardWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);
            _standardWidget.fadeIn(ANIMATION_DURATION, _state == ItemState::Undefined ? 0 : ANIMATION_DURATION);

            break;
        }
    }

    connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this, widthLerp](const QVariant& value) {
        _widget.setFixedWidth(widthLerp(value.toFloat()));
    });

    _sizeAnimation.setDuration(_state == ItemState::Undefined ? 0 : ANIMATION_DURATION);
    _sizeAnimation.start();

    _state = state;
}

QWidget* ToolbarAction::HorizontalWidget::StatefulItem::getWidget(const ItemState& itemState)
{
    switch (itemState)
    {
        case ItemState::Collapsed:
            return &_collapsedWidget;

        case ItemState::Standard:
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

ToolbarAction::HorizontalWidget::SpacerWidget::SpacerWidget(const Type& type /*= Type::Divider*/) :
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

ToolbarAction::HorizontalWidget::SpacerWidget::Type ToolbarAction::HorizontalWidget::SpacerWidget::getType(const ItemState& itemStateLeft, const ItemState& itemStateRight)
{
    return itemStateLeft == ItemState::Collapsed && itemStateRight == ItemState::Collapsed ? Type::Spacer : Type::Divider;
}

ToolbarAction::HorizontalWidget::SpacerWidget::Type ToolbarAction::HorizontalWidget::SpacerWidget::getType(const StatefulItem* statefulItemLeft, const StatefulItem* statefulItemRight)
{
    return getType(statefulItemLeft->getState(), statefulItemRight->getState());
}

void ToolbarAction::HorizontalWidget::SpacerWidget::setType(const Type& type)
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

std::int32_t ToolbarAction::HorizontalWidget::SpacerWidget::getWidth(const Type& type)
{
    switch (type)
    {
    case Type::Undefined:       return 0;
    case Type::Divider:         return 20;
    case Type::Spacer:          return 4;
    }

    return 0;
}

}
}
