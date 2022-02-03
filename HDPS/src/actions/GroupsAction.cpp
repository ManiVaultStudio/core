#include "GroupsAction.h"
#include "GroupSectionTreeItem.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>

namespace hdps {

namespace gui {

GroupsAction::GroupsAction(QObject* parent) :
    WidgetAction(parent),
    _groupActions()
{
}

bool GroupsAction::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            break;
        }

        default:
            break;
    }

    return WidgetAction::eventFilter(target, event);
}

void GroupsAction::set(const GroupActions& groupActions)
{
    // Remove previous connections
    for (auto groupAction : _groupActions) {
        disconnect(groupAction, &GroupAction::expanded, this, nullptr);
        disconnect(groupAction, &GroupAction::collapsed, this, nullptr);
    }

    _groupActions = groupActions;

    // Pass through group action expanded/collapsed signals
    for (auto groupAction : _groupActions) {

        // Pass through group action expanded signal
        connect(groupAction, &GroupAction::expanded, this, [this, groupAction]() -> void {
            emit expanded(groupAction);
        });

        // Pass through group action collapsed signal
        connect(groupAction, &GroupAction::collapsed, this, [this, groupAction]() -> void {
            emit collapsed(groupAction);
        });
    }

    emit changed(_groupActions);
}

const hdps::gui::GroupsAction::GroupActions& GroupsAction::getGroupActions() const
{
    return _groupActions;
}

GroupsAction::Widget::Widget(QWidget* parent, GroupsAction* groupsAction) :
    WidgetActionWidget(parent, groupsAction, WidgetActionWidget::State::Standard)
{
    setObjectName("GroupsAction");

    auto treeWidget = new QTreeWidget();

    // Configure tree widget
    treeWidget->setHeaderHidden(true);
    treeWidget->setIndentation(0);
    treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    treeWidget->setSelectionMode(QAbstractItemView::NoSelection);
    treeWidget->setUniformRowHeights(false);
    treeWidget->setEditTriggers(QTreeWidget::NoEditTriggers);

    treeWidget->setStyleSheet("QTreeWidget { border: none; }");

    QStyleOption styleOption;

    styleOption.init(treeWidget);

    auto palette = treeWidget->palette();

    palette.setColor(QPalette::Base, styleOption.palette.color(QPalette::Normal, QPalette::Button));

    treeWidget->setPalette(palette);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(treeWidget);

    const auto update = [this, treeWidget, groupsAction]() {

        // Clear previous sections
        treeWidget->clear();

        for (auto groupAction : groupsAction->getGroupActions()) {

            // Only add visible group actions
            if (!groupAction->isVisible())
                continue;

            // Create new tree widget item for the section expand/collapse button
            auto sectionTreeWidgetItem  = new GroupSectionTreeItem(treeWidget, groupAction);
            
            sectionTreeWidgetItem->setExpanded(groupAction->isExpanded());

            // Expand section tree widget item then the group action is expanded
            connect(groupAction, &GroupAction::expanded, this, [treeWidget, groupAction, &groupsAction]() -> void {
                const auto row = groupsAction->getGroupActions().indexOf(groupAction);
                treeWidget->setExpanded(treeWidget->model()->index(row, 0), true);
            });

            // Collapse section tree widget item then the group action is collapsed
            connect(groupAction, &GroupAction::collapsed, this, [treeWidget, groupAction, &groupsAction]() -> void {
                const auto row = groupsAction->getGroupActions().indexOf(groupAction);
                treeWidget->setExpanded(treeWidget->model()->index(row, 0), false);
            });
        }
    };

    // Change the accordion section when the groups action changes
    connect(groupsAction, &GroupsAction::changed, this, update);

    // Do an initial update
    update();
}

}
}
