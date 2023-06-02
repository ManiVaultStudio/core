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
    _collapsedWidget(),
    _expandedWidget()//,
    //_collapsedWidgetFader(this, _collapsedWidget),
    //_expandedWidgetFader(this, _expandedWidget)
{
    auto layout         = new QHBoxLayout();
    auto nonConstAction = const_cast<WidgetAction*>(_toolbarActionItem.getAction());

    _collapsedWidget    = new StateWidget(this, nonConstAction, ToolbarActionItem::State::Collapsed);
    _expandedWidget     = new StateWidget(this, nonConstAction, ToolbarActionItem::State::Expanded);

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(_collapsedWidget);
    layout->addWidget(_expandedWidget);

    setLayout(layout);

    const auto updateState = [this]() -> void {
        switch (_toolbarActionItem.getState())
        {
            case ToolbarActionItem::State::Collapsed:
            {
                _collapsedWidget->setVisible(true);
                _expandedWidget->setVisible(false);

                break;
            }

            case ToolbarActionItem::State::Expanded:
            {
                _collapsedWidget->setVisible(false);
                _expandedWidget->setVisible(true);

                break;
            }

            default:
                break;
        }
    };

    updateState();

    connect(&_toolbarActionItem, &ToolbarActionItem::stateChanged, this, updateState);

    QCoreApplication::processEvents();

    synchronizeWidgetSize(ToolbarActionItem::State::Collapsed);
    synchronizeWidgetSize(ToolbarActionItem::State::Expanded);
}

bool ToolbarActionItemWidget::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            const auto resizeEvent = static_cast<QResizeEvent*>(event);

            if (target == _collapsedWidget.get())
                synchronizeWidgetSize(ToolbarActionItem::State::Collapsed);

            if (target == _expandedWidget.get())
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
            _toolbarActionItem.setWidgetSize(_collapsedWidget->sizeHint(), state);
            break;

        case ToolbarActionItem::State::Expanded:
            _toolbarActionItem.setWidgetSize(_expandedWidget->sizeHint(), state);
            break;

        default:
            break;
    }
}

ToolbarActionItemWidget::StateWidget::StateWidget(QWidget* parent, WidgetAction* action, const ToolbarActionItem::State& state) :
    QWidget(parent)
{
    auto layout = new QVBoxLayout();
    auto label  = new QLabel();

    label->setStyleSheet("background-color: gray; font-size: 8px;");

    QWidget* widget = nullptr;

    switch (state) {
        case ToolbarActionItem::State::Collapsed:
            widget = action->createCollapsedWidget(this);
            break;

        case ToolbarActionItem::State::Expanded:
            widget = action->createWidget(this);
            break;

        default:
            break;
    }

    layout->setContentsMargins(0, 0, 0, 0);
    //layout->addWidget(label);
    layout->addWidget(widget);

    QFontMetrics metrics(font());

    label->setText(metrics.elidedText(action->text(), Qt::ElideMiddle, widget->width()));

    setLayout(layout);
}

}
