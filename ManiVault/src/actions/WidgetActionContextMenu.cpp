// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionContextMenu.h"
#include "WidgetAction.h"

#include "models/ActionsListModel.h"
#include "models/ActionsFilterModel.h"

#include "Application.h"
#include "CoreInterface.h"

#include <QDebug>

using namespace mv::util;

namespace mv::gui {

WidgetActionContextMenu::WidgetActionContextMenu(QWidget* parent, WidgetActions actions) :
    QMenu(parent),
    _actions(actions),
    _publishAction(this, "Publish..."),
    _connectAction(this, "Connect..."),
    _disconnectAction(this, "Disconnect..."),
    _disconnectAllAction(this, "Disconnect all..."),
    _removeAction(this, "Remove..."),
    _editAction(this, "Edit...")
{
    Q_ASSERT(actions.count() != 0);

    _publishAction.setIconByName("cloud-upload-alt");
    _connectAction.setIconByName("crosshairs");
    _disconnectAction.setIconByName("unlink");
    _disconnectAllAction.setIconByName("unlink");
    _removeAction.setIconByName("trash");
    _editAction.setIconByName("edit");

    auto firstAction = _actions.first();

    auto allPrivate = true;
    auto allPublic  = true;

    for (auto action : actions) {
        if (action->isPublic())
            allPrivate = false;

        if (action->isPrivate())
            allPublic = false;
    }

    addAction(&_publishAction);
    
    addSeparator();

    addAction(&_connectAction);
    addAction(&_disconnectAction);
    addAction(&_disconnectAllAction);

    addSeparator();

    addAction(&_removeAction);

    addSeparator();

    addAction(&_editAction);

    _connectAction.setVisible(false);
    _disconnectAllAction.setVisible(allPublic);
    _editAction.setVisible(!_actions.isEmpty());
    _removeAction.setVisible(allPublic && !_actions.isEmpty());

    if (allPrivate) {
        _removeAction.setVisible(false);

        if (_actions.count() == 1) {
            _publishAction.setEnabled(firstAction->isPublished() ? false : firstAction->mayPublish(WidgetAction::ConnectionContextFlag::Gui));
            _disconnectAction.setEnabled(firstAction->isConnected() ? firstAction->mayDisconnect(WidgetAction::ConnectionContextFlag::Gui) : false);

            if (firstAction->isConnected())
                _disconnectAction.setText(QString("Disconnect from: %1").arg(firstAction->getPublicAction()->text()));

            _publishAction.setText("Publish...");

            ActionsFilterModel actionsFilterModel(this);

            actionsFilterModel.setSourceModel(&mv::actions().getActionsListModel());
            actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Public" });
            actionsFilterModel.getTypeFilterAction().setString(firstAction->getTypeString());

            const auto numberOfRows = actionsFilterModel.rowCount();

            auto connectMenu = new QMenu("Connect to:");

            connectMenu->setIcon(StyledIcon("link"));

            for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
                auto publicAction = actionsFilterModel.getAction(rowIndex);

                if (!publicAction)
                    continue;

                if (publicAction == firstAction->getPublicAction())
                    continue;

                if (!firstAction->mayConnectToPublicAction(publicAction))
                    continue;

                auto connectAction = new QAction(publicAction->getLocation());

                connectAction->setToolTip("Connect " + firstAction->text() + " to " + publicAction->text());

                connect(connectAction, &QAction::triggered, this, [this, firstAction, publicAction]() -> void {
                    mv::actions().connectPrivateActionToPublicAction(firstAction, publicAction, true);
                });

                connectMenu->addAction(connectAction);
            }

            connectMenu->setEnabled(connectMenu->actions().isEmpty() ? false : firstAction->mayConnect(WidgetAction::ConnectionContextFlag::Gui));

            insertMenu(&_disconnectAction, connectMenu);
        }
        else {
            _publishAction.setVisible(false);
            _disconnectAction.setVisible(true);
        }
    }

    if (allPublic) {
        _publishAction.setVisible(false);
        _connectAction.setVisible(false);
        _disconnectAction.setVisible(false);

        WidgetActions publicActions;

        for (auto action : actions)
            if (action->isPublic() && action->isRoot())
                publicActions << action;

        _removeAction.setVisible(publicActions.count() >= 1);

        if (_actions.count() == 1)
            _removeAction.setText(QString("Remove %1").arg(firstAction->text()));
        else
            _removeAction.setText(QString("Remove %1 shared parameters").arg(QString::number(_actions.count())));

        connect(&_removeAction, &TriggerAction::triggered, this, [this, publicActions]() -> void {
            auto removePublicActions = true;

            if (mv::settings().getParametersSettings().getConfirmRemoveSharedParameterAction().isChecked()) {
                ConfirmRemovePublicActionDialog confirmRemovePublicActionDialog(this, publicActions);

                if (confirmRemovePublicActionDialog.exec() == QDialog::Rejected)
                    removePublicActions = false;
            }

            if (removePublicActions) {
                for (auto publicAction : publicActions) {
                    for (auto connectedAction : publicAction->getConnectedActions())
                        mv::actions().disconnectPrivateActionFromPublicAction(connectedAction, true);

                    delete publicAction;
                }
            }
        });
    }
    
    WidgetActions privateActions;

    for (auto action : actions)
        if (action->isPrivate())
            privateActions << action;

    WidgetActions connectablePrivateActions;

