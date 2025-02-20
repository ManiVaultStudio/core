// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ActionsManager.h"

#include <Application.h>
#include <models/ActionsListModel.h>
#include <models/ActionsFilterModel.h>
#include <actions/WidgetAction.h>
#include <util/Exception.h>

#include <QDialogButtonBox>
#include <QMetaType>
#include <QMetaObject>

using namespace mv::gui;
using namespace mv::util;

#ifdef _DEBUG
    //#define ACTIONS_MANAGER_VERBOSE
#endif

namespace mv
{

ActionsManager::ActionsManager(QObject* parent) :
    AbstractActionsManager(parent),
    _actionsListModel(nullptr)
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

    AbstractActionsManager::initialize();

    if (isInitialized())
        return;

    beginInitialization();
    {
        _actionsListModel = new ActionsListModel(this);
    }
    endInitialization();
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
            const auto metaTypeName         = publicActionMap["ActionType"].toString();

            if (metaTypeName.isEmpty())
                throw std::runtime_error(QString("Action type is not specified for %1").arg(publicActionTitle).toLatin1());

            const auto metaType     = QMetaType::fromName(metaTypeName.toLatin1());
            const auto metaObject   = metaType.metaObject();
                
            if (!metaObject)
                throw std::runtime_error(QString("Meta object type '%1' for '%2' is not known. Did you forget to register the action correctly with Qt meta object system? See ToggleAction.h for an example.").arg(metaTypeName, publicActionTitle).toLatin1());

            auto metaObjectInstance = metaObject->newInstance(Q_ARG(QObject*, this), Q_ARG(QString, publicActionTitle));
            auto publicAction       = dynamic_cast<WidgetAction*>(metaObjectInstance);

