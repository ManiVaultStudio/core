#include "ImportClustersAction.h"
#include "ClustersAction.h"
#include "ClusterData.h"
#include "ClustersActionWidget.h"

#include <QFileDialog>
#include <QJsonDocument>

ImportClustersAction::ImportClustersAction(ClustersActionWidget* clustersActionWidget) :
    TriggerAction(clustersActionWidget),
    _clustersActionWidget(clustersActionWidget)
{
    setText("Import");
    setToolTip("Import clusters from file");
    setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));

    connect(this, &TriggerAction::triggered, this, [this]() {

        try
        {
            QFileDialog fileDialog;

            // Configure file dialog
            fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
            fileDialog.setNameFilters({ "Cluster JSON files (*json)" });
            fileDialog.setDefaultSuffix(".json");

            // Show the dialog
            if (fileDialog.exec() == 0)
                return;

            if (fileDialog.selectedFiles().count() != 1)
                return;

            QFile clustersFile;

            // Load the file
            clustersFile.setFileName(fileDialog.selectedFiles().first());
            clustersFile.open(QIODevice::ReadOnly);

            // Get the cluster data
            QByteArray clustersData = clustersFile.readAll();

            QJsonDocument clusterJsonDocument;

            // Convert to JSON document
            clusterJsonDocument = QJsonDocument::fromJson(clustersData);

            // Get the clusters dataset from the clusters action
            auto clustersDataset = _clustersActionWidget->getClustersAction().getClustersDataset();

            // Load in the cluster from variant data
            clustersDataset->fromVariant(clusterJsonDocument.toVariant());

            // Let others know that the clusters changed
            Application::core()->notifyDataChanged(clustersDataset);
        }
        catch (std::exception& e)
        {
            QMessageBox::critical(nullptr, QString("Unable to import clusters"), e.what(), QMessageBox::Ok);
        }
    });
}