    for (auto privateAction : privateActions)
        if (privateAction->mayConnect(WidgetAction::Gui))
            connectablePrivateActions << privateAction;

    _connectAction.setEnabled(!connectablePrivateActions.isEmpty());
    
    WidgetActions disconnectablePrivateActions;

    for (auto privateAction : privateActions)
        if (privateAction->isConnected() && privateAction->mayDisconnect(WidgetAction::Gui))
            disconnectablePrivateActions << privateAction;
    
    _disconnectAction.setEnabled(!disconnectablePrivateActions.isEmpty());

    if (privateActions.isEmpty())
        _disconnectAction.setText("Disconnect...");
    else {
        if (privateActions.count() == 1)
            _disconnectAction.setText("Disconnect...");
        else
            _disconnectAction.setText(QString("Disconnect %1 parameter(s)").arg(QString::number(privateActions.count())));
    }

    connect(&_publishAction, &TriggerAction::triggered, this, [this]() -> void {
        mv::actions().publishPrivateAction(_actions.first());
    });

    connect(&_connectAction, &TriggerAction::triggered, this, [this]() -> void {
        if (_actions.isEmpty())
            return;

        _actions.first()->getDrag().start();
    });

    connect(&_disconnectAction, &TriggerAction::triggered, this, [this]() -> void {
        for (auto action : _actions)
            if (action->isPrivate())
                mv::actions().disconnectPrivateActionFromPublicAction(action, true);
            
    });

    connect(&_disconnectAllAction, &TriggerAction::triggered, this, [this]() -> void {
        for (auto publicAction : _actions)
            for (auto connectedPrivateAction : publicAction->getConnectedActions())
                mv::actions().disconnectPrivateActionFromPublicAction(connectedPrivateAction, true);
    });

    connect(&_editAction, &TriggerAction::triggered, this, [this]() -> void {
        if (_actions.isEmpty())
            return;

        EditActionsDialog editActionsDialog(this, _actions);

        editActionsDialog.exec();
    });
}

WidgetActionContextMenu::ConfirmRemovePublicActionDialog::ConfirmRemovePublicActionDialog(QWidget* parent, WidgetActions publicActions) :
    QDialog(parent),
    _publicActions(publicActions),
    _namesAction(this, "Names"),
    _removeAction(this, "Remove"),
    _cancelAction(this, "Cancel")
{
    setWindowIcon(StyledIcon("trash"));
    setMinimumWidth(400);
    
    QStringList names;

    for (const auto& publicAction : publicActions)
        names << publicAction->text();

    _namesAction.setEnabled(false);
    _namesAction.setToolTip("Names of the shared parameters which will be removed");
    _namesAction.setStrings(names);

    if (_publicActions.count() == 1)
        setWindowTitle(QString("Remove shared parameter %1").arg(_publicActions.first()->text()));
    else
        setWindowTitle(QString("Remove %1 shared parameters").arg(QString::number(_publicActions.count())));

    auto layout = new QVBoxLayout();

    std::int32_t numberOfConnectedActions = 0;

    for (const auto& publicAction : publicActions)
        numberOfConnectedActions += publicAction->getConnectedActions().count();

    layout->addWidget(new QLabel(QString("Remove the parameter(s) below?")));
    layout->addWidget(_namesAction.createWidget(this, StringsAction::ListView));

    if (numberOfConnectedActions >= 1)
        layout->addWidget(new QLabel(QString("Note: %1 parameter(s) will be disconnected as well").arg(QString::number(numberOfConnectedActions))));

    layout->addSpacing(10);

    _removeAction.setToolTip("Remove shared parameter(s)");
    _cancelAction.setToolTip("Quit and don't remove shared parameter(s)");

    auto buttonsLayout = new QHBoxLayout();

    buttonsLayout->addWidget(mv::settings().getParametersSettings().getConfirmRemoveSharedParameterAction().createWidget(this));
    buttonsLayout->addSpacing(10);
    buttonsLayout->addWidget(_removeAction.createWidget(this));
    buttonsLayout->addWidget(_cancelAction.createWidget(this));

    layout->addLayout(buttonsLayout);

    setLayout(layout);

    connect(&_removeAction, &TriggerAction::triggered, this, &QDialog::accept);
    connect(&_cancelAction, &TriggerAction::triggered, this, &QDialog::reject);
}

WidgetActionContextMenu::EditActionsDialog::EditActionsDialog(QWidget* parent, WidgetActions actions) :
    QDialog(parent),
    _actions(actions),
    _actionsGroupAction(this, "Actions")
{
    setWindowIcon(StyledIcon("edit"));
    setWindowTitle(QString("Edit %1").arg(actions.count() == 1 ? _actions.first()->text() : QString("%1 parameters").arg(QString::number(_actions.count()))));

    auto layout = new QGridLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(_actionsGroupAction.createWidget(this));

    for (auto action : _actions) {
        if (action->isPublic()) {
            action->cacheConnectionPermissions(true);
            action->setConnectionPermissionsToForceNone(true);
        }

        _actionsGroupAction.addAction(action);
    }

    _actionsGroupAction.setShowLabels(_actions.count() >= 2);

    setLayout(layout);
}

void WidgetActionContextMenu::EditActionsDialog::closeEvent(QCloseEvent* event)
{
    for (auto action : _actions)
        if (action->isPublic())
            action->restoreConnectionPermissions(true);

    QDialog::closeEvent(event);
}

}
