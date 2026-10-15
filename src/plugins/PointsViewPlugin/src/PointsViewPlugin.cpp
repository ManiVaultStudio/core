// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsViewPlugin.h"

#include <CoreInterface.h>
#include <DatasetsMimeData.h>
#include <ManiVaultVersion.h>
#include <PointData.h>
#include <widgets/DropWidget.h>

#include <QMimeData>

namespace
{

mv::Dataset<Points> getPointsDataset(const mv::Dataset<mv::DatasetImpl>& dataset)
{
    const auto* points = dynamic_cast<const Points*>(dataset.getDataset());

    return points != nullptr ? mv::Dataset<Points>(const_cast<Points*>(points)) : mv::Dataset<Points>();
}

}

Q_PLUGIN_METADATA(IID "studio.manivault.PointsViewPlugin")

PointsViewPlugin::PointsViewPlugin(const mv::plugin::PluginFactory* factory) :
    ViewPlugin(factory),
    _datasetSourceGroupAction(this, "Dataset selection"),
    _useHierarchySelectionAction(&_datasetSourceGroupAction, "Use hierarchy selection", true),
    _datasetPickerAction(&_datasetSourceGroupAction, "Dataset"),
    _widget(nullptr, false)
{
    getGuiNameAction().setString("Points View");
    _datasetSourceGroupAction.setShowLabels(true);
    _datasetSourceGroupAction.setToolTip("Select a point dataset directly, follow the current data-hierarchy selection, or drop a point dataset on the view.");
    _useHierarchySelectionAction.setDefaultWidgetFlags(mv::gui::ToggleAction::CheckBox);
    _useHierarchySelectionAction.setToolTip("Follow the currently selected Points dataset in the data hierarchy");
    _datasetPickerAction.setToolTip("Select a Points dataset to inspect");
    _datasetPickerAction.setFilterFunction([](const mv::Dataset<mv::DatasetImpl>& dataset) {
        return dataset.isValid() && dynamic_cast<const Points*>(dataset.getDataset()) != nullptr;
    });
    _datasetSourceGroupAction.addAction(&_datasetPickerAction);
    _datasetSourceGroupAction.addAction(&_useHierarchySelectionAction, mv::gui::ToggleAction::CheckBox);

    connect(&_useHierarchySelectionAction, &mv::gui::ToggleAction::toggled, this, &PointsViewPlugin::updateDatasetSelection);
    connect(&_datasetPickerAction, &mv::gui::DatasetPickerAction::currentDatasetChanged, this, [this](const mv::Dataset<mv::DatasetImpl>& dataset) {
        if (!_useHierarchySelectionAction.isChecked())
            setPickedPoints(getPointsDataset(dataset));
    });
    connect(&mv::dataHierarchy(), &mv::AbstractDataHierarchyManager::selectedItemsChanged, this, &PointsViewPlugin::refreshSelection);

    updateDatasetSelection();
}

void PointsViewPlugin::updateDatasetSelection()
{
    const auto followsHierarchy = _useHierarchySelectionAction.isChecked();

    _datasetPickerAction.setEnabled(!followsHierarchy);

    if (followsHierarchy)
        refreshSelection();
    else
        setPickedPoints(getPointsDataset(_datasetPickerAction.getCurrentDataset()));
}

void PointsViewPlugin::refreshSelection()
{
    if (!_useHierarchySelectionAction.isChecked())
        return;

    const auto selectedItems = mv::dataHierarchy().getSelectedItems();
    const auto points = selectedItems.size() == 1 ? getPointsDataset(selectedItems.first()->getDataset()) : mv::Dataset<Points>();

    _widget.setPoints(points);
}

void PointsViewPlugin::setPickedPoints(const mv::Dataset<Points>& points)
{
    _widget.setPoints(points);
}

void PointsViewPlugin::init()
{
    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(6);
    layout->addWidget(_datasetSourceGroupAction.createWidget(&getWidget(), mv::gui::GroupAction::Horizontal | mv::gui::GroupAction::WithMargins));
    layout->addWidget(&_widget);
    getWidget().setLayout(layout);
    getWidget().setAcceptDrops(true);

    auto* dropWidget = new mv::gui::DropWidget(&getWidget());
    dropWidget->initialize([this, dropWidget](const QMimeData* mimeData) {
        const auto* datasetsMimeData = dynamic_cast<const mv::DatasetsMimeData*>(mimeData);

        if (!datasetsMimeData)
            return mv::gui::DropWidget::DropRegions();

        const auto datasets = datasetsMimeData->getDatasets();

        if (datasets.size() != 1)
            return mv::gui::DropWidget::DropRegions();

        const auto droppedPoints = getPointsDataset(datasets.first());

        if (!droppedPoints.isValid())
            return mv::gui::DropWidget::DropRegions();

        return mv::gui::DropWidget::DropRegions {
            new mv::gui::DropWidget::DropRegion(dropWidget, "Inspect point dataset", "Drop to display this dataset", "database", true, [this, droppedPoints] {
                _datasetPickerAction.setCurrentDataset(mv::Dataset<mv::DatasetImpl>(droppedPoints));
                _useHierarchySelectionAction.setChecked(false);
            })
        };

    });
    dropWidget->setGeometry(getWidget().rect());
}

PointsViewPluginFactory::PointsViewPluginFactory() :
    ViewPluginFactory(false)
{
    setIconByName("table");
    getPluginMetadata().setDescription("Inspect point data through tile-oriented access APIs");
    getPluginMetadata().setSummary("A developer view for exercising and visualizing dense point-data tile access.");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

QUrl PointsViewPluginFactory::getRepositoryUrl() const
{
    return { "https://github.com/ManiVaultStudio/core" };
}

mv::plugin::ViewPlugin* PointsViewPluginFactory::produce()
{
    return new PointsViewPlugin(this);
}
