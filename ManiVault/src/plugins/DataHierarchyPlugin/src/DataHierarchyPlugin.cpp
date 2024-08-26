// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "DataHierarchyPlugin.h"

#include <Application.h>

Q_PLUGIN_METADATA(IID "studio.manivault.DataHierarchyPlugin")

using namespace mv;
using namespace mv::gui;

DataHierarchyPlugin::DataHierarchyPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataHierarchyWidget(nullptr)
{
    addShortcut({ QKeySequence(Qt::Key_R), "Selection", "Rectangle (default)" });
    addShortcut({ QKeySequence(Qt::Key_L), "Selection", "Lasso" });
    addShortcut({ QKeySequence(Qt::Key_B), "Selection", "Circular brush (mouse wheel adjusts the radius)" });
    addShortcut({ QKeySequence(Qt::SHIFT), "Selection", "Add to selection" });
    addShortcut({ QKeySequence(Qt::CTRL), "Selection", "Remove from selection" });

    addShortcut({ QKeySequence(Qt::Key_S), "Render", "Scatter mode (default)" });
    addShortcut({ QKeySequence(Qt::Key_D), "Render", "Density mode" });
    addShortcut({ QKeySequence(Qt::Key_C), "Render", "Contour mode" });

    addShortcut({ QKeySequence(Qt::ALT), "Navigation", "Pan (LMB down)" });
    addShortcut({ QKeySequence(Qt::ALT), "Navigation", "Zoom (mouse wheel)" });
    addShortcut({ QKeySequence(Qt::Key_O), "Navigation", "Original view" });
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

QUrl DataHierarchyPluginFactory::getRespositoryUrl() const
{
    return QUrl("https://github.com/ManiVaultStudio/core");
}

ViewPlugin* DataHierarchyPluginFactory::produce()
{
    return new DataHierarchyPlugin(this);
}
