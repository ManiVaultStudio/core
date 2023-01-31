#include "GroupDataDialog.h"

#include <Application.h>
#include <Set.h>

#include <QGridLayout>
#include <QDialogButtonBox>

using namespace hdps;
using namespace hdps::gui;

GroupDataDialog::GroupDataDialog(QWidget* parent, const hdps::Datasets& datasets) :
    QDialog(parent),
    _datasets(datasets),
    _groupNameAction(this, "Group name"),
    _showDialogAction(this, "Show dialog next time", true, true)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle(QString("Group %1 datasets").arg(QString::number(_datasets.count())));
    
    QStringList datasetNames;

    for (const auto& dataset : datasets)
        datasetNames << dataset->getGuiName();

    _groupNameAction.setString(QString("%1").arg(datasetNames.join("+")));

    auto layout = new QGridLayout();

    auto groupNameLabelWidget   = _groupNameAction.createLabelWidget(this);
    auto groupNameWidget        = _groupNameAction.createWidget(this);
    auto showNextTimeWidget     = _showDialogAction.createWidget(this);

    layout->addWidget(groupNameLabelWidget, 0, 0);
    layout->addWidget(groupNameWidget, 0, 1);
    layout->addWidget(showNextTimeWidget, 1, 1);

    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding), 2, 0, 1, 2);

    setLayout(layout);

    connect(&_showDialogAction, &ToggleAction::toggled, this, [this](bool toggled) {
        Application::current()->setSetting("AskForGroupName", toggled);
    });

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Create");
    dialogButtonBox->button(QDialogButtonBox::Cancel)->setText("Cancel");

    layout->addWidget(dialogButtonBox, 3, 0, 1, 2);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &GroupDataDialog::accept);
    connect(dialogButtonBox, &QDialogButtonBox::rejected, this, &GroupDataDialog::reject);

    connect(&_groupNameAction, &StringAction::stringChanged, this, [this, dialogButtonBox](const QString& string) {
        dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!string.isEmpty());
    });
}

QString GroupDataDialog::getGroupName() const
{
    return _groupNameAction.getString();
}
