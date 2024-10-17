// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyPlugin.h"

#include <Application.h>

#include "util/Miscellaneous.h"

Q_PLUGIN_METADATA(IID "studio.manivault.DataHierarchyPlugin")

using namespace mv;
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
It also allows for advanced **search and filtering** of datasets and
other manipulations of the data hierarchy such as toggling **columns**.
View categorized examples below to get started.
<details>
  <summary id='ImportData'>Import data</summary>

- Click **RMB** in an empty area of the data hierarchy to show the context menu
- Click **LMB** on **Import**, this will show all data import plugins (this menu is not available when there are no compatible exporter plugins)
- Click **LMB** to start the importer of choice
)"
	+ util::embedGifFromResource(":/animation/ImportDatasetScaled.gif").toStdString() +
R"(
</details>
<details>
  <summary id='ExportData'>Export data</summary>

- Select one (or more) datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB** to show the context menu
- Click **LMB** on **Export**, this will show all data export plugins (this menu is not available when there are no compatible exporter plugins)
- Click **LMB** the exporter of choice
)"
+ util::embedGifFromResource(":/animation/ExportDatasetScaled.gif").toStdString() +
R"(
</details>
<details>
  <summary id='AnalyzeData'>Analyze data</summary>

- Select one (or more) datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB** to show the context menu
- Click **LMB** on **Analyze**, this will show all compatible analysis plugins (this menu is not available when there are no compatible analysis plugins)
- Click **LMB** on the analysis of choice
)"
+ util::embedGifFromResource(":/animation/AnalyzeDatasetScaled.gif").toStdString() +
R"(
</details>
<details>
  <summary id='TransformData'>Transform data</summary>

- Select one (or more) datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB** to show the context menu
- Click **LMB** on **Transform**, this will show all compatible analysis plugins (this menu is not available when there are no compatible transform plugins)
- Click **LMB** the transform plugin of choice
)"
+ util::embedGifFromResource(":/animation/TransformDatasetScaled.gif").toStdString() +
R"(
</details>
<details id='ViewData'>
  <summary>View data</summary>

### View single dataset
- Select a single dataset
- Click **RMB** to show the context menu
- Click **LMB** on **View**, this will show all compatible view plugins (this menu is not available when there are no compatible view plugins)
- Click **LMB** the view plugin of choice
)"
+ util::embedGifFromResource(":/animation/ViewDatasetContextMenuScaled.gif").toStdString() +
R"(

### View multiple datasets
- Select multiple datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB** to show the context menu
- Click **LMB** on **View**, this will show all compatible view plugins (this menu is not available when there are no compatible view plugins)
- Click **LMB** the view plugin of choice
)"
+ util::embedGifFromResource(":/animation/ViewDatasetContextMenuMultipleScaled.gif").toStdString() +
R"(

### View dataset with drag and drop
- Select dataset(s)
- Hold **LMB** down and move to view plugin in which you want to show the data
- Drop the data by releasing the **LMB**
)"
+ util::embedGifFromResource(":/animation/ViewDatasetDragAndDropScaled.gif").toStdString() +
R"(
</details>
<details id='Interact'>
  <summary>Interact with the data hierarchy</summary>

### Edit data properties
- Select a single dataset
- Edit the data properties in the **Data properties** view plugin
)"
+ util::embedGifFromResource(":/animation/EditDatasetPropertiesScaled.gif").toStdString() +
R"(

### Search and filter datasets
<a id="target-section"></a>
)"
+ util::embedGifFromResource(":/animation/SearchAndFilterDatasetsScaled.gif").toStdString() +
R"(

### Columns
)"
+ util::embedGifFromResource(":/animation/ColumnsScaled.gif").toStdString() +
R"(

### Remove dataset (simple)
- Select one (or more) datasets (hold **CTRL** to add a dataset and **SHIFT** to select a range of datasets)
- Click **RMB**
- Click **LMB** on **Remove**
)"
+ util::embedGifFromResource(":/animation/RemoveDatasetSimpleScaled.gif").toStdString() +
R"(

### Hide dataset
- Select a dataset
- Click **RMB**
- Click **LMB** on **Hide**
- Click **LMB** on **Selected**
)"
+ util::embedGifFromResource(":/animation/HideSelectedDatasetScaled.gif").toStdString() +
R"(

### Unhide dataset
- Select a dataset
- Click **RMB**
- Click **LMB** on **Unhide**
- Click **LMB** on **Selected**
)"
+ util::embedGifFromResource(":/animation/UnhideDatasetScaled.gif").toStdString() +
R"(

### Unhide all datasets
- Click **RMB** in empty area
- Click **LMB** on **Unhide**
- Click **LMB** on **All**
)"
+ util::embedGifFromResource(":/animation/UnhideAllDatasetsScaled.gif").toStdString() +
R"(

### Lock selected dataset(s)
- Select a dataset
- Click **RMB**
- Click **LMB** on **Lock**
- Click **LMB** on **Selected**
)"
+ util::embedGifFromResource(":/animation/LockDatasetScaled.gif").toStdString() +
R"(

### Unlock dataset
- Select a dataset
- Click **RMB**
- Click **LMB** on **Unlock**
- Click **LMB** on **Selected**
)"
+ util::embedGifFromResource(":/animation/UnlockDatasetScaled.gif").toStdString() +
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
