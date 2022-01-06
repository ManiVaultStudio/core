#include "VerticalToolbarWidget.h"

#include <QResizeEvent>

#include <cmath>

namespace hdps {

namespace gui {

VerticalToolbarWidget::VerticalToolbarWidget(QWidget* parent, ToolbarAction& toolbarAction) :
    ToolbarWidget(parent, toolbarAction, Qt::Vertical),
    _layout(),
    _toolbarLayout(),
    _toolbarWidget()
{
    /*
    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSpacing(0);
    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);

    // Create stateful items and spacers for each toolbar action
    for (auto& item : _responsiveToolbarAction._items) {
        _statefulItems << SharedStatefulItem::create(this, _statefulItems.count(), *item.getAction(), item.getPriority());

        _toolbarLayout.addWidget(_statefulItems.last()->getWidget());
    }

    // Add item for hidden items
    _statefulItems << SharedStatefulItem::create(this, _statefulItems.count(), _responsiveToolbarAction.getHiddenItemsAction(), -1);

    for (auto statefulItem : _statefulItems) {
        statefulItem->setPrevious(statefulItem == _statefulItems.first() ? nullptr : _statefulItems[_statefulItems.indexOf(statefulItem) - 1].get());
        statefulItem->setNext(statefulItem == _statefulItems.last() ? nullptr : _statefulItems[_statefulItems.indexOf(statefulItem) + 1].get());
    }

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
    */
}

void VerticalToolbarWidget::computeLayout(ToolbarItemWidget* ignoreToolbarItemWidget /*= nullptr*/)
{
    /*
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

    // Hide some items if all items are collapsed and there is not enough room
    if (std::adjacent_find(itemStates.begin(), itemStates.end(), std::not_equal_to<>()) == itemStates.end() && itemStates.first() == ItemState::Collapsed)
    {
    }

    // Assign new item states
    setItemStates(itemStates);
    */
}

}
}
