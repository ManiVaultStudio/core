#include "ResponsiveToolbarAction.h"
#include "Application.h"

#include <QCoreApplication>
#include <QResizeEvent>

#include <cmath>

namespace hdps {

namespace gui {

ResponsiveToolbarAction::ResponsiveToolbarAction(QObject* parent, const QString& title /*= ""*/) :
    WidgetAction(parent),
    _items(),
    _enableAnimation(true),
    _hiddenItemsAction(*this)
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Horizontal);
}

void ResponsiveToolbarAction::addAction(WidgetAction* action, std::int32_t priority /*= 1*/)
{
    _items << Item(action, priority);
}

bool ResponsiveToolbarAction::getEnableAnimation() const
{
    return _enableAnimation;
}

void ResponsiveToolbarAction::setEnableAnimation(bool enableAnimation)
{
    _enableAnimation = enableAnimation;
}

ResponsiveToolbarAction::HiddenItemsAction& ResponsiveToolbarAction::getHiddenItemsAction()
{
    return _hiddenItemsAction;
}

ResponsiveToolbarAction::HorizontalWidget::HorizontalWidget(QWidget* parent, ResponsiveToolbarAction& responsiveToolbarAction) :
    ToolbarWidget(parent, responsiveToolbarAction),
    _resizeTimer(),
    _mainLayout(),
    _toolbarLayout(),
    _toolbarWidget()
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

    // Create stateful items and spacers for each toolbar action
    for (auto& item : _responsiveToolbarAction._items) {
        _statefulItems << SharedStatefulItem::create(this, _statefulItems.count(), *item.getAction(), item.getPriority());

        if (_statefulItems.count() >= 2) {
            _spacerWidgets << SharedSpacerWidget::create(this);

            _toolbarLayout.addWidget(_spacerWidgets.last().get());
        }

        _toolbarLayout.addWidget(_statefulItems.last()->getWidget());
    }

    // Add last spacer widget
    _spacerWidgets << SharedSpacerWidget::create(this);

    // And add it to the toolbar layout
    _toolbarLayout.addWidget(_spacerWidgets.last().get());

    // Add item for hidden items
    _statefulItems << SharedStatefulItem::create(this, _statefulItems.count(), _responsiveToolbarAction.getHiddenItemsAction(), -1);

    // And add it to the toolbar layout
    _toolbarLayout.addWidget(_statefulItems.last()->getWidget());

    // Re-compute the layout when a stateful item resizes
    for (auto statefulItem : _statefulItems)
        statefulItem->installEventFilter(this);

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

    itemStates.resize(_statefulItems.count());

    // Initialize collapsed
    std::fill(itemStates.begin(), itemStates.end(), ItemState::Collapsed);

    if (statefulItem != nullptr)
        itemStates[statefulItem->getIndex()] = ItemState::Standard;

    itemStates.last() = ItemState::Hidden;

    // Compute candidate configuration width
    const auto getWidth = [this, &itemStates](std::int32_t numberOfItems) -> std::int32_t {
        std::int32_t width = 0;

        // Compute width of items and spacers
        for (auto statefulItem : _statefulItems) {

            // Get index of the stateful item
            const auto statefulItemIndex = _statefulItems.indexOf(statefulItem);

            if (statefulItemIndex >= numberOfItems)
                break;

            // Add width of the stateful item
            width += statefulItem->getWidth(itemStates[statefulItem->getIndex()]);

            if (statefulItem != _statefulItems.first()) {
                const auto spacerWidgetIndex    = _statefulItems.indexOf(statefulItem);
                const auto itemStateLeft        = itemStates[spacerWidgetIndex];
                const auto itemStateRight       = itemStates[spacerWidgetIndex - 1];
                const auto spacerWidgetType     = SpacerWidget::getType(itemStateLeft, itemStateRight);
                const auto spacerWidgetWidth    = SpacerWidget::getWidth(spacerWidgetType);

                // Add width of the spacer item
                width += spacerWidgetWidth;
            }
        }

        return width;
    };

    auto sortedStatefulItems = _statefulItems;

    // Sort stateful items in descending order
    std::sort(sortedStatefulItems.begin(), sortedStatefulItems.end(), [](auto a, auto b) {
        return a->getPriority() > b->getPriority();
    });

    // Compute stateful item states
    for (auto sortedStatefulItem : sortedStatefulItems) {
        auto cachedItemStates = itemStates;

        if (sortedStatefulItem->getPriority() >= 0)
            itemStates[sortedStatefulItem->getIndex()] = ItemState::Standard;

        if (getWidth(itemStates.count()) > width()) {
            itemStates = cachedItemStates;
            break;
        }
    }

    if (std::adjacent_find(itemStates.begin(), itemStates.end(), std::not_equal_to<>()) == itemStates.end() && itemStates.first() == ItemState::Collapsed)
    {
        for (auto statefulItem : _statefulItems) {
            if (statefulItem == _statefulItems.last())
                break;

            auto cachedItemStates = itemStates;

            if (getWidth(_statefulItems.indexOf(statefulItem) + 2) > width())
                itemStates[statefulItem->getIndex()] = ItemState::Hidden;
        }

        qDebug() << "All items are collapsed";
        /*
        for (auto statefulItem : _statefulItems) {
            statefulItem->setVisibility(getWidth(_statefulItems.indexOf(statefulItem)) > width());
        }

        
        // Hide items
        if (getWidth(itemStates.count()) > width()) {

            // Find the index of the first item that is (partially) hidden
            //const auto itemIndex = static_cast<float>(getWidth()) / 
            for (auto statefulItem : _statefulItems) {

                // Get index of the stateful item
                const auto statefulItemIndex = _statefulItems.indexOf(statefulItem);

                if (getWidth(statefulItemIndex + 1) > width()) {
                    qDebug() << statefulItemIndex - 1 << "is the first non-visible item";

                    statefulItem->hide();
                }
                else {
                    statefulItem->show();
                }
            }
        }
        */
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

    layout->setMargin(0);

    if (widgetFlags & WidgetFlag::Horizontal) {
        widget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::MinimumExpanding);
        layout->addWidget(new ResponsiveToolbarAction::HorizontalWidget(parent, *this));
    }

    if (widgetFlags & WidgetFlag::Vertical) {
        widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);

        layout->setAlignment(Qt::AlignTop);
        layout->addWidget(new ResponsiveToolbarAction::VerticalWidget(parent, *this));
    }

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

