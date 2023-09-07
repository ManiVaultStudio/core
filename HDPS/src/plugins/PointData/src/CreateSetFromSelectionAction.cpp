// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "CreateSetFromSelectionAction.h"

#include <Application.h>

#include <QHBoxLayout>

using namespace hdps;

CreateSetFromSelectionAction::CreateSetFromSelectionAction(QObject* parent, const Dataset<Points>& points) :
    WidgetAction(parent, "Create Set From Selection"),
    _points(points),
    _nameAction(this, "Name"),
    _createAction(this, "Create")
{
    setText("Create set from selection");
    setToolTip("Create a new dataset from the current selection");

    _nameAction.setToolTip("Name of the created set");
    _createAction.setToolTip("Create the set");

    _nameAction.setPlaceHolderString("Enter set name here...");

    connect(&_createAction, &TriggerAction::triggered, this, [this]() -> void {
        auto points = _points->isDerivedData() ? _points->getSourceDataset<Points>() : _points;

        if (points->isProxy()) {

            Datasets subsets;

            for (auto proxyMember : _points->getProxyMembers())
                subsets << Application::core()->createSubsetFromSelection(proxyMember->getSelection(), proxyMember, "Subset", proxyMember);

            Application::core()->groupDatasets(subsets, _nameAction.getString());
        }
        else {
            points->createSubsetFromSelection(_nameAction.getString());
        }

        _nameAction.setString("");
    });

    const auto updateCreateAction = [this]() -> void {
        _createAction.setEnabled(!_nameAction.getString().isEmpty());
    };

    connect(&_nameAction, &StringAction::stringChanged, this, updateCreateAction);

    updateCreateAction();
}

CreateSetFromSelectionAction::Widget::Widget(QWidget* parent, CreateSetFromSelectionAction* createSetFromSelectionAction) :
    WidgetActionWidget(parent, createSetFromSelectionAction)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);

    layout->addWidget(createSetFromSelectionAction->getNameAction().createWidget(this), 1);
    layout->addWidget(createSetFromSelectionAction->getCreateAction().createWidget(this));

    setLayout(layout);
}