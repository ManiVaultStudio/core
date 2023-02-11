#include "ProjectEditorDialog.h"
#include "ViewPlugin.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace hdps;
using namespace hdps::plugin;

ProjectEditorDialog::ProjectEditorDialog(QWidget* parent, ViewPlugin* viewPlugin) :
    QDialog(parent),
    _groupsAction(this, "Groups"),
    _actionHierarchyWidget(this, viewPlugin)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    //setWindowTitle(QString("Edit project (%1)").arg(_viewPlugin->text()));
    setMinimumSize(QSize(320, 240));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_actionHierarchyWidget);
    layout->addWidget(viewPlugin->getMayCloseAction().createWidget(this));
    layout->addWidget(viewPlugin->getMayFloatAction().createWidget(this));
    layout->addWidget(viewPlugin->getMayMoveAction().createWidget(this));
    layout->addWidget(viewPlugin->getLockingAction().getLockedAction().createWidget(this));
    layout->addWidget(viewPlugin->getGuiNameAction().createWidget(this));
    //layout->addWidget(_groupsAction.createWidget(this));

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ProjectEditorDialog::accept);
}
