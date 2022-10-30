#include "ActionHierarchyDialog.h"

#include <Application.h>

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace hdps;

ActionHierarchyDialog::ActionHierarchyDialog(QWidget* parent, WidgetAction* action) :
    QDialog(parent),
    _action(action),
    _actionHierarchyWidget(this)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle(QString("Edit actions (%1)").arg(action->text()));
    setMinimumSize(QSize(320, 240));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_actionHierarchyWidget);

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ActionHierarchyDialog::accept);
}
