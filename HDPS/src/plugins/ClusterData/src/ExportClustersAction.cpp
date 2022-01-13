#include "ExportClustersAction.h"
#include "ClustersAction.h"
#include "ClusterData.h"
#include "ClustersActionWidget.h"

#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>

ExportClustersAction::ExportClustersAction(ClustersActionWidget* clustersActionWidget) :
    TriggerAction(clustersActionWidget),
    _clustersActionWidget(clustersActionWidget)
{
    setText("Export");
    setToolTip("Export clusters to file");
    setIcon(Application::getIconFont("FontAwesome").getIcon("file-export"));

    // Export clusters when the action is triggered
    connect(this, &TriggerAction::triggered, this, [this]() {

        try
        {
            // Create JSON document from clusters variant map
            QJsonDocument document(QJsonArray::fromVariantList(_clustersActionWidget->getClustersAction().getClustersDataset()->toVariant().toList()));

            // Show the dialog
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptSave);
            fileDialog.setNameFilters({ "Cluster JSON files (*json)" });
            fileDialog.setDefaultSuffix(".json");

            if (fileDialog.exec() == 0)
                return;

            // Only save if we have one file
            if (fileDialog.selectedFiles().count() != 1)
                return;

            QFile jsonFile(fileDialog.selectedFiles().first());

            // Save the file
            jsonFile.open(QFile::WriteOnly);
            jsonFile.write(document.toJson());
        }
        catch (std::exception& e)
        {
            QMessageBox::critical(nullptr, QString("Unable to save clusters"), e.what(), QMessageBox::Ok);
        }
    });

    // Update read only status
    const auto updateReadOnly = [this]() -> void {
        setEnabled(_clustersActionWidget->getFilterModel().rowCount() >= 1);
    };

    // Update read only status when the model selection or layout changes
    connect(&_clustersActionWidget->getFilterModel(), &QAbstractItemModel::layoutChanged, this, updateReadOnly);

    // Do an initial update of the read only status
    updateReadOnly();
}
