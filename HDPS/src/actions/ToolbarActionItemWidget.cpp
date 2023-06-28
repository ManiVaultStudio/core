#include "ToolbarActionItemWidget.h"
#include "ToolbarActionItem.h"
#include "WidgetAction.h"

#include <QHBoxLayout>
#include <QResizeEvent>
#include <QCoreApplication>
#include <QLabel>

namespace hdps::gui {

ToolbarActionItemWidget::ToolbarActionItemWidget(QWidget* parent, ToolbarActionItem& toolbarActionItem) :
    QWidget(parent),
    _toolbarActionItem(toolbarActionItem),
    _collapsedWidget(this, const_cast<WidgetAction*>(_toolbarActionItem.getAction()), toolbarActionItem.getWidgetFlags(), ToolbarActionItem::State::Collapsed),
    _expandedWidget(this, const_cast<WidgetAction*>(_toolbarActionItem.getAction()), toolbarActionItem.getWidgetFlags(), ToolbarActionItem::State::Expanded)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSizeConstraint(QLayout::SetFixedSize);

    layout->addWidget(&_collapsedWidget);
    layout->addWidget(&_expandedWidget);

    setLayout(layout);

    const auto updateState = [this]() -> void {
        switch (_toolbarActionItem.getState())
        {
            case ToolbarActionItem::State::Collapsed:
            {
                _collapsedWidget.setVisible(true);
                _expandedWidget.setVisible(false);

                break;
            }

            case ToolbarActionItem::State::Expanded:
            {
                _collapsedWidget.setVisible(false);
                _expandedWidget.setVisible(true);

                break;
            }

            default:
                break;
        }

        _toolbarActionItem.setChangingState(false);
    };

    updateState();

    connect(&_toolbarActionItem, &ToolbarActionItem::stateChanged, this, updateState);

    QCoreApplication::processEvents();

    synchronizeWidgetSize(ToolbarActionItem::State::Collapsed);
    synchronizeWidgetSize(ToolbarActionItem::State::Expanded);

    _collapsedWidget.installEventFilter(this);
    _expandedWidget.installEventFilter(this);
}

bool ToolbarActionItemWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            const auto resizeEvent = static_cast<QResizeEvent*>(event);

            if (target == &_collapsedWidget)
                synchronizeWidgetSize(ToolbarActionItem::State::Collapsed);

            if (target == &_expandedWidget)
                synchronizeWidgetSize(ToolbarActionItem::State::Expanded);

            break;
        }

        default:
            break;
    }

    return QWidget::eventFilter(target, event);
}

void ToolbarActionItemWidget::synchronizeWidgetSize(const ToolbarActionItem::State& state)
{
    switch (state) {
        case ToolbarActionItem::State::Collapsed:
            _toolbarActionItem.setWidgetSize(_collapsedWidget.sizeHint(), state);
            break;

        case ToolbarActionItem::State::Expanded:
            _toolbarActionItem.setWidgetSize(_expandedWidget.sizeHint(), state);
            break;

        default:
            break;
    }
}

ToolbarActionItemWidget::StateWidget::StateWidget(QWidget* parent, WidgetAction* action, std::int32_t widgetFlags, const ToolbarActionItem::State& state) :
    QWidget(parent)//,
    //_widgetFader(this, this, 1.0f, 0.0f, 1.0f, 500, 1000)
{
    setObjectName("StateWidget");

    auto layout = new QVBoxLayout();

    setStyleSheet("QWidget#StateWidget { background-color: rgba(150, 150, 150, 25); }");

    QWidget* widget = nullptr;

    switch (state) {
        case ToolbarActionItem::State::Collapsed:
            widget = action->createCollapsedWidget(this);
            break;

        case ToolbarActionItem::State::Expanded:
            widget = action->createWidget(this, widgetFlags);
            break;

        default:
            break;
    }

    /*
    QFont labelFont("Courier", 7, 300);

    QFontMetrics metrics(labelFont);

    auto label = new QLabel(this);

    label->setFont(labelFont);
    label->setAlignment(Qt::AlignCenter);

    if (state == ToolbarActionItem::State::Expanded)
        label->setText(metrics.elidedText(action->text(), Qt::ElideRight, widget->width()));

    layout->addWidget(label);
    */

    layout->setContentsMargins(4, 4, 4, 4);
    layout->addWidget(widget);
    
    setLayout(layout);
}

void ToolbarActionItemWidget::StateWidget::setVisible(bool visible)
{
    //if (visible)
    //    _widgetFader.setOpacity(1.0f, 250);
    //else
    //    _widgetFader.setOpacity(0.0f);

    QWidget::setVisible(visible);
}

}
