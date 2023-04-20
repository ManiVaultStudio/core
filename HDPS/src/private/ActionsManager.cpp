#include "ActionsManager.h"

#include <Application.h>
#include <models/ActionsFilterModel.h>
#include <actions/WidgetAction.h>
#include <util/Exception.h>

#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>

using namespace hdps::gui;
using namespace hdps::util;

#ifdef _DEBUG
    #define ACTIONS_MANAGER_VERBOSE
#endif

namespace hdps
{

ActionsManager::ActionsManager() :
    AbstractActionsManager(),
    _model()
{
}

ActionsManager::~ActionsManager()
{
    reset();
}

void ActionsManager::initialize()
{
#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractManager::initialize();
}

void ActionsManager::reset()
{
#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
    }
    endReset();
}

const hdps::gui::WidgetActions& ActionsManager::getActions() const
{
    return _model.getActions();
}

void ActionsManager::addActionToModel(WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    _model.addAction(action);

    emit actionAdded(action);
}

void ActionsManager::removeActionFromModel(WidgetAction* action)
{
    Q_ASSERT(action != nullptr);

#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << action->text();
#endif

    const auto actionId = action->getId();

    emit actionAboutToBeRemoved(action);
    {
        _model.removeAction(action);
    }
    emit actionRemoved(actionId);
}

void ActionsManager::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "PublicActions");

    const auto publicActions = variantMap["PublicActions"].toList();

    for (const auto& publicAction : publicActions) {
        const auto publicActionMap  = publicAction.toMap();
        const auto metaType         = publicActionMap["Type"].toString();
        const auto metaTypeId       = QMetaType::type(metaType.toLatin1());
        const auto metaObject       = QMetaType::metaObjectForType(metaTypeId);

        if (metaObject) {
            auto action = qobject_cast<WidgetAction*>(metaObject->newInstance());

            action->setText(publicActionMap["Title"].toString());
            action->fromVariantMap(publicActionMap);

            makeActionPublic(action);
        }
    }
}

QVariantMap ActionsManager::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    QVariantList publicActions;

    for (auto action : _model.getActions()) {
        //Q_ASSERT(action != nullptr);

        if (action == nullptr)
            break;

        if (!action->isPublic())
            continue;

        //auto actionVariantMap = action->toVariantMap();

        //actionVariantMap["Title"] = action->text();

        //publicActions << actionVariantMap;
    }

    variantMap.insert({
        { "PublicActions", publicActions }
    });

    return variantMap;
}

WidgetAction* ActionsManager::getAction(const QString& id)
{
    for (const auto action : _model.getActions())
        if (id == action->getId())
            return action;
    
    return nullptr;
}

ActionsModel& ActionsManager::getActionsModel()
{
    return _model;
}

void ActionsManager::publishPrivateAction(WidgetAction* privateAction, const QString& name /*= ""*/, bool recursive /*= true*/)
{
#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__ << privateAction->text();
#endif

    try
    {
        if (name.isEmpty()) {
            auto& fontAwesome = Application::getIconFont("FontAwesome");

            QDialog publishDialog;

            publishDialog.setWindowIcon(fontAwesome.getIcon("cloud-upload-alt"));
            publishDialog.setWindowTitle("Publish " + privateAction->text() + " parameter");

            QVBoxLayout mainLayout;
            QGridLayout parameterLayout;
            QHBoxLayout buttonsLayout;

            StringAction nameAction(this, "Name", privateAction->text());
            ToggleAction recursiveAction(this, "Recursive");
            TriggerAction publishAction(this, "Publish");
            TriggerAction cancelAction(this, "Cancel");

            nameAction.setToolTip("Name of the shared parameter");

            parameterLayout.addWidget(nameAction.createLabelWidget(&publishDialog), 0, 0);
            parameterLayout.addWidget(nameAction.createWidget(&publishDialog), 0, 1);
            //parameterLayout.addWidget(recursiveAction.createWidget(&publishDialog), 1, 1);

            buttonsLayout.addStretch(1);
            buttonsLayout.addWidget(publishAction.createWidget(&publishDialog));
            buttonsLayout.addWidget(cancelAction.createWidget(&publishDialog));

            mainLayout.addLayout(&parameterLayout);
            mainLayout.addLayout(&buttonsLayout);

            publishDialog.setLayout(&mainLayout);
            publishDialog.setFixedWidth(300);

            const auto updateActionsReadOnly = [&]() -> void {
                recursiveAction.setEnabled(!nameAction.getString().isEmpty());
                publishAction.setEnabled(!nameAction.getString().isEmpty());
            };

            connect(&nameAction, &StringAction::stringChanged, this, updateActionsReadOnly);
            connect(&publishAction, &TriggerAction::triggered, &publishDialog, &QDialog::accept);
            connect(&cancelAction, &TriggerAction::triggered, &publishDialog, &QDialog::reject);

            updateActionsReadOnly();

            if (publishDialog.exec() == QDialog::Accepted)
                publishPrivateAction(privateAction, nameAction.getString(), recursiveAction.isChecked());
        }
        else {
            if (privateAction->isPublished())
                throw std::runtime_error("Action is already published");

            auto publicAction = privateAction->_getPublicCopy();

            if (publicAction == nullptr)
                throw std::runtime_error("Unable to create public action");

            connectPrivateActionToPublicAction(privateAction, publicAction);

            emit privateAction->isPublishedChanged(privateAction->isPublished());
            emit privateAction->isConnectedChanged(privateAction->isConnected());
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish " + privateAction->text(), e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to publish " + privateAction->text());
    }
}

}
