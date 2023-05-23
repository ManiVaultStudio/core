#include "WidgetActionContextMenu.h"
#include "WidgetAction.h"

#include "models/ActionsListModel.h"
#include "models/ActionsFilterModel.h"

#include "Application.h"
#include "CoreInterface.h"

#include <QDebug>

namespace hdps::gui {

WidgetActionContextMenu::WidgetActionContextMenu(QWidget* parent, WidgetActions actions) :
    QMenu(parent),
    _actions(actions),
    _publishAction(this, "Publish..."),
    _disconnectAction(this, "Disconnect..."),
    _removeAction(this, "Remove..."),
    _editAction(this, "Edit...")
{
    Q_ASSERT(actions.count() != 0);

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _publishAction.setIcon(fontAwesome.getIcon("cloud-upload-alt"));
    _disconnectAction.setIcon(fontAwesome.getIcon("unlink"));
    _removeAction.setIcon(fontAwesome.getIcon("trash"));
    _editAction.setIcon(fontAwesome.getIcon("edit"));

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

    addAction(&_disconnectAction);

    addSeparator();

    addAction(&_removeAction);
    addAction(&_editAction);

    _editAction.setVisible(allPublic && _actions.count() == 1);

    if (allPrivate) {
        _removeAction.setVisible(false);

        if (_actions.count() == 1) {
            _publishAction.setEnabled(!firstAction->isPublished());
            _disconnectAction.setEnabled(firstAction->isConnected());

            if (firstAction->isConnected())
                _disconnectAction.setText(QString("Disconnect from: %1").arg(firstAction->getPublicAction()->text()));

            _publishAction.setText("Publish...");

            ActionsListModel actionsListModel(this);
            ActionsFilterModel actionsFilterModel(this);

            actionsFilterModel.setSourceModel(&actionsListModel);
            actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Public" });
            actionsFilterModel.getTypeFilterAction().setString(firstAction->getTypeString());

            const auto numberOfRows = actionsFilterModel.rowCount();

            auto connectMenu = new QMenu("Connect to:");

            connectMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("link"));

            for (int rowIndex = 0; rowIndex < numberOfRows; ++rowIndex) {
                auto publicAction = actionsFilterModel.getAction(rowIndex);

                if (!publicAction)
                    continue;

                if (publicAction == firstAction->getPublicAction())
                    continue;

                auto connectAction = new QAction(publicAction->getLocation());

                connectAction->setToolTip("Connect " + firstAction->text() + " to " + publicAction->text());

                connect(connectAction, &QAction::triggered, this, [this, firstAction, publicAction]() -> void {
                    hdps::actions().connectPrivateActionToPublicAction(firstAction, publicAction, true);
                });

                connectMenu->addAction(connectAction);
            }

            connectMenu->setEnabled(!connectMenu->actions().isEmpty());

            insertMenu(&_disconnectAction, connectMenu);
        }
        else {
            _publishAction.setVisible(false);
            _disconnectAction.setVisible(true);
            _removeAction.setVisible(false);

            _disconnectAction.setText(QString("Disconnect %1").arg(QString::number(_actions.count())));
        }
    }

    if (allPublic) {
        _publishAction.setVisible(false);
        _disconnectAction.setVisible(false);

        WidgetActions rootPublicActions;

        for (auto action : actions) {
            if (action->isPublic() && action->isRoot())
                rootPublicActions << action;
        }

        _removeAction.setVisible(rootPublicActions.count() >= 1);

        if (_actions.count() == 1)
            _removeAction.setText(QString("Remove: %1").arg(firstAction->text()));
        else
            _removeAction.setText(QString("Remove: %1").arg(QString::number(_actions.count())));

        WidgetActions publicActions;

        for (auto action : _actions)
            if (action->isPublic())
                publicActions << action;

        connect(&_removeAction, &TriggerAction::triggered, this, [this, rootPublicActions]() -> void {
            for (auto rootPublicAction : rootPublicActions) {
                for (auto connectedAction : rootPublicAction->getConnectedActions())
                    hdps::actions().disconnectPrivateActionFromPublicAction(connectedAction, true);

                delete rootPublicAction;
            }
        });
    }
    
    connect(&_publishAction, &TriggerAction::triggered, this, [this]() -> void {
        hdps::actions().publishPrivateAction(_actions.first());
    });

    connect(&_disconnectAction, &TriggerAction::triggered, this, [this]() -> void {
        for (auto action : _actions)
            hdps::actions().disconnectPrivateActionFromPublicAction(action, true);
    });

    connect(&_editAction, &TriggerAction::triggered, this, [this]() -> void {
        if (_actions.isEmpty())
            return;

        EditActionDialog editActionDialog(this, *_actions.first());

        editActionDialog.exec();
    });
}

WidgetActionContextMenu::EditActionDialog::EditActionDialog(QWidget* parent, WidgetAction& action) :
    QDialog(parent),
    _action(action)
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("edit"));
    setWindowTitle(QString("Edit %1").arg(_action.text()));

    auto layout = new QVBoxLayout();

    layout->addWidget(_action.createWidget(this));

    setLayout(layout);
}

}
