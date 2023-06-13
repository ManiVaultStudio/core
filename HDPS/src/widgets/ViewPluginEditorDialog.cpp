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
    _actionsWidget(this, _actionsHierarchyModel),
    _settingsAction(this, "Settings")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setWindowTitle(QString("Edit (%1) settings").arg(viewPlugin->text()));
    setMinimumSize(QSize(640, 480));

    auto layout = new QVBoxLayout();
    
    layout->addWidget(&_actionsWidget);

    _settingsAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    _settingsAction.addAction(&viewPlugin->getLockingAction().getLockedAction());
    _settingsAction.addAction(&viewPlugin->getDockingOptionsAction());
    _settingsAction.addAction(&viewPlugin->getGuiNameAction());

    layout->addWidget(_settingsAction.createWidget(this));

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ViewPluginEditorDialog::accept);

    _actionsWidget.getFilterModel().getPublicRootOnlyAction().setChecked(false);
}

}