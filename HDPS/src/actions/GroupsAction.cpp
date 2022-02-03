#include "GroupsAction.h"
#include "Application.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QItemDelegate>
#include <QGroupBox>

namespace hdps {

namespace gui {

    class GroupWidgetDelegate : public QItemDelegate
    {
    public:
        GroupWidgetDelegate(GroupsAction* groupsAction, QObject* parent = 0) :
            QItemDelegate(parent),
            _groupsAction(groupsAction)
        {
        };

        QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
        {
            auto groupAction = _groupsAction->getGroupActions()[index.parent().isValid() ? index.parent().row() : index.row()];

            if (!index.parent().isValid()) {
                auto sectionPushButton = new QPushButton(groupAction->text(), parent);

                connect(sectionPushButton, &QPushButton::clicked, groupAction, &GroupAction::toggle);

                return sectionPushButton;
            }

            //emit layoutChanged();

            //auto widget = groupAction->createWidget(parent);
            auto widget = new QGroupBox(parent);

            //widget->setCheckable(true);
            //widget->setFixedHeight(50);

            //connect(widget, &QGroupBox::toggled, this, [widget](bool toggled) {
            //    widget->setFixedHeight(toggled ? 200 : 50);
            //    widget->updateGeometry();
            //});

            return widget;
        }

        /*
        void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem& option, const QModelIndex &index) const
        {
            if (index.parent().isValid())
                editor->setFixedSize(option.widget->sizeHint());
        }
        */

    private:
        GroupsAction*   _groupsAction;      /** Pointer to widget action group */
    };

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
    treeWidget->setItemDelegateForColumn(0, new GroupWidgetDelegate(groupsAction));

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
            auto sectionTreeWidgetItem  = new QTreeWidgetItem();
            treeWidget->addTopLevelItem(sectionTreeWidgetItem);

            auto groupTreeWidgetItem    = new GroupTreeItem(sectionTreeWidgetItem, groupAction);

            

            // Add the item as a top level item
            
            //treeWidget->openPersistentEditor(sectionTreeWidgetItem);
            //treeWidget->openPersistentEditor(groupTreeWidgetItem);

            
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

            // Create the section expand/collapse button
            //auto button = new SectionPushButton(treeWidgetItem, groupAction, groupAction->text());

            // Assign the button to the top-level tree widget item
            //treeWidget->setItemWidget(treeWidgetItem, 0, button);

            //connect(groupAction)
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
    _groupTreeWidgetItem(nullptr),
    _parentTreeWidgetItem(treeWidgetItem),
    _groupWidget(nullptr),
    _overlayWidget(this),
    _overlayLayout(),
    _iconLabel(),
    _settingsLabel(),
    _widgetActionOptions(groupAction, &_settingsLabel)
{
    setFixedHeight(22);
    //setContentsMargins(0, 0, 0, 10);

    // Get reference to the Font Awesome icon font
    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _overlayWidget.setLayout(&_overlayLayout);
    _overlayWidget.raise();

    _overlayLayout.setContentsMargins(10, 6, 10, 6);
    _overlayLayout.addWidget(&_iconLabel);
    _overlayLayout.addStretch(1);

    if (_widgetActionGroup->isSerializable())
        _overlayLayout.addWidget(&_settingsLabel);

    _iconLabel.setAlignment(Qt::AlignCenter);
    _iconLabel.setFont(fontAwesome.getFont(7));
    _settingsLabel.setFont(fontAwesome.getFont(7));

    // Install event filter to synchronize overlay widget size with push button size
    installEventFilter(this);

    // Toggle the section expansion when the section push button is clicked
    connect(this, &QPushButton::clicked, this, [this]() {
        if (!_settingsLabel.rect().contains(QCursor::pos()))
            _widgetActionGroup->toggle();
    });

    // Update the state of the push button when the group action changes
    const auto update = [this, &fontAwesome]() -> void {
        if (_widgetActionGroup->isExpanded()) {
            _parentTreeWidgetItem->setExpanded(true);

            //updateGroupWidget();
        }
        else {

            // Set the section as collapsed and remove the group tree widget item
            _parentTreeWidgetItem->setExpanded(false);
            
            //removeGroupWidget();
        }

        // Assign the icon characters
        _iconLabel.setText(fontAwesome.getIconCharacter(_widgetActionGroup->isExpanded() ? "angle-down" : "angle-right"));
        _settingsLabel.setText(fontAwesome.getIconCharacter("ellipsis-h"));
    };

    const auto updateText = [this, groupAction]() -> void {
        setText(groupAction->text());
        //_treeWidgetItem->setHidden(!groupAction->isVisible());
    };

    connect(groupAction, &WidgetAction::changed, this, updateText);

    // Update when the group action is expanded or collapsed
    connect(_widgetActionGroup, &GroupAction::expanded, this, update);
    connect(_widgetActionGroup, &GroupAction::collapsed, this, update);

    update();
}

bool GroupsAction::Widget::SectionPushButton::eventFilter(QObject* target, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Resize:
        {
            if (target == this)
                _overlayWidget.setFixedSize(static_cast<QResizeEvent*>(event)->size());
            
            if (target == _groupWidget) {
                //updateGroupWidget();
                //if (_groupActionWidget)
                //    delete _groupActionWidget;

                //// Create group action widget on-the-fly
                //_groupActionWidget = _widgetActionGroup->createWidget(this);

                //_treeWidgetItem->treeWidget()->setItemWidget(_sectionItem, 0, _groupActionWidget);

                qDebug() << _groupWidget->size();
                //_groupActionWidget->updateGeometry();
                //_treeWidgetItem->setSizeHint(0, _groupActionWidget->sizeHint());
                //_treeWidgetItem->treeWidget()->updateGeometry();
            }

            break;
        }

        default:
            break;
    }

    return QPushButton::eventFilter(target, event);
}

void GroupsAction::Widget::SectionPushButton::updateGroupWidget()
{
    // Create group action widget on-the-fly
    _groupWidget = _widgetActionGroup->createWidget(this);

    // Get notified when the group widget resizes
    _groupWidget->installEventFilter(this);

    QStyleOption styleOption;

    styleOption.init(_groupWidget);

    // Create new tree widget item for the group action widget
    _groupTreeWidgetItem = new QTreeWidgetItem(_parentTreeWidgetItem);

    // Set group background color
    _groupTreeWidgetItem->setBackground(0, styleOption.palette.color(QPalette::Normal, QPalette::Button));

    // Add the item as a child and assign the group action widget
    _parentTreeWidgetItem->addChild(_groupTreeWidgetItem);

    _parentTreeWidgetItem->treeWidget()->setItemWidget(_groupTreeWidgetItem, 0, _groupWidget);
}

void GroupsAction::Widget::SectionPushButton::removeGroupWidget()
{
    // Remove our tree widget item from the parent
    _parentTreeWidgetItem->removeChild(_groupTreeWidgetItem);

    delete _groupTreeWidgetItem;
    delete _groupWidget;

    _groupTreeWidgetItem = nullptr;
    _groupWidget = nullptr;
}

}
}
