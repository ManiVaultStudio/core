// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyPlugin.h"

#include <Application.h>

#include "util/Miscellaneous.h"

Q_PLUGIN_METADATA(IID "studio.manivault.DataHierarchyPlugin")using namespace mv;
using namespace mv::gui;

void listResources(const QString& resourcePath) {
    // Create a QDir object for the resource path
    QDir resourceDir(resourcePath);

    // Check if the directory exists
    if (!resourceDir.exists()) {
        qDebug() << "Resource directory does not exist:" << resourcePath;
        return;
    }

    // Get the list of all files and directories in the resource path
    QFileInfoList resourceList = resourceDir.entryInfoList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot);

    // Print out each resource
    for (const QFileInfo& fileInfo : resourceList) {
        if (fileInfo.isDir()) {
            // Recursively list resources in subdirectories
            listResources(fileInfo.absoluteFilePath());
        }
        else {
            qDebug() << "Resource found:" << fileInfo.absoluteFilePath();
        }
    }
}

DataHierarchyPlugin::DataHierarchyPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataHierarchyWidget(nullptr)
{
    auto& shortcuts = getShortcuts();

    shortcuts.add({ QKeySequence(Qt::CTRL | Qt::Key_Minus), "Expand/collapse", "Collapse all" });
    shortcuts.add({ QKeySequence(Qt::CTRL | Qt::Key_Plus), "Expand/collapse", "Expand all" });

    getLearningCenterAction().setPluginTitle("Data hierarchy view");

    getLearningCenterAction().setShortDescription("Hierarchical overview of all loaded data");
    getLearningCenterAction().setLongDescriptionMarkdown(QString(R"(
This view plugin displays all the loaded data in a hierarchical way.
With this plugin, you can **import**, **export**, **analyze**, **transform**, and **view** data.

<details>
  <summary>Import data</summary>

- **RMB** in an empty area of the data hierarchy to show the context menu
- **LMB** on **Import**, this will show all data import plugins (this menu is not available when there are no compatible exporter plugins)
- **LMB** to start the importer of choice
%1
</details>

<details>
  <summary>Export data</summary>

- Select one (or more) items
- **RMB** to show the context menu
- **LMB** on **Export**, this will show all data export plugins (this menu is not available when there are no compatible exporter plugins)
- **LMB** the exporter of choice
%2
</details>

<details>
  <summary>Analyze data</summary>

- Select one (or more) items
- **RMB** to show the context menu
- **LMB** on **Analyze**, this will show all compatible analysis plugins (this menu is not available when there are no compatible analysis plugins)
- **LMB** on the analysis of choice
</details>

<details>
  <summary>Transform data</summary>

- Select one (or more) items"
- **RMB** to show the context menu
- **LMB** on **Transform**, this will show all compatible analysis plugins (this menu is not available when there are no compatible transform plugins)
- **LMB** the transform plugin of choice
</details>

<details>
  <summary>View data</summary>

- Select one (or more) items"
- **RMB** to show the context menu
- **LMB** on **View**, this will show all compatible view plugins (this menu is not available when there are no compatible view plugins)
- **LMB** the view plugin of choice
</details>
)").arg(util::embedGifFromResource(":/animation/ImportDataScaled.gif"), util::embedGifFromResource(":/animation/ExportDataScaled.gif")));

	getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
}

void DataHierarchyPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_dataHierarchyWidget);

    getWidget().setLayout(layout);
}

DataHierarchyPluginFactory::DataHierarchyPluginFactory() :
    ViewPluginFactory(true)
{
}

QIcon DataHierarchyPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sitemap", color);
}

QUrl DataHierarchyPluginFactory::getReadmeMarkdownUrl() const
{
#ifdef ON_LEARNING_CENTER_FEATURE_BRANCH
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/feature/learning_center/ManiVault/src/plugins/DataHierarchyPlugin/README.md");
#else
    return QUrl("https://raw.githubusercontent.com/ManiVaultStudio/core/master/ManiVault/src/plugins/DataHierarchyPlugin/README.md");
#endif
}

QUrl DataHierarchyPluginFactory::getRepositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core/tree/master/ManiVault/src/plugins/DataHierarchyPlugin");
}

ViewPlugin* DataHierarchyPluginFactory::produce()
{
    return new DataHierarchyPlugin(this);
}
