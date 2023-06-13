#include "VerticalToolbarAction.h"

#include <QEvent>
#include <QResizeEvent>

namespace hdps::gui {

VerticalToolbarAction::VerticalToolbarAction(QObject* parent, const QString& title, const Qt::AlignmentFlag& alignment /*= Qt::AlignmentFlag::AlignLeft*/) :
    ToolbarAction(parent, title)
{
    getGroupAction().setDefaultWidgetFlags(GroupAction::Vertical);
}

VerticalToolbarAction::Widget::Widget(QWidget* parent, VerticalToolbarAction* verticalToolbarAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, verticalToolbarAction, widgetFlags),
    _verticalToolbarAction(verticalToolbarAction),
    _layout(),
    _toolbarLayout(),
    _toolbarWidget(),
    _timer()
{
    parent->installEventFilter(this);

    _timer.setInterval(250);
    _timer.setSingleShot(true);

    _toolbarLayout.setContentsMargins(ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN);
    _toolbarLayout.setAlignment(Qt::AlignLeft);

    _toolbarWidget.setLayout(&_toolbarLayout);
    
    _layout.setSizeConstraint(QLayout::SetFixedSize);
    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.setAlignment(Qt::AlignLeft);
    _layout.addWidget(&_toolbarWidget);
    _layout.addStretch(1);
    
    setLayout(&_layout);

    connect(_verticalToolbarAction, &ToolbarAction::actionWidgetsChanged, this, &VerticalToolbarAction::Widget::setActionWidgets);
    connect(_verticalToolbarAction, &ToolbarAction::layoutInvalidated, this, &Widget::updateLayout);
    connect(&_timer, &QTimer::timeout, this, &Widget::updateLayout);

    setActionWidgets();
}

bool VerticalToolbarAction::Widget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            const auto resizeEvent = static_cast<QResizeEvent*>(event);

            auto layoutDirty = resizeEvent->size().width() != resizeEvent->oldSize().width();

            if (!layoutDirty)
                break;

            if (_timer.isActive())
                _timer.stop();

            _timer.start();

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void VerticalToolbarAction::Widget::setActionWidgets()
{
    QLayoutItem* layoutItem;

    while ((layoutItem = _toolbarLayout.takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    for (auto actionItem : _verticalToolbarAction->getActionItems())
        _toolbarLayout.addWidget(actionItem->createWidget(&_toolbarWidget));

    updateLayout();
}

void VerticalToolbarAction::Widget::updateLayout()
{
    /*
    for (auto actionItem : _horizontalToolbarAction->getActionItems())
        if (actionItem->isChangingState()) {
            qDebug() << actionItem->getAction()->text() << "is changing state";
            return;
        }
            

    //qDebug() << "Update layout" << _toolbarWidget.sizeHint().width() << parentWidget()->width() << width();
    
    QMap<ToolbarActionItem*, ToolbarActionItem::State> states;

    ToolbarAction::ActionItems actionItems;

    for (auto actionItem : _horizontalToolbarAction->getActionItems()) {
        states[actionItem] = ToolbarActionItem::State::Collapsed;

        const auto forceCollapsedInGroup    = actionItem->getAction()->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::ForceCollapsedInGroup);
        const auto forceExpandedInGroup     = actionItem->getAction()->isConfigurationFlagSet(WidgetAction::ConfigurationFlag::ForceExpandedInGroup);

        if (forceCollapsedInGroup || forceExpandedInGroup) {
            if (forceExpandedInGroup)
                states[actionItem] = ToolbarActionItem::State::Expanded;
        }
        else {
            actionItems << actionItem;
        }
    }

    const auto getWidth = [this, &states]() -> std::uint32_t {
        std::uint32_t width = 2 * ToolbarAction::CONTENTS_MARGIN;

        for (auto actionItem : _horizontalToolbarAction->getActionItems())
            width += actionItem->getWidgetSize(states[actionItem]).width();

        width += (std::max(1, static_cast<int>(_horizontalToolbarAction->getActionItems().count() - 1)) * _toolbarLayout.spacing());

        return width;
    };

    std::sort(actionItems.begin(), actionItems.end());
    std::reverse(actionItems.begin(), actionItems.end());

    for (auto actionItem : actionItems) {
        auto cachedStates = states;

        states[actionItem] = ToolbarActionItem::State::Expanded;

        if (getWidth() > static_cast<std::uint32_t>(parentWidget()->width())) {
            states = cachedStates;
            break;
        }
    }

    for (auto actionItem : actionItems)
        actionItem->setState(states[actionItem]);
    */
}

}
