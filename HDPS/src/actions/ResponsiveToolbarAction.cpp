#include "ResponsiveToolbarAction.h"

#include <QCoreApplication>
#include <QResizeEvent>

#include <cmath>

namespace hdps {

namespace gui {

ResponsiveToolbarAction::ResponsiveToolbarAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _items()
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Horizontal);
}

void ResponsiveToolbarAction::addAction(WidgetAction* action, std::int32_t priority /*= 1*/)
{
    _items << Item(action, priority);
}

ResponsiveToolbarAction::HorizontalWidget::HorizontalWidget(QWidget* parent, ResponsiveToolbarAction* toolbarAction) :
    QWidget(parent),
    _responsiveToolbarAction(toolbarAction),
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
    connect(&_resizeTimer, &QTimer::timeout, this, [this]() {
        computeLayout();
    });

    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSpacing(0);
    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);
    
    // Create stateful item for each toolbar action
    for (auto& item : _responsiveToolbarAction->_items) {
        _statefulItems << SharedStatefulItem::create(*this, _responsiveToolbarAction->_items.indexOf(item), item);

        // Listen for resize events in the stateful item so that we can re-compute the layout
        _statefulItems.last()->installEventFilter(this);

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

void ResponsiveToolbarAction::HorizontalWidget::resizeEvent(QResizeEvent* resizeEvent)
{
    // Prevent resize timer timeout
    if (_resizeTimer.isActive())
        _resizeTimer.stop();

    // Start resize timer
    _resizeTimer.start();
}

void ResponsiveToolbarAction::HorizontalWidget::computeLayout(StatefulItem* statefulItem /*= nullptr*/)
{
    qDebug() << "Compute toolbar widget layout" << width();

    QVector<ItemState> itemStates;

    itemStates.resize(_responsiveToolbarAction->_items.count());

    // Initialize collapsed
    std::fill(itemStates.begin(), itemStates.end(), ItemState::Collapsed);

    if (statefulItem != nullptr)
        itemStates[statefulItem->getIndex()] = ItemState::Standard;

    // Compute candidate configuration width
    const auto getWidth = [this, &itemStates]() -> std::int32_t {
        std::int32_t width = 0;

        // Compute width of spacer widgets
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

void ResponsiveToolbarAction::HorizontalWidget::setItemStates(const QVector<ItemState>& itemStates)
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

QWidget* ResponsiveToolbarAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Maximum);

    layout->setMargin(0);

    if (widgetFlags & WidgetFlag::Horizontal)
        layout->addWidget(new ResponsiveToolbarAction::HorizontalWidget(parent, this));

    widget->setLayout(layout);

    return widget;
}

ResponsiveToolbarAction::Item::Item(WidgetAction* action, std::int32_t priority) :
    _action(action),
    _priority(priority)
{
}

WidgetAction* ResponsiveToolbarAction::Item::getAction()
{
    return _action;
}

std::int32_t ResponsiveToolbarAction::Item::getPriority() const
{
    return _priority;
}

ResponsiveToolbarAction::HorizontalWidget::StatefulItem::StatefulItem(HorizontalWidget& horizontalWidget, std::int32_t index, Item& item) :
    QObject(&horizontalWidget),
    _horizontalWidget(horizontalWidget),
    _index(index),
    _item(item),
    _state(ItemState::Undefined),
    _widget(),
    _widgetLayout(),
    _collapsedWidget(&_widget, _item.getAction()->createCollapsedWidget(&_widget)),
    _standardWidget(&_widget, _item.getAction()->createWidget(&_widget)),
    _sizeAnimation(&_widget)
{
    _collapsedWidget.setOpacity(0.0f);
    _standardWidget.setOpacity(0.0f);

    // Respond to changes in the size of the standard widget
    _standardWidget.installEventFilter(this);

    // Configure size animation
    _sizeAnimation.setDuration(ANIMATION_DURATION);
    _sizeAnimation.setStartValue(0.0f);
    _sizeAnimation.setEndValue(1.0f);
    _sizeAnimation.setEasingCurve(QEasingCurve::InOutQuad);

    _widgetLayout.setMargin(0);

    _widget.setLayout(&_widgetLayout);
}

ResponsiveToolbarAction::Item& ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getItem()
{
    return _item;
}

std::int32_t ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getIndex() const
{
    return _index;
}

ResponsiveToolbarAction::ItemState ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getState() const
{
    return _state;
}

void ResponsiveToolbarAction::HorizontalWidget::StatefulItem::setState(const ItemState& state)
{
    const auto widthBegin           = static_cast<float>(getWidth(_state == ItemState::Undefined ? state : _state));
    const auto widthEnd             = static_cast<float>(getWidth(state));
    const auto stateChanged         = state != _state;
    const auto widthChanged         = widthBegin != widthEnd;
    const auto animationDuration    = (_state == ItemState::Undefined) ? 0 : (widthChanged ? ANIMATION_DURATION : 0);

    // Width interpolation
    const auto widthLerp = [widthBegin, widthEnd](float norm) {
        return widthBegin + norm * (widthEnd - widthBegin);
    };

    // Configure and possibly animate widgets based on the state
    switch (state)
    {
        case ItemState::Collapsed:
        {
            // Collapsed widget should respond to mouse events and the standard widget should not
            _collapsedWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);
            _standardWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);

            // The collapsed widget should be raised above the standard widget
            _collapsedWidget.raise();

            if (stateChanged) {

                // Fade out the standard widget and swap the widgets when done
                _standardWidget.fadeOut(animationDuration, 0, [this]() {
                    _widgetLayout.takeAt(0);
                    _widgetLayout.addWidget(&_collapsedWidget);
                });

                // Fade in the collapsed widget when the state changed
                _collapsedWidget.fadeIn(animationDuration, animationDuration);
            }

            break;
        }

        case ItemState::Standard:
        {
            // Collapsed widget should not respond to mouse events and the standard widget should
            _collapsedWidget.setAttribute(Qt::WA_TransparentForMouseEvents, true);
            _standardWidget.setAttribute(Qt::WA_TransparentForMouseEvents, false);
            
            // The standard widget should be raised above the collapsed widget
            _standardWidget.raise();

            if (stateChanged) {

                // Fade in the standard widget when the state changed
                _standardWidget.fadeIn(animationDuration, animationDuration);

                // Fade out the collapsed widget and swap the widgets when done
                _collapsedWidget.fadeOut(animationDuration, 0, [this]() {
                    _widgetLayout.takeAt(0);
                    _widgetLayout.addWidget(&_standardWidget);
                });
            }

            break;
        }
    }

    // Update the widget fixed width when the size animation values changes
    connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this, widthLerp](const QVariant& value) {
        _widget.setFixedWidth(widthLerp(value.toFloat()));
    });

    // Set size animation duration and start
    _sizeAnimation.setDuration(animationDuration);
    _sizeAnimation.start();

    // Assign new state
    _state = state;
}

