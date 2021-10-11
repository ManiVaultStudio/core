#include "DataRemoveAction.h"
#include "DataHierarchyItem.h"
#include "DataHierarchyManager.h"
#include "Application.h"

#include <QMenu>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListView>

namespace hdps {

using namespace gui;

DataRemoveAction::DataRemoveAction(QObject* parent, const QString& datasetName) :
    WidgetAction(parent),
    _dataset(datasetName),
    _removeSelectedAction(this, "Selected"),
    _removeSelectedAndChildrenAction(this, "Selected + descendants")
{
    setText("Remove");
    setIcon(Application::getIconFont("FontAwesome").getIcon("trash-alt"));

    _removeSelectedAction.setIcon(Application::getIconFont("FontAwesome").getIcon("mouse-pointer"));
    _removeSelectedAndChildrenAction.setIcon(Application::getIconFont("FontAwesome").getIcon("sitemap"));
    
    _dataset->getHierarchyItem().addAction(*this);

    const auto removeDataset = [this](const bool& recursively = false) -> void {
        QStringList datasetsToRemove({ _dataset.getDatasetName() });

        auto& dataHierarchyManager = Application::core()->getDataHierarchyManager();

        if (recursively) {
            auto children = dataHierarchyManager.getChildren(&_dataset->getHierarchyItem());

            for (auto child : children)
                datasetsToRemove << child->getDatasetName();
        }

        if (Application::current()->getSetting("ConfirmDataRemoval", true).toBool()) {
            ConfirmDataRemoveDialog confirmDataRemoveDialog(nullptr, datasetsToRemove);

            // Show the confirm data removal dialog
            confirmDataRemoveDialog.exec();

            // Remove data if accepted
            if (confirmDataRemoveDialog.result() == 1)
                dataHierarchyManager.removeItem(_dataset.getDatasetName(), recursively);
        }
        else {
            dataHierarchyManager.removeItem(_dataset.getDatasetName(), recursively);
        }

        Application::core()->removeDatasets(datasetsToRemove);
    };

    connect(&_removeSelectedAction, &TriggerAction::triggered, this, [this, removeDataset]() {
        removeDataset();
    });

    connect(&_removeSelectedAndChildrenAction, &TriggerAction::triggered, this, [this, removeDataset]() {
        removeDataset(true);
    });
}

QMenu* DataRemoveAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    _removeSelectedAndChildrenAction.setEnabled(_dataset->getHierarchyItem().hasChildren());

    auto menu = new QMenu(text(), parent);

    menu->setIcon(icon());

    menu->addAction(&_removeSelectedAction);
    menu->addAction(&_removeSelectedAndChildrenAction);

    return menu;
}

DataRemoveAction::ConfirmDataRemoveDialog::ConfirmDataRemoveDialog(QWidget* parent, const QStringList& datasetsToRemove) :
    QDialog(parent),
    _showAgainAction(this, "Ask for confirmation next time"),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel")
{
    const auto numberOfDatasetsToRemove = datasetsToRemove.count();

    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("trash-alt"));
    setWindowTitle(QString("Remove %1 %2").arg(datasetsToRemove.first(), datasetsToRemove.count() > 1 ? "+ descendants" : ""));
    setModal(true);
    setFixedWidth(400);

    _showAgainAction.setChecked(Application::current()->getSetting("ConfirmDataRemoval", true).toBool());

    auto layout = new QVBoxLayout();

    setLayout(layout);

    const auto confirmationQuestion = QString("You are about to remove %1 dataset%2, are you sure?").arg(QString::number(numberOfDatasetsToRemove), numberOfDatasetsToRemove > 1 ? "s" : "");

    layout->addWidget(new QLabel(confirmationQuestion));

    auto datasetsToRemoveListView = new QListView();

    datasetsToRemoveListView->setFixedHeight(100);
    datasetsToRemoveListView->setEnabled(false);
    datasetsToRemoveListView->setModel(new QStringListModel(datasetsToRemove));

    layout->addWidget(datasetsToRemoveListView);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addWidget(_showAgainAction.createWidget(this));
    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_removeAction.createWidget(this));
    bottomLayout->addWidget(_cancelAction.createWidget(this));

    layout->addLayout(bottomLayout);

    connect(&_showAgainAction, &ToggleAction::toggled, this, [this](bool toggled) {
        Application::current()->setSetting("ConfirmDataRemoval", toggled);
    });

    connect(&_removeAction, &TriggerAction::triggered, [this]() {
        accept();
    });

    connect(&_cancelAction, &TriggerAction::triggered, [this]() {
        reject();
    });
}

}
