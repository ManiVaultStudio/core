#include "GroupsAction.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QResizeEvent>

namespace hdps {

namespace gui {

GroupsAction::GroupsAction(QObject* parent) :
    WidgetAction(parent),
    _groupActions()
{
}

void GroupsAction::set(const GroupActions& groupActions)
{
    _groupActions = groupActions;

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
            auto treeWidgetItem = new QTreeWidgetItem();

            // Add the item as a top level item
            treeWidget->addTopLevelItem(treeWidgetItem);

            // Create the section expand/collapse button
            auto button = new SectionPushButton(treeWidgetItem, groupAction, groupAction->text());

            // Assign the button to the top-level tree widget item
            treeWidget->setItemWidget(treeWidgetItem, 0, button);
        }
    };

    // Change the accordion section when the groups action changes
    connect(groupsAction, &GroupsAction::changed, this, update);

    // Do an initial update
    update();
}

GroupsAction::Widget::SectionPushButton::SectionPushButton(QTreeWidgetItem* treeWidgetItem, GroupAction* groupAction, const QString& text, QWidget* parent /*= nullptr*/) :
    QPushButton(text, parent),
    _widgetActionGroup(groupAction),
    _treeWidgetItem(treeWidgetItem)
{
    auto frameLayout    = new QHBoxLayout();
    auto iconLabel      = new QLabel();

    iconLabel->setAlignment(Qt::AlignCenter);

    frameLayout->setMargin(5);
    frameLayout->addWidget(iconLabel);
    frameLayout->addStretch(1);

    setLayout(frameLayout);

    // Toggle the section expansion when the section push button is clicked
    connect(this, &QPushButton::clicked, this, [this]() {
        _widgetActionGroup->toggle();
    });

    // Update the state of the push button when the group action changes
    const auto update = [this, iconLabel]() -> void {
        if (_widgetActionGroup->isExpanded()) {
            _treeWidgetItem->setExpanded(true);

            // Create group action widget on-the-fly
            auto groupActionWidget = _widgetActionGroup->createWidget(this);

            QStyleOption styleOption;

            styleOption.init(groupActionWidget);

            // Create new tree widget item for the group action widget
            auto section = new QTreeWidgetItem(_treeWidgetItem);

            // Set group background color
            section->setBackground(0, styleOption.palette.color(QPalette::Normal, QPalette::Button));

            // Add the item as a child and assign the group action widget
            _treeWidgetItem->addChild(section);
            _treeWidgetItem->treeWidget()->setItemWidget(section, 0, groupActionWidget);
        }
        else {

            // Set the section as collapsed and remove the group tree widget item
            _treeWidgetItem->setExpanded(false);
            _treeWidgetItem->removeChild(_treeWidgetItem->child(0));
        }

        // Pick icon that corresponds to the group action state
        const auto iconName = _widgetActionGroup->isExpanded() ? "angle-down" : "angle-right";
        const auto icon     = Application::getIconFont("FontAwesome").getIcon(iconName);

        // Assign the icon
        iconLabel->setPixmap(icon.pixmap(QSize(12, 12)));
    };

    // Update when the group action is expanded or collapsed
    connect(_widgetActionGroup, &GroupAction::expanded, this, update);
    connect(_widgetActionGroup, &GroupAction::collapsed, this, update);

    /*
    // Update the state of the reset push button
    const auto updateResetPushButton = [this, groupAction, resetToolButton]() -> void {
        auto isResettable = false;

        for (auto child : groupAction->children()) {
            auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

            if (childWidgetAction == nullptr)
                continue;

            if (childWidgetAction->isResettable())
                isResettable = true;
        }

        resetToolButton->setEnabled(isResettable);
    };

    // Walk over all child actions
    for (auto child : groupAction->children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (childWidgetAction == nullptr)
            continue;

        connect(childWidgetAction, &WidgetAction::isResettableChanged, this, updateResetPushButton);
    }
    */

    update();
    //updateResetPushButton();
}

}
}
