#include "DataGroupingAction.h"
#include "Core.h"
#include "Application.h"

#include <QMenu>
#include <QDialogButtonBox>

namespace hdps {

using namespace gui;

DataGroupingAction::DataGroupingAction(QObject* parent, const Dataset<DatasetImpl>& dataset) :
    WidgetAction(parent),
    _dataset(dataset),
    _assignAction(this, "Assign"),
    _removeAction(this, "Remove"),
    _removeRecursivelyAction(this, "Remove recursively")
{
    setText("Grouping");
    setIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));

    _dataset->getDataHierarchyItem().addAction(*this);

    _assignAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plus"));
    _removeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("trash-alt"));
    _removeRecursivelyAction.setIcon(Application::getIconFont("FontAwesome").getIcon("trash-alt"));

    // Assign group indices when the assign action is triggered
    connect(&_assignAction, &TriggerAction::triggered, this, [this]() {

        // Create grouping dialog
        GroupingDialog groupingDialog(nullptr, _dataset);

        // Show the dialog
        groupingDialog.exec();
    });

    // Remove when the remove action is triggered
    connect(&_removeAction, &TriggerAction::triggered, this, [this]() {
        _dataset->setGroupIndex(-1);
    });

    // Remove recursively when the remove recursively action is triggered
    connect(&_removeRecursivelyAction, &TriggerAction::triggered, this, [this]() {

        // Remove selected
        _dataset->setGroupIndex(-1);

        // Get dataset children
        const auto children = _dataset->getDataHierarchyItem().getChildren();

        // Loop over all children and remove group index
        for (const auto& child : children)
            child->getDataset()->setGroupIndex(-1);
    });
}

QMenu* DataGroupingAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    if (!Application::core()->isDatasetGroupingEnabled())
        return nullptr;

    auto menu = new QMenu(text(), parent);

    menu->setIcon(icon());

    menu->addAction(&_assignAction);

    menu->addSeparator();

    menu->addAction(&_removeAction);
    menu->addAction(&_removeRecursivelyAction);

    _removeAction.setEnabled(_dataset->getGroupIndex() >= 0);
    _removeRecursivelyAction.setEnabled(mayRemoveRecursively());

    return menu;
}

bool DataGroupingAction::mayRemoveRecursively() const
{
    // Get dataset children
    const auto children = _dataset->getDataHierarchyItem().getChildren();

    // Loop over all children and see if the group index is removal
    for (const auto& child : children)
        if (child->getDataset()->getGroupIndex() >= 0)
            return true;

    return false;
}

DataGroupingAction::GroupingDialog::GroupingDialog(QWidget* parent, const Dataset<DatasetImpl>& dataset) :
    QDialog(parent),
    _dataset(dataset),
    _groupIndexAction(this, "Start index", 0, 100, 0, 0),
    _recursiveAction(this, "Recursively", true, true),
    _applyAction(this, "Apply"),
    _cancelAction(this, "Cancel")
{
    // Exit if no dataset is present
    if (!_dataset.isValid())
        return;

    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("object-group"));
    setModal(true);

    // Updates the window title based on the grouping configuration
    const auto updateWindowTitle = [this]() -> void {

        if (_recursiveAction.isChecked()) {

            // Get dataset children
            const auto children = _dataset->getDataHierarchyItem().getChildren(_recursiveAction.isChecked());

            // Get number of children
            const auto numberOfChildren = children.count();

            if (numberOfChildren == 1)
                setWindowTitle(QString("Set group ID for  %1 and 1 child").arg(_dataset->getGuiName()));

            if (numberOfChildren > 1)
                setWindowTitle(QString("Set group ID for  %1 and %2 children").arg(_dataset->getGuiName(), QString::number(numberOfChildren)));
        }
        else {
            setWindowTitle(QString("Set group ID for %1").arg(_dataset->getGuiName()));
        }
    };
    
    auto layout = new QGridLayout();

    auto groupIndexLabelWidget  = _groupIndexAction.createLabelWidget(this);
    auto groupIndexWidget       = _groupIndexAction.createWidget(this);
    auto recursiveWidget        = _recursiveAction.createWidget(this);

    // Add the widget to the layout
    layout->addWidget(groupIndexLabelWidget, 0, 0);
    layout->addWidget(groupIndexWidget, 0, 1);
    layout->addWidget(recursiveWidget, 1, 1);

    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 2, 0, 1, 2);

    setLayout(layout);

    // Create dialog button box so that the user can proceed or cancel with the conversion
    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Apply");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

    // Add buttons to the layout
    layout->addWidget(dialogButtonBox, 3, 0, 1, 2);

    // Handle when accepted
    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, [this]() {

        // Start group index
        std::int32_t groupIndex = _groupIndexAction.getValue();

        // Apply start group index to root dataset
        _dataset->setGroupIndex(groupIndex);

        if (_groupIndexAction.getValue() >= 0)
            groupIndex++;

        if (_recursiveAction.isChecked()) {

            // Get dataset children
            const auto children = _dataset->getDataHierarchyItem().getChildren();

            // Loop over all children and assign an increasing group index
            for (const auto& child : children) {

                // Assign the group index
                child->getDataset()->setGroupIndex(groupIndex);

                // Increase the group index by one
                if (_groupIndexAction.getValue() >= 0)
                    groupIndex++;
            }
        }

        accept();
    });

    // Handle when rejected
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &GroupingDialog::reject);

    // Update the window title when the recursive option is toggled
    connect(&_recursiveAction, &ToggleAction::toggled, this, updateWindowTitle);

    // Initial update of the window title
    updateWindowTitle();
}

}