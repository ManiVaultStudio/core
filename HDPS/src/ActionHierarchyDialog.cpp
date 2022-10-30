#include "ActionHierarchyDialog.h"

#include <Application.h>

#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace hdps;

ActionHierarchyDialog::ActionHierarchyDialog(QWidget* parent, WidgetAction* action) :
    QDialog(parent),
    _action(action)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle(QString("Edit %1 actions").arg(action->text()));

    auto layout = new QGridLayout();

    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 2, 0, 1, 2);

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(dialogButtonBox, 3, 0, 1, 2);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ActionHierarchyDialog::accept);
}
