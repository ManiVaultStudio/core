#include "GroupsAction.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QTreeWidget>

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
    auto treeWidget = new QTreeWidget();

    // Configure tree widget
    treeWidget->setHeaderHidden(true);
    treeWidget->setIndentation(0);
    treeWidget->setAutoFillBackground(true);
    treeWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    auto layout = new QVBoxLayout();

    setLayout(layout);

    layout->setMargin(0);
    layout->setAlignment(Qt::AlignTop);
    layout->addWidget(treeWidget);

    // Change the accordion section when the groups action changes
    connect(groupsAction, &GroupsAction::changed, this, [this, groupsAction, treeWidget]() {

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
    });
}

GroupsAction::Widget::SectionPushButton::SectionPushButton(QTreeWidgetItem* treeWidgetItem, GroupAction* groupAction, const QString& text, QWidget* parent /*= nullptr*/) :
    QPushButton(text, parent),
    _widgetActionGroup(groupAction),
    _treeWidgetItem(treeWidgetItem)
{
    auto frameLayout    = new QHBoxLayout();
    auto iconLabel      = new QLabel();

    frameLayout->addWidget(iconLabel);

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
            auto groupWidget = _widgetActionGroup->createWidget(this);

            groupWidget->setAutoFillBackground(true);

            // Create new tree widget item for the group action widget
            auto section = new QTreeWidgetItem(_treeWidgetItem);

            // Add the item as a child and assign the group action widget
            _treeWidgetItem->addChild(section);
            _treeWidgetItem->treeWidget()->setItemWidget(section, 0, groupWidget);
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


    update();
}

}
}
