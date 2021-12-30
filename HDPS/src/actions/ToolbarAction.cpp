#include "ToolbarAction.h"

#include <QCoreApplication>

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
    _offScreenCollapsedWidgetsLayout(),
    _offScreenWidgetsLayout(),
    _widgets()
{
    //_offScreenWidgetLayout.setSizeConstraint(QLayout::SetMinimumSize);

    // Set resize timer interval and only timeout once
    _resizeTimer.setInterval(RESIZE_TIMER_INTERVAL);
    _resizeTimer.setSingleShot(true);

    // Handle resize timer timeout
    connect(&_resizeTimer, &QTimer::timeout, this, &HorizontalWidget::computeLayout);

    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);

    _offScreenCollapsedWidgetsLayout.setMargin(0);
    _offScreenWidgetsLayout.setMargin(0);

    for (auto& item : _toolbarAction->_items) {
        _widgets << item.getAction()->createCollapsedWidget(this);

        _offScreenCollapsedWidgetsLayout.addWidget(_widgets.last());
        
        _widgets << item.getAction()->createWidget(this);
        
        _offScreenWidgetsLayout.addWidget(_widgets.last());

        _toolbarLayout.addWidget(new QWidget());
    }

    _offScreenCollapsedWidgetsLayout.addStretch(1);
    _offScreenWidgetsLayout.addStretch(1);

    _offScreenLayout.addLayout(&_offScreenCollapsedWidgetsLayout);
    _offScreenLayout.addLayout(&_offScreenWidgetsLayout);

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

    QVector<ItemState> itemStates;

    itemStates.resize(_toolbarAction->_items.count());

    // Initialize collapsed
    std::fill(itemStates.begin(), itemStates.end(), Collapsed);

    const auto getWidth = [this, &itemStates]() -> std::int32_t {
        std::int32_t width = (_toolbarAction->_items.count() - 1) * _toolbarLayout.spacing();

        for (std::int32_t itemIndex = 0; itemIndex < itemStates.count(); itemIndex++)
            width += getWidgetWidth(itemIndex, itemStates[itemIndex]);

        return width;
    };

    for (std::int32_t itemIndex = 0; itemIndex < _toolbarAction->_items.count(); itemIndex++) {
        auto cachedItemStates = itemStates;

        itemStates[itemIndex] = Standard;

        if (getWidth() > width()) {
            itemStates = cachedItemStates;
            break;
        }
    }

    for (std::int32_t itemIndex = 0; itemIndex < itemStates.count(); itemIndex++) {
        const auto itemState    = itemStates[itemIndex];
        const auto widget       = getWidget(itemIndex, itemState);
        const auto widgetIndex  = getWidgetIndex(itemIndex, itemState);
        const auto stateChanged = _itemStates.isEmpty() ? true : itemState != _itemStates[itemIndex];

        if (stateChanged) {
            switch (itemState)
            {
                case Collapsed:
                    _toolbarLayout.insertWidget(itemIndex, getWidget(itemIndex, Collapsed));
                    _offScreenWidgetsLayout.addWidget(getWidget(itemIndex, Standard));
                    break;

                case Standard:
                    _toolbarLayout.insertWidget(itemIndex, getWidget(itemIndex, Standard));
                    _offScreenCollapsedWidgetsLayout.addWidget(getWidget(itemIndex, Collapsed));
                    break;

                default:
                    break;
            }
        }
    }

    _itemStates = itemStates;
}

std::int32_t ToolbarAction::HorizontalWidget::getWidgetIndex(std::int32_t itemIndex, const ItemState& itemState)
{
    switch (itemState)
    {
        case Collapsed:
            return itemIndex * 2;

        case Standard:
            return itemIndex * 2 + 1;

        default:
            break;
    }

    return 0;
}

QWidget* ToolbarAction::HorizontalWidget::getWidget(std::int32_t itemIndex, const ItemState& itemState)
{
    switch (itemState)
    {
        case Collapsed:
            return _widgets[itemIndex * 2];

        case Standard:
            return _widgets[itemIndex * 2 + 1];

        default:
            break;
    }

    return nullptr;
}

std::int32_t ToolbarAction::HorizontalWidget::getWidgetWidth(std::int32_t itemIndex, const ItemState& itemState) const
{
    return const_cast<ToolbarAction::HorizontalWidget*>(this)->getWidget(itemIndex, itemState)->sizeHint().width();
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

}
}
