#include "CreateClusterDialog.h"

#include "CoreInterface.h"
#include "Application.h"
#include "PointData.h"
#include "ClusterData.h"

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace hdps;

CreateClusterDialog::CreateClusterDialog(CoreInterface* core, const QString& inputDatasetName) :
    QDialog(nullptr),
    _targetAction(this, "Cluster set"),
    _nameAction(this, "Name"),
    _colorAction(this, "Color")
{
    setWindowTitle("Add cluster");
    setWindowIcon(hdps::Application::getIconFont("FontAwesome").getIcon("th-large"));
    
    auto layout = new QGridLayout();

    setLayout(layout);

    auto inputDataHierarchyItem = core->getDataHierarchyItem(inputDatasetName);

    QStringList clusterDatasetNames;

    for (auto child : inputDataHierarchyItem->getChildren()) {
        auto childDataHierarchyItem = core->getDataHierarchyItem(child);

        if (dynamic_cast<Clusters*>(&childDataHierarchyItem->getDataset()))
            clusterDatasetNames << child;
    }

    _targetAction.setOptions(clusterDatasetNames);

    const auto addWidgetAction = [this, layout](WidgetAction& widgetAction) -> void {
        const auto numRows = layout->rowCount();

        layout->addWidget(widgetAction.createLabelWidget(this), numRows, 0);
        layout->addWidget(widgetAction.createWidget(this), numRows, 1);
    };

    addWidgetAction(_targetAction);
    addWidgetAction(_nameAction);
    addWidgetAction(_colorAction);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("&Create");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("&Cancel");

    auto dialogButtonBoxLayout = new QHBoxLayout();

    dialogButtonBoxLayout->addStretch(1);
    dialogButtonBoxLayout->addWidget(dialogButtonBox);

    layout->addLayout(dialogButtonBoxLayout, layout->rowCount(), 0, 1, 2);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, [this, core, inputDataHierarchyItem]() {
        const auto targetClusterDataseName = _targetAction.getCurrentText();

        if (!targetClusterDataseName.isEmpty()) {
            auto& points            = inputDataHierarchyItem->getDataset<Points>();
            auto& selection         = dynamic_cast<Points&>(points.getSelection());
            auto& clusterDataset    = core->getDataHierarchyItem(targetClusterDataseName)->getDataset<Clusters>();

            Cluster cluster;

            cluster._name   = _nameAction.getString();
            cluster._color  = _colorAction.getColor();
            cluster.indices = selection.indices;

            clusterDataset.addCluster(cluster);

            accept();
        }

        reject();
    });

    connect(dialogButtonBox, &QDialogButtonBox::rejected, [this]() {
        reject();
    });
}
