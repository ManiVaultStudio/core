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
    const std::string longDescriptionMarkdown = R"(
This view plugin displays all the loaded data in a hierarchical way.
With this plugin, you can **import**, **export**, **analyze**, **transform**, **view** and **interact** with data.
View examples below to get started.
<details>
  <summary>Import data</summary>

- Click **RMB** in an empty area of the data hierarchy to show the context menu
- Click **LMB** on **Import**, this will show all data import plugins (this menu is not available when there are no compatible exporter plugins)
- Click **LMB** to start the importer of choice
)"
	+ util::embedGifFromResource(":/animation/ImportDataScaled.gif").toStdString() +
R"(
</details>
<details>
  <summary>Export data</summary>

- Select one (or more) datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB** to show the context menu
- Click **LMB** on **Export**, this will show all data export plugins (this menu is not available when there are no compatible exporter plugins)
- Click **LMB** the exporter of choice
)"
+ util::embedGifFromResource(":/animation/ImportDataScaled.gif").toStdString() +
R"(
</details>
<details>
  <summary>Analyze data</summary>

- Select one (or more) datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB** to show the context menu
- Click **LMB** on **Analyze**, this will show all compatible analysis plugins (this menu is not available when there are no compatible analysis plugins)
- Click **LMB** on the analysis of choice
)"
+ util::embedGifFromResource(":/animation/AnalyzeDataScaled.gif").toStdString() +
R"(
</details>
<details>
  <summary>Transform data</summary>

- Select one (or more) datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB** to show the context menu
- Click **LMB** on **Transform**, this will show all compatible analysis plugins (this menu is not available when there are no compatible transform plugins)
- Click **LMB** the transform plugin of choice
)"
+ util::embedGifFromResource(":/animation/TransformDataScaled.gif").toStdString() +
R"(
</details>
<details>
  <summary>View data</summary>

### View single dataset
- Select a single dataset
- Click **RMB** to show the context menu
- Click **LMB** on **View**, this will show all compatible view plugins (this menu is not available when there are no compatible view plugins)
- Click **LMB** the view plugin of choice
)"
+ util::embedGifFromResource(":/animation/ViewDataContextMenuScaled.gif").toStdString() +
R"(

### View multiple datasets
- Select multiple datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB** to show the context menu
- Click **LMB** on **View**, this will show all compatible view plugins (this menu is not available when there are no compatible view plugins)
- Click **LMB** the view plugin of choice
)"
+ util::embedGifFromResource(":/animation/ViewDataContextMenuMultipleScaled.gif").toStdString() +
R"(

### View dataset with drag and drop
- Select dataset(s)
- Hold **LMB** down and move to view plugin in which you want to show the data
- Drop the data by releasing the **LMB**
)"
+ util::embedGifFromResource(":/animation/ViewDataDragAndDropScaled.gif").toStdString() +
R"(
</details>
)";

    getLearningCenterAction().setLongDescriptionMarkdown(QString::fromStdString(longDescriptionMarkdown));

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
