#include "ActionHierarchyDialog.h"

#include <Application.h>

#include <QGridLayout>
#include <QDialogButtonBox>
#include <QPushButton>

using namespace hdps;

ActionHierarchyDialog::ActionHierarchyDialog(QWidget* parent) :
    QDialog(parent)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle("Edit actions");

    auto layout = new QGridLayout();

    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 2, 0, 1, 2);

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Create");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

    layout->addWidget(dialogButtonBox, 3, 0, 1, 2);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ActionHierarchyDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &ActionHierarchyDialog::reject);
}
