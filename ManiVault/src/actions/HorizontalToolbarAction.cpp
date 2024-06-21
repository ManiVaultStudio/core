// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HorizontalToolbarAction.h"
#include "WidgetActionExampleWidget.h"

#include <QEvent>
#include <QResizeEvent>

namespace mv::gui {

HorizontalToolbarAction::HorizontalToolbarAction(QObject* parent, const QString& title, const Qt::AlignmentFlag& alignment /*= Qt::AlignmentFlag::AlignLeft*/) :
    ToolbarAction(parent, title, alignment)
{  
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

    _timer.setInterval(300);
    _timer.setSingleShot(true);

    _toolbarLayout.setContentsMargins(ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN, ToolbarAction::CONTENTS_MARGIN);
    _toolbarLayout.setSpacing(0);

    _toolbarWidget.setLayout(&_toolbarLayout);
    _toolbarLayout.setAlignment(horizontalToolbarAction->getGroupAction().getAlignment());
    
    _layout.setContentsMargins(0, 0, 0, 0);
    _layout.addWidget(&_toolbarWidget);

    setLayout(&_layout);

    connect(_horizontalToolbarAction, &ToolbarAction::actionWidgetsChanged, this, &HorizontalToolbarAction::Widget::setActionWidgets);
    connect(_horizontalToolbarAction, &ToolbarAction::layoutInvalidated, this, &Widget::updateLayout);
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

    for (auto actionItem : _horizontalToolbarAction->getActionItems()) {
        auto stretchAction = dynamic_cast<const StretchAction*>(actionItem->getAction());

        if (stretchAction)
            _toolbarLayout.addWidget(actionItem->createWidget(&_toolbarWidget), stretchAction->getStretch());
        else
            _toolbarLayout.addWidget(actionItem->createWidget(&_toolbarWidget));
    }

    updateLayout();
}

void HorizontalToolbarAction::Widget::updateLayout()
{
    /*
    for (auto actionItem : _horizontalToolbarAction->getActionItems()) {
        if (actionItem->isChangingState()) {
            qDebug() << actionItem->getAction()->text() << "is changing state";
            return;
        }
    }
    */

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
}

QWidget* HorizontalToolbarAction::createExampleWidget(QWidget* parent) const
{
    auto exampleWidget              = new WidgetActionExampleWidget(parent);
    auto horizontalToolbarAction    = new HorizontalToolbarAction(exampleWidget, "Example");
    auto colorAction                = new ColorAction(exampleWidget, "Color");
    auto inputAAction               = new StringAction(exampleWidget, "Input A");
    auto inputBAction               = new StringAction(exampleWidget, "Input B");

    inputAAction->setPlaceHolderString("Input first string here...");
    inputBAction->setPlaceHolderString("Input second string here...");

    inputAAction->setStretch(1);
    inputBAction->setStretch(1);

    horizontalToolbarAction->addAction(colorAction);
    horizontalToolbarAction->addAction(inputAAction);
    horizontalToolbarAction->addAction(inputBAction);

    const QString markdownText =
        "## General \n\n"
        "`mv::gui::HorizontalToolbarAction` groups actions `horizontally` and change their visual representation based on the available width. \n\n"
        "## Usage \n\n"
        "[#include <actions/HorizontalGroupAction.h>](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/HorizontalGroupAction.h)\n\n"
        "To use: \n\n"
        "- Create an instance of `HorizontalGroupAction` \n\n"
        "- Add actions using `HorizontalGroupAction::addAction(...)` \n\n"
        "- Remove actions using `HorizontalGroupAction::removeAction(...)` \n\n"
        "## API \n\n"
        "### Alignment \n\n"
        "- `Qt::AlignmentFlag getAlignment() const final` \n\n"
        "  Get the action alignment as a `Qt::AlignmentFlag` (only applies to vertical layouts) \n\n"
        "### Expansion \n\n"
        "- `void setExpanded(const bool& expanded)` \n\n"
        "  Set expansion state to `expanded` (when added to a [GroupsAction](https://github.com/ManiVaultStudio/core/blob/master/ManiVault/src/actions/GroupsAction.h)) \n\n"
        "- `void expand()` \n\n"
        "  Expand the group \n\n"
        "- `void collapse()` \n\n"
        "  Collapse the group \n\n"
        "- `void toggle()` \n\n"
        "  Toggles the expansion state \n\n"
        "- `bool isExpanded() const` \n\n"
        "  Establishes whether the group is in an expanded state or not \n\n"
        "- `bool isCollapsed() const` \n\n"
        "  Establishes whether the group is in an collapsed state or not \n\n"
        "### Read-only \n\n"
        "- `bool isReadOnly() const` \n\n"
        "  Establishes whether the group is in a read-only state or not \n\n"
        "- `void setReadOnly(const bool& readOnly)` \n\n"
        "  Set the read-only state to `readOnly` \n\n"
        "### Label visibility \n\n"
        "- `bool getShowLabels() const` \n\n"
        "  Establishes whether the group shows an action label that precedes the action \n\n"
        "- `void setShowLabels(bool showLabels)` \n\n"
        "  Set show labels to `showLabels` \n\n"
        "### Stretch \n\n"
        "- `StretchAction* addStretch(std::int32_t stretch = 1)` \n\n"
        "  Add `stretch` at the current location n \n\n"
        "### Actions management \n\n"
        "- `void addAction(WidgetAction* action, std::int32_t widgetFlags = -1, WidgetConfigurationFunction widgetConfigurationFunction = WidgetConfigurationFunction())` \n\n"
        "  Add `action` to the group with possibly `widgetFlags` and `widgetConfigurationFunction` \n\n"
        "- `void removeAction(WidgetAction* action) final` \n\n"
        "  Remove `action` from the group \n\n"
        "- `void clear() final` \n\n"
        "  Remove all actions from the group \n\n"
        "- `WidgetActions getActions() final` \n\n"
        "  Get all added actions \n\n"
        "### Signals \n\n"
        "- `void actionsChanged(const WidgetActions& actions)` \n\n"
        "  Signals that the actions changed to `actions` \n\n"
        "- `void expanded()` \n\n"
        "  Signals that the group got expanded \n\n"
        "- `void collapsed()` \n\n"
        "  Signals that the group got collapsed \n\n"
        "- `void readOnlyChanged(const bool& readOnly)` \n\n"
        "  Signals that the read-only status changed to `readOnly` \n\n"
        "- `void showLabelsChanged(const bool& showLabels)` \n\n"
        "  Signals that the show labels state changed to `showLabels` \n\n";

    exampleWidget->addWidget(horizontalToolbarAction->createWidget(exampleWidget));

    auto priorityAction         = new VerticalGroupAction(exampleWidget, "Expand priority");
    auto colorPriorityAction    = new IntegralAction(exampleWidget, "Color", 0, 10, 0);
    auto inputAPriorityAction   = new IntegralAction(exampleWidget, "Input A", 0, 10, 0);
    auto inputBPriorityAction   = new IntegralAction(exampleWidget, "Input B", 0, 10, 0);

    priorityAction->setLabelSizingType(GroupAction::LabelSizingType::Auto);
    priorityAction->setDefaultWidgetFlag(GroupAction::WidgetFlag::NoMargins);

    priorityAction->addAction(colorPriorityAction);
    priorityAction->addAction(inputAPriorityAction);
    priorityAction->addAction(inputBPriorityAction);

    const auto updatePriorityColor = [horizontalToolbarAction, colorPriorityAction]() -> void {
        horizontalToolbarAction->setActionAutoExpandPriority(colorPriorityAction, colorPriorityAction->getValue());
    };

    const auto updatePriorityInputA = [horizontalToolbarAction, inputAPriorityAction]() -> void {
        horizontalToolbarAction->setActionAutoExpandPriority(inputAPriorityAction, inputAPriorityAction->getValue());
    };

    const auto updatePriorityInputB = [horizontalToolbarAction, inputBPriorityAction]() -> void {
        horizontalToolbarAction->setActionAutoExpandPriority(inputBPriorityAction, inputBPriorityAction->getValue());
    };

    updatePriorityColor();
    updatePriorityInputA();
    updatePriorityInputB();

    connect(colorPriorityAction, &IntegralAction::valueChanged, exampleWidget, updatePriorityColor);
    connect(inputAPriorityAction, &IntegralAction::valueChanged, exampleWidget, updatePriorityInputA);
    connect(inputBPriorityAction, &IntegralAction::valueChanged, exampleWidget, updatePriorityInputB);

    exampleWidget->addWidget(priorityAction->createWidget(exampleWidget));

    exampleWidget->addMarkDownSection(markdownText);

    return exampleWidget;
}

}
