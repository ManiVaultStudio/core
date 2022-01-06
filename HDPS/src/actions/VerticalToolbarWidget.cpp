#include "VerticalToolbarWidget.h"
#include "ToolbarAction.h"
#include "ToolbarActionWidget.h"
#include "ToolbarSpacerWidget.h"

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
    //setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    for (auto toolbarItem : _toolbarAction.getItems()) {
        _toolbarItemWidgets << SharedToolbarItemWidget(new ToolbarActionWidget(this, *toolbarItem.getAction(), toolbarItem.getPriority()));
        _toolbarItemWidgets << SharedToolbarItemWidget(new ToolbarSpacerWidget(this));
    }

    _toolbarItemWidgets << SharedToolbarItemWidget(new ToolbarActionWidget(this, _toolbarHiddenItemsAction, -1));

    for (auto toolbarItemWidget : _toolbarItemWidgets)
        _toolbarLayout.addWidget(toolbarItemWidget.get());

    _toolbarWidget.setLayout(&_toolbarLayout);

    _toolbarLayout.setSizeConstraint(QLayout::SetFixedSize);
    _toolbarLayout.setMargin(0);
    _toolbarLayout.setSpacing(0);

    _layout.setMargin(0);
    _layout.addWidget(&_toolbarWidget);
    _layout.addStretch(1);

    setLayout(&_layout);

    //setStyleSheet("background-color: red;");
}

void VerticalToolbarWidget::computeLayout(ToolbarItemWidget* ignoreToolbarItemWidget /*= nullptr*/)
{
    qDebug() << "Compute toolbar widget layout" << height();

    auto toolbarActionWidgets = getToolbarActionWidgets();
    auto toolbarSpacerWidgets = getToolbarSpacerWidgets();

    // Remove the hidden items actions
    toolbarActionWidgets.removeLast();

    QVector<std::int32_t> states;

    states.resize(toolbarActionWidgets.count());

    // Initialize collapsed
    std::fill(states.begin(), states.end(), ToolbarActionWidget::State::Hidden);

    // Compute height for candidate state configuration 
    const auto getHeight = [this, &states, toolbarActionWidgets]() -> std::int32_t {
        std::int32_t height = 0;

        for (auto toolbarActionWidget : toolbarActionWidgets)
            height += toolbarActionWidget->getSize(states[toolbarActionWidgets.indexOf(toolbarActionWidget)]).height();

        //for (auto toolbarSpacerWidget : toolbarSpacerWidgets)
        //    height += toolbarSpacerWidget->getSize(states[toolbarActionWidgets.indexOf(toolbarActionWidget)]).height();

        return height;
    };

    // Compute toolbar item states
    for (auto toolbarActionWidget : toolbarActionWidgets) {
        //if (toolbarActionWidget == toolbarActionWidgets.last())
        //    break;

        const auto cachedStates             = states;
        const auto toolbarActionWidgetIndex = toolbarActionWidgets.indexOf(toolbarActionWidget);

        states[toolbarActionWidgetIndex] = ToolbarActionWidget::State::Collapsed;

        if (getHeight() > parentWidget()->height()) {
            states = cachedStates;
            break;
        }
    }

    //for (auto toolbarActionWidget : toolbarActionWidgets) {
    //    const auto toolbarActionWidgetIndex = toolbarActionWidgets.indexOf(toolbarActionWidget);

    //    if (states[toolbarActionWidgetIndex] == ToolbarActionWidget::State::Hidden)
    //        states[std::max(0, toolbarActionWidgetIndex)] = ToolbarActionWidget::State::Hidden;
    //}

    const auto numberOfHiddenActionWidgets = std::count_if(states.begin(), states.end(), [](auto state) {
        return state == ToolbarActionWidget::State::Hidden;
    });

    getToolbarActionWidgets().last()->setState(numberOfHiddenActionWidgets > 0 ? ToolbarActionWidget::State::Collapsed : ToolbarActionWidget::State::Hidden);

    for (auto toolbarActionWidget : toolbarActionWidgets)
        toolbarActionWidget->setState(states[toolbarActionWidgets.indexOf(toolbarActionWidget)]);

    for (auto toolbarSpacerWidget : toolbarSpacerWidgets) {
        const auto toolbarSpacerWidgetIndex = toolbarSpacerWidgets.indexOf(toolbarSpacerWidget);
        const auto leftState                = static_cast<ToolbarActionWidget::State>(states[toolbarSpacerWidgetIndex]);
        const auto rightState               = static_cast<ToolbarActionWidget::State>(states[toolbarSpacerWidgetIndex]);

        toolbarSpacerWidget->setState(ToolbarSpacerWidget::getState(Qt::Horizontal, leftState, rightState));
    }

    //qDebug() << states;
}

}
}
