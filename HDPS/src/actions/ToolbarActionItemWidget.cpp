#include "ToolbarActionItemWidget.h"
#include "ToolbarActionItem.h"
#include "WidgetAction.h"

#include <QHBoxLayout>

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

    _collapsedWidget    = nonConstAction->createCollapsedWidget(this);
    _expandedWidget     = nonConstAction->createWidget(this);

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
}

}
