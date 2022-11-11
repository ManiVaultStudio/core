#include "DataRemoveAction.h"
#include "DataHierarchyItem.h"
#include "DataHierarchyManager.h"
#include "Application.h"

#include <QMenu>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QListView>
#include <QVBoxLayout>
#include <QLabel>

namespace hdps {

using namespace gui;

DataRemoveAction::DataRemoveAction(QObject* parent, const Dataset<DatasetImpl>& dataset) :
    TriggerAction(parent),
    _dataset(dataset)
{
    setText("Remove");
    setIcon(Application::getIconFont("FontAwesome").getIcon("trash-alt"));

    // Remove dataset and children when triggered
    connect(this, &TriggerAction::triggered, this, [this]() {

        // Dataset to remove
        QVector<Dataset<DatasetImpl>> datasetsToRemove({ _dataset });

        // Ask for confirmation if necessary
        if (Application::current()->getSetting("ConfirmDataRemoval", true).toBool()) {

            // Ask for confirmation dialog
            ConfirmDataRemoveDialog confirmDataRemoveDialog(nullptr, "Remove dataset(s)", datasetsToRemove);

            // Show the confirm data removal dialog
            confirmDataRemoveDialog.exec();

            // Remove dataset and children from the core if accepted
            if (confirmDataRemoveDialog.result() == 1)
                Application::core()->removeDataset(_dataset);
        }
        else {
            Application::core()->removeDataset(_dataset);
        }
    });
}

DataRemoveAction::ConfirmDataRemoveDialog::ConfirmDataRemoveDialog(QWidget* parent, const QString& windowTitle, const QVector<Dataset<DatasetImpl>>& datasetsToRemove) :
    QDialog(parent),
    _showAgainAction(this, "Ask for confirmation next time"),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel")
{
    // GUI names of the datasets to remove
    QStringList datasetGuiNames;

    // Add dataset GUI names
    for (const auto& dataset : datasetsToRemove)
        datasetGuiNames << dataset->getDataHierarchyItem().getFullPathName();

    // Establish number of datasets to remove
    const auto numberOfDatasetsToRemove = datasetsToRemove.count();

    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("trash-alt"));
    setWindowTitle(windowTitle);
    setModal(true);
    setMinimumWidth(400);
    setMinimumHeight(250);

    // Check whether to show the dialog again or not
    _showAgainAction.setChecked(Application::current()->getSetting("ConfirmDataRemoval", true).toBool());

    auto layout = new QVBoxLayout();

    setLayout(layout);

    // Ask for confirmation question
    const auto confirmationQuestion = QString("You are about to remove %1 dataset%2, are you sure?").arg(QString::number(numberOfDatasetsToRemove), numberOfDatasetsToRemove > 1 ? "s" : "");

    layout->addWidget(new QLabel(confirmationQuestion));

    auto datasetsToRemoveListView = new QListView();

    datasetsToRemoveListView->setVerticalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAsNeeded);
    datasetsToRemoveListView->setModel(reinterpret_cast<QStringListModel*>(new DatasetsStringListModel(datasetGuiNames)));

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