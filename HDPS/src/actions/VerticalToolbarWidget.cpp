#include "VerticalToolbarWidget.h"
#include "ToolbarAction.h"
#include "ToolbarActionWidget.h"

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
    for (auto toolbarItem : _toolbarAction.getItems())
        _toolbarItemWidgets << SharedToolbarItemWidget(new ToolbarActionWidget(this, *toolbarItem.getAction(), toolbarItem.getPriority()));

    for (auto toolbarItemWidget : _toolbarItemWidgets)
        _toolbarLayout.addWidget(toolbarItemWidget.get());

    _toolbarWidget.setLayout(&_toolbarLayout);
    
    
    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);

    _layout.setMargin(0);
    _layout.addWidget(&_toolbarWidget);
    _layout.addStretch(1);

    setLayout(&_layout);

    //setStyleSheet("background-color: red;");
}

void VerticalToolbarWidget::computeLayout(ToolbarItemWidget* ignoreToolbarItemWidget /*= nullptr*/)
{
    qDebug() << "Compute toolbar widget layout" << width();

    auto toolbarActionWidgets = getToolbarActionWidgets();

    QVector<std::int32_t> states;

    states.resize(toolbarActionWidgets.count());

    // Initialize collapsed
    std::fill(states.begin(), states.end(), ToolbarActionWidget::State::Collapsed);

    for (auto toolbarActionWidget : toolbarActionWidgets)
        toolbarActionWidget->setState(states[toolbarActionWidgets.indexOf(toolbarActionWidget)]);
}

}
}
