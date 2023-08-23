// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

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
    AbstractActionsManager()
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

    Application::current()->getStartupTask().setSubtaskFinished("Initializing actions manager");

    AbstractActionsManager::initialize();
}

void ActionsManager::reset()
{
#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        //for (auto publicAction : _publicActions)
        //    delete publicAction;
    }
    endReset();
}

void ActionsManager::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

#ifdef ACTIONS_MANAGER_VERBOSE
    qDebug() << "Loading public actions";
#endif
    
    variantMapMustContain(variantMap, "PublicActions");

    const auto publicActionsMap = variantMap["PublicActions"].toList();

    for (const auto& publicActionVariant : publicActionsMap) {
        try
        {
            const auto publicActionMap      = publicActionVariant.toMap();
            const auto publicActionTitle    = publicActionMap["Title"].toString();
            const auto metaType             = publicActionMap["ActionType"].toString();

            if (metaType.isEmpty())
                throw std::runtime_error(QString("Action type is not specified for %1").arg(publicActionTitle).toLatin1());

            const auto metaTypeId   = QMetaType::type(metaType.toLatin1());
            const auto metaObject   = QMetaType::metaObjectForType(metaTypeId);
                
            if (!metaObject)
                throw std::runtime_error(QString("Meta object type '%1' for '%2' is not known. Did you forget to register the action correctly with Qt meta object system? See ToggleAction.h for an example.").arg(metaType, publicActionTitle).toLatin1());

            auto metaObjectInstance = metaObject->newInstance(Q_ARG(QObject*, this), Q_ARG(QString, publicActionTitle));
            auto publicAction       = dynamic_cast<WidgetAction*>(metaObjectInstance);

            if (!publicAction)
                throw std::runtime_error(QString("Unable to create a new instance of type '%1'").arg(metaType).toLatin1());

            publicAction->fromVariantMap(publicActionMap);

            makeActionPublic(publicAction);
        }
        catch (std::exception& e)
        {
            exceptionMessageBox("Unable to load public action:", e);
        }
        catch (...)
        {
            exceptionMessageBox("Unable to load public action:");
        }
    }
}

QVariantMap ActionsManager::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    QVariantList publicActions;

    for (auto publicAction : getPublicActions()) {
        auto parentPublicAction = dynamic_cast<WidgetAction*>(publicAction->getParentAction());

        if (parentPublicAction)
            continue;

        auto actionVariantMap = publicAction->toVariantMap();

        actionVariantMap["Title"] = publicAction->text();
        
        publicActions << actionVariantMap;
    }

    variantMap.insert({
        { "PublicActions", publicActions }
    });

    return variantMap;
}

bool ActionsManager::publishPrivateAction(WidgetAction* privateAction, const QString& name /*= ""*/, bool recursive /*= true*/)
{
    try
    {
        Q_ASSERT(privateAction != nullptr);

        if (privateAction == nullptr)
            throw std::runtime_error("Supplied private action may not be a null pointer");

        auto& askForSharedParameterNameAction = hdps::settings().getParametersSettings().getAskForSharedParameterNameAction();

        if (name.isEmpty()) {
            if (askForSharedParameterNameAction.isChecked()) {
                auto& fontAwesome = Application::getIconFont("FontAwesome");

                QDialog publishDialog;

                publishDialog.setWindowIcon(fontAwesome.getIcon("cloud-upload-alt"));
                publishDialog.setWindowTitle("Publish " + privateAction->getLocation());
                publishDialog.setMinimumWidth(400);

                QVBoxLayout mainLayout;
                QGridLayout parameterLayout;
                QHBoxLayout buttonsLayout;

                StringAction nameAction(this, "Name", privateAction->text());
                TriggerAction publishAction(this, "Publish");
                TriggerAction cancelAction(this, "Cancel");

                nameAction.setConnectionPermissionsToForceNone();
                publishAction.setConnectionPermissionsToForceNone();
                cancelAction.setConnectionPermissionsToForceNone();

                nameAction.setToolTip("Name of the shared parameter");

                parameterLayout.addWidget(nameAction.createLabelWidget(&publishDialog), 0, 0);
                parameterLayout.addWidget(nameAction.createWidget(&publishDialog), 0, 1);

                buttonsLayout.addWidget(askForSharedParameterNameAction.createWidget(&publishDialog));
                buttonsLayout.addSpacing(10);
                buttonsLayout.addWidget(publishAction.createWidget(&publishDialog));
                buttonsLayout.addWidget(cancelAction.createWidget(&publishDialog));

                mainLayout.addLayout(&parameterLayout);
                mainLayout.addSpacing(10);
                mainLayout.addLayout(&buttonsLayout);

                publishDialog.setLayout(&mainLayout);

                const auto updateActionsReadOnly = [&publishAction, &nameAction]() -> void {
                    publishAction.setEnabled(!nameAction.getString().isEmpty());
                };

                connect(&nameAction, &StringAction::stringChanged, this, updateActionsReadOnly);
                connect(&publishAction, &TriggerAction::triggered, &publishDialog, &QDialog::accept);
                connect(&cancelAction, &TriggerAction::triggered, &publishDialog, &QDialog::reject);

                updateActionsReadOnly();

                publishDialog.open();

                QEventLoop eventLoop;
            
                QObject::connect(&publishDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
            
                eventLoop.exec();

                switch (publishDialog.result())
                {
                    case QDialog::Accepted:
                        return publishPrivateAction(privateAction, nameAction.getString(), true);

                    case QDialog::Rejected:
                        return false;
                }
            }
            else {
                return publishPrivateAction(privateAction, QString("%1_pub").arg(privateAction->text()), true);
            }
        }
        else {
    #ifdef ACTIONS_MANAGER_VERBOSE
            qDebug() << __FUNCTION__ << privateAction->text();
    #endif

            if (privateAction->isPublished())
                throw std::runtime_error("Action is already published");

            auto publicAction = privateAction->getPublicCopy();

            publicAction->setText(name);

            connectPrivateActionToPublicAction(privateAction, publicAction, true);

            emit privateAction->isPublishedChanged(privateAction->isPublished());
            emit privateAction->isConnectedChanged(privateAction->isConnected());

            return true;
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish parameter:", e);

        return false;
    }
    catch (...)
    {
        exceptionMessageBox("Unable to publish parameter:");

        return false;
    }

    return false;
}

}
