// SPDX-License-Identifier: LGPL-3.0-or-later

#include "PointDataViewPlugin.h"

#include <ManiVaultVersion.h>

Q_PLUGIN_METADATA(IID "studio.manivault.PointDataViewPlugin")

PointDataViewPlugin::PointDataViewPlugin(const mv::plugin::PluginFactory* factory) :
    ViewPlugin(factory),
    _tileTask(this, "Enable tiled storage"),
    _widget(nullptr, &_tileTask)
{
    getGuiNameAction().setString("Points View");
    setProgressTask(&_tileTask);
    connect(&_widget, &PointDataViewWidget::tiledStorageEnabled, this, [this](const QString& datasetName) {
        addNotification(QString("Tiled storage enabled for %1").arg(datasetName));
    });
}

void PointDataViewPlugin::init()
{
    auto* layout = new QVBoxLayout();
    layout->setContentsMargins(6, 6, 6, 6);
    layout->addWidget(&_widget);
    getWidget().setLayout(layout);
}

PointDataViewPluginFactory::PointDataViewPluginFactory() :
    ViewPluginFactory(true)
{
    setIconByName("table");
    getPluginMetadata().setDescription("Inspect point data through tile-oriented access APIs");
    getPluginMetadata().setSummary("A developer view for exercising and visualizing dense point-data tile access.");
    getPluginMetadata().setCopyrightHolder({ "BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft)" });
    getPluginMetadata().setLicenseText("This plugin is distributed under the [LGPL v3.0](https://www.gnu.org/licenses/lgpl-3.0.en.html) license.");
}

QUrl PointDataViewPluginFactory::getRepositoryUrl() const
{
    return { "https://github.com/ManiVaultStudio/core" };
}

mv::plugin::ViewPlugin* PointDataViewPluginFactory::produce()
{
    return new PointDataViewPlugin(this);
}
