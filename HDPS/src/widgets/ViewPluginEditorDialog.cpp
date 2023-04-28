#include "ViewPluginEditorDialog.h"
#include "ViewPlugin.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace hdps;
using namespace hdps::plugin;

namespace hdps::gui {

ViewPluginEditorDialog::ViewPluginEditorDialog(QWidget* parent, ViewPlugin* viewPlugin) :
    QDialog(parent),
    _groupsAction(this, "Groups"),
    _actionsListModel(this, viewPlugin),
    _actionsHierarchyModel(this, viewPlugin),
    _actionsListWidget(this, _actionsListModel),
    _actionsHierarchyWidget(this, _actionsHierarchyModel),
    _settingsAction(this, "Settings")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle(QString("Edit (%1) settings").arg(viewPlugin->text()));
    setMinimumSize(QSize(320, 240));

    auto layout = new QVBoxLayout();
    
    auto actionsWidgetsLayout = new QHBoxLayout();

    actionsWidgetsLayout->addWidget(&_actionsListWidget);
    actionsWidgetsLayout->addWidget(&_actionsHierarchyWidget);
    
    layout->addLayout(actionsWidgetsLayout);

    _settingsAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    _settingsAction << viewPlugin->getLockingAction().getLockedAction();
    _settingsAction << viewPlugin->getDockingOptionsAction();
    _settingsAction << viewPlugin->getGuiNameAction();

    layout->addWidget(_settingsAction.createWidget(this));

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ViewPluginEditorDialog::accept);
}

}