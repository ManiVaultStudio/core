#include "DataRemoveAction.h"
#include "DataHierarchyItem.h"
#include "DataHierarchyManager.h"
#include "Application.h"

#include <QMenu>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListView>
#include <QVBoxLayout>
#include <QStringListModel>

namespace hdps {

using namespace gui;

DataRemoveAction::DataRemoveAction(QObject* parent, const Dataset<DatasetImpl>& dataset) :
    TriggerAction(parent),
    _dataset(dataset)
{
    setText("Remove");
    setIcon(Application::getIconFont("FontAwesome").getIcon("trash-alt"));
    setSerializable(false);

    // Remove dataset and children when triggered
    connect(this, &TriggerAction::triggered, this, [this]() {

        // Dataset to remove
        QVector<Dataset<DatasetImpl>> datasetsToRemove({ _dataset });

        // Ask for confirmation if necessary
        if (Application::current()->getSetting("ConfirmDataRemoval", true).toBool()) {

            // Ask for confirmation dialog
            ConfirmDataRemoveDialog confirmDataRemoveDialog(nullptr, datasetsToRemove);

            // Show the confirm data removal dialog
            confirmDataRemoveDialog.exec();

            // Remove dataset and children from the core if accepted
            if (confirmDataRemoveDialog.result() == 1)
                Application::core()->removeDatasets(datasetsToRemove);
        }
        else {
            Application::core()->removeDatasets(datasetsToRemove);
        }
    });
}

DataRemoveAction::ConfirmDataRemoveDialog::ConfirmDataRemoveDialog(QWidget* parent, const QVector<Dataset<DatasetImpl>>& datasetsToRemove) :
    QDialog(parent),
    _showAgainAction(this, "Ask for confirmation next time"),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel")
{
    // GUI names of the datasets to remove
    QStringList datasetGuiNames;

    // Add primary dataset GUI name
    datasetGuiNames << datasetsToRemove.first()->getGuiName();

    // Add child dataset GUI names
    for (const auto& dataset : datasetsToRemove.first()->getDataHierarchyItem().getChildren(true))
        datasetGuiNames << dataset->getGuiName();

    // Establish number of datasets to remove
    const auto numberOfDatasetsToRemove = datasetGuiNames.count();

    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("trash-alt"));
    setWindowTitle(QString("Remove %1 %2").arg(datasetsToRemove.first()->getGuiName(), numberOfDatasetsToRemove > 1 ? "+ descendants" : ""));
    setModal(true);
    setFixedWidth(400);

    // Check whether to show the dialog again or not
    _showAgainAction.setChecked(Application::current()->getSetting("ConfirmDataRemoval", true).toBool());

    auto layout = new QVBoxLayout();

    setLayout(layout);

    // Ask for confirmation question
    const auto confirmationQuestion = QString("You are about to remove %1 dataset%2, are you sure?").arg(QString::number(numberOfDatasetsToRemove), numberOfDatasetsToRemove > 1 ? "s" : "");

    layout->addWidget(new QLabel(confirmationQuestion));

    auto datasetsToRemoveListView = new QListView();

    datasetsToRemoveListView->setFixedHeight(100);
    datasetsToRemoveListView->setEnabled(false);
    datasetsToRemoveListView->setModel(new QStringListModel(datasetGuiNames));

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