ResponsiveToolbarAction::HorizontalWidget::StatefulItem::StatefulItem(ToolbarWidget* toolbarWidget, std::int32_t index, WidgetAction& action, std::int32_t priority) :
    QObject(toolbarWidget),
    _toolbarWidget(toolbarWidget),
    _index(index),
    _action(action),
    _priority(priority),
    _state(ItemState::Undefined),
    _widget(),
    _widgetLayout(),
    _collapsedWidget(&_widget, _action.createCollapsedWidget(&_widget)),
    _standardWidget(&_widget, _action.createWidget(&_widget)),
    _sizeAnimation(&_widget)
{
    _collapsedWidget.setOpacity(1.0f);
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
    const auto animationDuration    = (_state == ItemState::Undefined || !_toolbarWidget->getEnableAnimation()) ? 0 : (widthChanged ? ANIMATION_DURATION : 0);

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

        case ItemState::Hidden:
        {
            if (stateChanged) {

                // Fade out the collapsed widget and hide when done
                _collapsedWidget.fadeOut(animationDuration, 0, [this]() {
                    _widgetLayout.takeAt(0);
                    _widgetLayout.addWidget(&_standardWidget);
                });
            }

            break;
        }
    }

    if (_toolbarWidget->getEnableAnimation()) {

        // Update the widget fixed width when the size animation values changes
        connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this, widthLerp](const QVariant& value) {
            _widget.setFixedWidth(widthLerp(value.toFloat()));
        });

        // Set size animation duration and start
        _sizeAnimation.setDuration(animationDuration);
        _sizeAnimation.start();
    }
    else {
        _widget.setFixedWidth(widthEnd);
    }

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
    if (_state == ItemState::Hidden)
        return 0;

    return const_cast<StatefulItem*>(this)->getWidget(_state)->sizeHint().width();
}

std::int32_t ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getWidth(const ItemState& state) const
{
    if (state == ItemState::Hidden)
        return 0;

    return const_cast<StatefulItem*>(this)->getWidget(state)->sizeHint().width();
}

std::int32_t ResponsiveToolbarAction::HorizontalWidget::StatefulItem::getPriority() const
{
    return _priority;
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
            _toolbarWidget->computeLayout(this);
            break;
        }
    }

    return QObject::eventFilter(target, event);
}

