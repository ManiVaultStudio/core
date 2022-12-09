#include "PluginManagerDialog.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QPushButton>

using namespace hdps;
using namespace hdps::gui;

#ifdef _DEBUG
    #define PLUGIN_MANAGER_DIALOG_VERBOSE
#endif

PluginManagerDialog::PluginManagerDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Plugin", _model, &_filterModel)
{
    const auto pluginIcon = Application::getIconFont("FontAwesome").getIcon("plug");

    setWindowIcon(pluginIcon);
    setModal(true);
    setWindowTitle("Plugin manager");
    
    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(pluginIcon);
    _hierarchyWidget.getTreeView().setRootIsDecorated(true);
}
