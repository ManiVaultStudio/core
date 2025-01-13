// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyPlugin.h"

#include <Application.h>

#include <util/Miscellaneous.h>
#include <util/LearningCenterVideo.h>

Q_PLUGIN_METADATA(IID "studio.manivault.DataHierarchyPlugin")using namespace mv;
using namespace mv::gui;
using namespace mv::util;

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

    getLearningCenterAction().setDescription("Hierarchical overview of all loaded data");
	getLearningCenterAction().addVideos(QStringList({ "Practitioner", "Developer" }));
	getLearningCenterAction().addTutorials(QStringList({ "GettingStarted", "DataHierarchyPlugin" }));
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

void DataHierarchyPluginFactory::initialize()
{
	ViewPluginFactory::initialize();
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