QWidget* ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getWidget(const ItemState& state)
{
    switch (state)
    {
        case ItemState::Collapsed:
            return &_collapsedWidget;

        case ItemState::Standard:
            return &_standardWidget;
    }

    return nullptr;
}

QWidget* ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getWidget()
{
    return &_widget;
}

std::int32_t ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getWidth() const
{
    return const_cast<StatefulItem*>(this)->getWidget(_state)->sizeHint().width();
}

std::int32_t ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getWidth(const ItemState& state) const
{
    return const_cast<StatefulItem*>(this)->getWidget(state)->sizeHint().width();
}

std::int32_t ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getPriority() const
{
    return _item.getPriority();
}

bool ResponsiveToolbarAction::HorizontalWidget::StatefulItem::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (_state != ItemState::Standard)
                break;

            const auto resizeEvent = static_cast<QResizeEvent*>(event);
            QCoreApplication::processEvents();
            _widget.setFixedWidth(resizeEvent->size().width());
            _horizontalWidget.computeLayout(this);
            break;
        }
    }

    return QObject::eventFilter(target, event);
}

ResponsiveToolbarAction::HorizontalWidget::SpacerWidget::SpacerWidget(const Type& type /*= Type::Divider*/) :
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
    _layout.setSpacing(0);
    _layout.setAlignment(Qt::AlignCenter);
    _layout.addWidget(&_verticalLine);

    setLayout(&_layout);

    _fadeableWidget.setOpacity(0.0f);

    _sizeAnimation.setEasingCurve(QEasingCurve::InOutQuad);

    //setStyleSheet("background-color: purple;");
}

ResponsiveToolbarAction::HorizontalWidget::SpacerWidget::Type ResponsiveToolbarAction::HorizontalWidget::SpacerWidget::getType(const ItemState& itemStateLeft, const ItemState& itemStateRight)
{
    return itemStateLeft == ItemState::Collapsed && itemStateRight == ItemState::Collapsed ? Type::Spacer : Type::Divider;
}

ResponsiveToolbarAction::HorizontalWidget::SpacerWidget::Type ResponsiveToolbarAction::HorizontalWidget::SpacerWidget::getType(const StatefulItem* statefulItemLeft, const StatefulItem* statefulItemRight)
{
    return getType(statefulItemLeft->getState(), statefulItemRight->getState());
}

void ResponsiveToolbarAction::HorizontalWidget::SpacerWidget::setType(const Type& type)
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

std::int32_t ResponsiveToolbarAction::HorizontalWidget::SpacerWidget::getWidth(const Type& type)
{
    switch (type)
    {
        case Type::Undefined:   return 0;
        case Type::Divider:     return 20;
        case Type::Spacer:      return 4;
    }

    return 0;
}

}
}
