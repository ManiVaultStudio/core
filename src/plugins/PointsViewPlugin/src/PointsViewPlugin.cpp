// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointsViewPlugin.h"

#include <CoreInterface.h>
#include <ManiVaultVersion.h>
#include <PointData.h>

Q_PLUGIN_METADATA(IID "studio.manivault.PointsViewPlugin")

PointsViewPlugin::PointsViewPlugin(const mv::plugin::PluginFactory* factory) :
    ViewPlugin(factory),
    _widget(nullptr)
{
    getGuiNameAction().setString("Points View");
    connect(&mv::dataHierarchy(), &mv::AbstractDataHierarchyManager::selectedItemsChanged, this, &PointsViewPlugin::refreshSelection);

    refreshSelection();
}

void PointsViewPlugin::refreshSelection()
{
    const auto selectedItems = mv::dataHierarchy().getSelectedItems();
    const auto points = selectedItems.size() == 1 ? selectedItems.first()->getDataset<Points>() : mv::Dataset<Points>();

    _widget.setPoints(points);
}

void PointsViewPlugin::init()
{
    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(6, 6, 6, 6);
    layout->addWidget(&_widget);
    getWidget().setLayout(layout);
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
