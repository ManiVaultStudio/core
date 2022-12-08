#include "LoadedPluginsDialog.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QPushButton>

using namespace hdps;
using namespace hdps::gui;

LoadedPluginsDialog::LoadedPluginsDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _model(this),
    _filterModel(this),
    _hierarchyWidget(this, "Plugin", _model, &_filterModel)
{
    const auto pluginIcon = Application::getIconFont("FontAwesome").getIcon("plug");

    setWindowIcon(pluginIcon);
    setModal(true);
    setWindowTitle("Loaded plugins");
    
    auto layout = new QVBoxLayout();

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(pluginIcon);
}
