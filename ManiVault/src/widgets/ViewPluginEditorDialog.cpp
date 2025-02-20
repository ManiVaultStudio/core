// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ViewPluginEditorDialog.h"
#include "ViewPlugin.h"

#include <QVBoxLayout>
#include <QDialogButtonBox>

using namespace mv;
using namespace mv::plugin;
using namespace mv::util;

namespace mv::gui {

ViewPluginEditorDialog::ViewPluginEditorDialog(QWidget* parent, ViewPlugin* viewPlugin) :
    QDialog(parent),
    _groupsAction(this, "Groups"),
    _actionsListModel(this, viewPlugin),
    _actionsHierarchyModel(this, viewPlugin),
    _actionsWidget(this, _actionsHierarchyModel),
    _settingsAction(this, "Settings")
{
    setWindowTitle(QString("Edit (%1) settings").arg(viewPlugin->text()));

    _settingsAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    _settingsAction.addAction(&viewPlugin->getLockingAction().getLockedAction());
    _settingsAction.addAction(&viewPlugin->getDockingOptionsAction());
    _settingsAction.addAction(&viewPlugin->getGuiNameAction());

    init();
}

ViewPluginEditorDialog::ViewPluginEditorDialog(QWidget* parent, WidgetAction* rootAction) :
    QDialog(parent),
    _groupsAction(this, "Groups"),
    _actionsListModel(this, rootAction),
    _actionsHierarchyModel(this, rootAction),
    _actionsWidget(this, _actionsHierarchyModel),
    _settingsAction(this, "Settings")
{
    setWindowTitle(QString("Edit plugin settings"));

    init();
}

void ViewPluginEditorDialog::init()
{
    setWindowIcon(StyledIcon("cog"));
    setMinimumSize(QSize(640, 480));

    auto layout = new QVBoxLayout();

    layout->addWidget(&_actionsWidget);

    _settingsAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    layout->addWidget(_settingsAction.createWidget(this));

    setLayout(layout);

    auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    layout->addWidget(dialogButtonBox);

    connect(dialogButtonBox, &QDialogButtonBox::accepted, this, &ViewPluginEditorDialog::accept);

    _actionsWidget.getFilterModel().getPublicRootOnlyAction().setChecked(false);
}

}