ResponsiveToolbarAction::HorizontalWidget::SpacerWidget::SpacerWidget(ToolbarWidget* toolbarWidget, const Type& type /*= Type::Divider*/) :
    QWidget(),
    _toolbarWidget(toolbarWidget),
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
    if (itemStateLeft == ItemState::Collapsed && itemStateRight == ItemState::Hidden)
        return Type::Hidden;

    if (itemStateLeft == ItemState::Hidden && itemStateRight == ItemState::Collapsed)
        return Type::Hidden;

    if (itemStateLeft == ItemState::Hidden && itemStateRight == ItemState::Hidden)
        return Type::Hidden;

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
            _fadeableWidget.fadeIn(_toolbarWidget->getEnableAnimation() ? ANIMATION_DURATION : 0, 0);

            _sizeAnimation.setStartValue(getWidth(Type::Spacer));
            _sizeAnimation.setEndValue(getWidth(Type::Divider));

            break;
        }

        case Type::Spacer:
        {
            if (_type != Type::Undefined)
                _fadeableWidget.fadeOut(_toolbarWidget->getEnableAnimation() ? ANIMATION_DURATION / 2 : 0, 0);

            _sizeAnimation.setStartValue(getWidth(Type::Divider));
            _sizeAnimation.setEndValue(getWidth(Type::Spacer));

            break;
        }

        case Type::Hidden:
        {
            if (_type != Type::Undefined)
                _fadeableWidget.fadeOut(_toolbarWidget->getEnableAnimation() ? ANIMATION_DURATION / 2 : 0, 0);

            _sizeAnimation.setStartValue(getWidth(_type));
            _sizeAnimation.setEndValue(getWidth(type));

            break;
        }

        default:
            break;
    }

    connect(&_sizeAnimation, &QVariantAnimation::valueChanged, [this](const QVariant& value) {
        setFixedWidth(value.toFloat());
    });

    _sizeAnimation.setDuration((_type == Type::Undefined || !_toolbarWidget->getEnableAnimation() )? 0 : ANIMATION_DURATION);
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
        case Type::Hidden:      return 0;
    }

    return 0;
}

ResponsiveToolbarAction::VerticalWidget::VerticalWidget(QWidget* parent, ResponsiveToolbarAction& responsiveToolbarAction) :
    ToolbarWidget(parent, responsiveToolbarAction),
    _layout()
{
    _layout.setMargin(0);
    _layout.setSpacing(4);
    _layout.setSizeConstraint(QLayout::SetFixedSize);

    for (auto& item : _responsiveToolbarAction._items)
        _layout.addWidget(item.getAction()->createCollapsedWidget(this));

    if (!_responsiveToolbarAction._items.isEmpty())
        _layout.addStretch(1);

    setLayout(&_layout);
}

ResponsiveToolbarAction::HiddenItemsAction::HiddenItemsAction(ResponsiveToolbarAction& responsiveToolbarAction) :
    WidgetAction(&responsiveToolbarAction),
    _responsiveToolbarAction(responsiveToolbarAction)
{
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("ellipsis-h"));
}

ResponsiveToolbarAction& ResponsiveToolbarAction::HiddenItemsAction::getResponsiveToolbarAction()
{
    return _responsiveToolbarAction;
}

ResponsiveToolbarAction::HiddenItemsAction::Widget::Widget(QWidget* parent, HiddenItemsAction* hiddenItemsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, hiddenItemsAction, widgetFlags),
    _hiddenItemsAction(hiddenItemsAction),
    _layout()
{
    if (widgetFlags & WidgetActionWidget::PopupLayout) {
        setPopupLayout(&_layout);
    }
    else {
        _layout.setMargin(0);
        setLayout(&_layout);
    }
}

void ResponsiveToolbarAction::ToolbarWidget::StatefulItem::show()
{
    _collapsedWidget.fadeIn(ANIMATION_DURATION);
    _standardWidget.fadeIn(ANIMATION_DURATION);
}

void ResponsiveToolbarAction::ToolbarWidget::StatefulItem::hide()
{
    _collapsedWidget.fadeOut(ANIMATION_DURATION);
    _standardWidget.fadeOut(ANIMATION_DURATION);
}

void ResponsiveToolbarAction::ToolbarWidget::StatefulItem::setVisibility(bool visible)
{
    if (visible)
        show();
    else
        hide();
}

}
}
