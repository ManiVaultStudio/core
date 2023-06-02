#include "HorizontalToolbarAction.h"

#include <QEvent>
#include <QResizeEvent>

namespace hdps::gui {

HorizontalToolbarAction::HorizontalToolbarAction(QObject* parent, const QString& title /*= "Horizontal Toolbar"*/) :
    ToolbarAction(parent, title)
{
    setText(title);
    
    getGroupAction().setDefaultWidgetFlags(GroupAction::Horizontal);
}

HorizontalToolbarAction::Widget::Widget(QWidget* parent, HorizontalToolbarAction* horizontalToolbarAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, horizontalToolbarAction, widgetFlags),
    _horizontalToolbarAction(horizontalToolbarAction),
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

    connect(_horizontalToolbarAction, &ToolbarAction::actionWidgetsChanged, this, &HorizontalToolbarAction::Widget::setActionWidgets);
    connect(&_timer, &QTimer::timeout, this, &Widget::updateLayout);

    setActionWidgets();
}

bool HorizontalToolbarAction::Widget::eventFilter(QObject* target, QEvent* event)
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

void HorizontalToolbarAction::Widget::setActionWidgets()
{
    QLayoutItem* layoutItem;

    while ((layoutItem = _toolbarLayout.takeAt(0)) != nullptr) {
        delete layoutItem->widget();
        delete layoutItem;
    }

    for (auto actionItem : _horizontalToolbarAction->getActionItems())
        _toolbarLayout.addWidget(actionItem->createWidget(&_toolbarWidget));

    updateLayout();
}

void HorizontalToolbarAction::Widget::updateLayout()
{
    //qDebug() << "Update layout" << _toolbarWidget.sizeHint().width() << parentWidget()->width() << width();
    
    QMap<ToolbarActionItem*, ToolbarActionItem::State> states;

    for (auto actionItem : _horizontalToolbarAction->getActionItems())
        states[actionItem] = ToolbarActionItem::State::Collapsed;

    const auto getWidth = [this, &states]() -> std::uint32_t {
        std::uint32_t width = 2 * ToolbarAction::CONTENTS_MARGIN;

        for (auto actionItem : _horizontalToolbarAction->getActionItems())
            width += actionItem->getWidgetSize(states[actionItem]).width();

        width += (std::max(1, static_cast<int>(_horizontalToolbarAction->getActionItems().count() - 1)) * _toolbarLayout.spacing());

        return width;
    };

    auto autoExpandPrioritySortedActionItems = _horizontalToolbarAction->getActionItems().values();

    std::sort(autoExpandPrioritySortedActionItems.begin(), autoExpandPrioritySortedActionItems.end());

    for (auto actionItem : autoExpandPrioritySortedActionItems) {
        auto cachedStates = states;

        states[actionItem] = ToolbarActionItem::State::Expanded;

        if (getWidth() > static_cast<std::uint32_t>(parentWidget()->width())) {
            states = cachedStates;
            break;
        }
    }

    for (auto actionItem : _horizontalToolbarAction->getActionItems())
        actionItem->setState(states[actionItem]);
}

}