            if (!publicAction)
                throw std::runtime_error(QString("Unable to create a new instance of type '%1'").arg(publicActionTitle).toLatin1());

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
        if (auto parentPublicAction = dynamic_cast<WidgetAction*>(publicAction->getParent()))
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

bool ActionsManager::publishPrivateAction(WidgetAction* privateAction, const QString& name /*= ""*/, bool recursive /*= true*/, bool allowDuplicateName /*= false*/)
{
    try
    {
        Q_ASSERT(privateAction != nullptr);

        if (privateAction == nullptr)
            throw std::runtime_error("Supplied private action may not be a null pointer");

        auto& askForSharedParameterNameAction = mv::settings().getParametersSettings().getAskForSharedParameterNameAction();

        if (name.isEmpty()) {
            if (askForSharedParameterNameAction.isChecked()) {
                ActionsFilterModel actionsFilterModel;

                actionsFilterModel.setSourceModel(&getActionsListModel());
                actionsFilterModel.getTextFilterColumnAction().setCurrentText("Name");
                actionsFilterModel.getTypeFilterAction().setString(privateAction->getTypeString());
                actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Public" });

                QDialog publishDialog;

                publishDialog.setWindowIcon(StyledIcon("cloud-upload-alt"));
                publishDialog.setWindowTitle("Publish " + privateAction->getLocation());
                publishDialog.setMinimumWidth(400);

                QVBoxLayout mainLayout;
                QGridLayout parameterLayout;
                QHBoxLayout buttonsLayout;

                StringAction nameAction(this, "Name", privateAction->text());
                TriggerAction publishAction(this, "Publish");
                TriggerAction cancelAction(this, "Cancel");
                StringAction nameAvailabilityAction(this, "Name availability", "");

                nameAction.setConnectionPermissionsToForceNone();
                publishAction.setConnectionPermissionsToForceNone();
                cancelAction.setConnectionPermissionsToForceNone();
                nameAvailabilityAction.setConnectionPermissionsToForceNone();

                nameAvailabilityAction.setDefaultWidgetFlags(StringAction::Label);

                nameAvailabilityAction.setWidgetConfigurationFunction([&actionsFilterModel , &nameAction](WidgetAction* action, QWidget* widget) -> void {
                    const auto nameChanged = [&nameAction, &actionsFilterModel, widget]() -> void {
                        const auto isDuplicateName = !actionsFilterModel.match(actionsFilterModel.index(0, static_cast<int>(AbstractActionsModel::Column::Name)), Qt::DisplayRole, nameAction.getString(), -1, Qt::MatchFlag::MatchExactly).isEmpty();
                        
                        if (nameAction.getString().isEmpty() ? true : isDuplicateName)
                            widget->setStyleSheet("color: red;");
                        else
                            widget->setStyleSheet("");
					};

                    connect(&nameAction, &StringAction::stringChanged, widget, nameChanged);
				});

                nameAction.setToolTip("Name of the shared parameter");

                parameterLayout.addWidget(nameAction.createLabelWidget(&publishDialog), 0, 0);
                parameterLayout.addWidget(nameAction.createWidget(&publishDialog), 0, 1);
                parameterLayout.addWidget(nameAvailabilityAction.createWidget(&publishDialog), 1, 1);

                buttonsLayout.addWidget(askForSharedParameterNameAction.createWidget(&publishDialog));
                buttonsLayout.addSpacing(10);
                buttonsLayout.addWidget(publishAction.createWidget(&publishDialog));
                buttonsLayout.addWidget(cancelAction.createWidget(&publishDialog));

                mainLayout.addLayout(&parameterLayout);
                mainLayout.addSpacing(10);
                mainLayout.addLayout(&buttonsLayout);

                publishDialog.setLayout(&mainLayout);

                const auto nameChanged = [&publishAction, &nameAction, &nameAvailabilityAction, &actionsFilterModel]() -> void {
                    const auto isDuplicateName = !actionsFilterModel.match(actionsFilterModel.index(0, static_cast<int>(AbstractActionsModel::Column::Name)), Qt::DisplayRole, nameAction.getString(), -1, Qt::MatchFlag::MatchExactly).isEmpty();

                    publishAction.setEnabled(!nameAction.getString().isEmpty() && !isDuplicateName);
                    nameAvailabilityAction.setString(nameAction.getString().isEmpty() ? "Name may not be empty" : isDuplicateName ? QString("<i>%1</i> is already taken").arg(nameAction.getString()) : QString("<i>%1</i> is available").arg(nameAction.getString()));
                };

                connect(&nameAction, &StringAction::stringChanged, this, nameChanged);
                connect(&publishAction, &TriggerAction::triggered, &publishDialog, &QDialog::accept);
                connect(&cancelAction, &TriggerAction::triggered, &publishDialog, &QDialog::reject);

                nameChanged();

                publishDialog.open();

                QEventLoop eventLoop;
            
                QObject::connect(&publishDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
            
                eventLoop.exec();

                switch (publishDialog.result())
                {
                    case QDialog::Accepted:
                        return publishPrivateAction(privateAction, nameAction.getString(), recursive, allowDuplicateName);

                    case QDialog::Rejected:
                        return false;
                }
            }
            else {
                return publishPrivateAction(privateAction, QString("%1_pub").arg(privateAction->text()), recursive, allowDuplicateName);
            }
        }
        else {
    #ifdef ACTIONS_MANAGER_VERBOSE
            qDebug() << __FUNCTION__ << privateAction->text();
    #endif

            ActionsFilterModel  actionsFilterModel(this);

            actionsFilterModel.setSourceModel(&mv::actions().getActionsListModel());
            actionsFilterModel.setFilterColumn(static_cast<int>(ActionsListModel::Column::Name));
            actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Public" });
            actionsFilterModel.setFilterRegularExpression(name);

            const auto numberOfActionsWithDuplicateName = actionsFilterModel.rowCount();

            auto shouldPublish = false;

            if (numberOfActionsWithDuplicateName >= 1) {
                const auto duplicateActionNamesMessage = QString("Found %1 action%2 with the same name").arg(QString::number(numberOfActionsWithDuplicateName), numberOfActionsWithDuplicateName == 1 ? "" : "s");

                if (allowDuplicateName == false) {
                    return false;
                } else {
                    qDebug() << duplicateActionNamesMessage;

                    shouldPublish = true;
                }
            }
            else {
                shouldPublish = true;
            }

            if (shouldPublish == true) {
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

ActionsListModel& ActionsManager::getActionsListModel()
{
    return *_actionsListModel;
}

}
