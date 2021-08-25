#include "CreateClusterDialog.h"

#include "CoreInterface.h"
#include "Application.h"
#include "PointData.h"
#include "ClusterData.h"

#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QRandomGenerator>

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
    _targetAction.setCurrentText(clusterDatasetNames.first());
    _targetAction.setEnabled(clusterDatasetNames.count() > 1);

    const auto randomHue        = QRandomGenerator::global()->bounded(360);
    const auto randomSaturation = QRandomGenerator::global()->bounded(150, 255);
    const auto randomLightness  = QRandomGenerator::global()->bounded(100, 200);

    _colorAction.setColor(QColor::fromHsl(randomHue, randomSaturation, randomLightness));

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
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("C&ancel");

    auto dialogButtonBoxLayout = new QHBoxLayout();

    dialogButtonBoxLayout->addStretch(1);
    dialogButtonBoxLayout->addWidget(dialogButtonBox);

    layout->addLayout(dialogButtonBoxLayout, layout->rowCount(), 0, 1, 2);

    const auto updateCreateButton = [this, dialogButtonBox]() -> void {
        dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!_nameAction.getString().isEmpty());
    };

    const auto updateNameAction = [this, core]() -> void {
        if (!_nameAction.getString().isEmpty())
            return;

        auto& clusterDataset = core->getDataHierarchyItem(_targetAction.getCurrentText())->getDataset<Clusters>();

        _nameAction.setString(QString("cluster_%1").arg(clusterDataset.getClusters().size() + 1));
    };

    connect(&_targetAction, &OptionAction::currentTextChanged, this, [this, updateNameAction](const QString& currentText) {
        updateNameAction();
    });

    connect(&_nameAction, &StringAction::stringChanged, this, [this, updateCreateButton](const QString& string) {
        updateCreateButton();
    });

    updateNameAction();
    updateCreateButton();

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

            core->notifyDataChanged(targetClusterDataseName);

            accept();
        }

        reject();
    });

    connect(dialogButtonBox, &QDialogButtonBox::rejected, [this]() {
        reject();
    });
}
