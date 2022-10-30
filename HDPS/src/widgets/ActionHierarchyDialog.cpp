#include "ActionHierarchyDialog.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace hdps;

ActionHierarchyDialog::ActionHierarchyDialog(QWidget* parent, WidgetAction* rootAction) :
    QDialog(parent),
    _action(rootAction),
    _actionHierarchyWidget(this, rootAction)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle(QString("Edit actions (%1)").arg(rootAction->text()));
    setMinimumSize(QSize(320, 240));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_actionHierarchyWidget);

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ActionHierarchyDialog::accept);
}
