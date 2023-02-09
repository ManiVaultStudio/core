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
        const auto metaType         = publicAction.toMap()["Type"].toString();
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

    /*
    for (const auto action : _model.getActions()) {
        if (!action->isPublic())
            continue;

        auto actionVariantMap = action->toVariantMap();

        actionVariantMap["Title"] = action->text();

        publicActions << actionVariantMap;
    }
    */

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

void ActionsManager::publishPrivateAction(gui::WidgetAction* privateAction, const QString& name /*= ""*/)
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

            auto mainLayout         = new QVBoxLayout();
            auto parameterLayout    = new QHBoxLayout();
            auto label              = new QLabel("Name:");
            auto lineEdit           = new QLineEdit(privateAction->text());

            parameterLayout->addWidget(label);
            parameterLayout->addWidget(lineEdit);

            mainLayout->addLayout(parameterLayout);

            auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

            dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Publish");
            dialogButtonBox->button(QDialogButtonBox::Ok)->setToolTip("Publish the parameter");
            dialogButtonBox->button(QDialogButtonBox::Cancel)->setToolTip("Cancel publishing");

            connect(dialogButtonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, &publishDialog, &QDialog::accept);
            connect(dialogButtonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, &publishDialog, &QDialog::reject);

            mainLayout->addWidget(dialogButtonBox);

            publishDialog.setLayout(mainLayout);
            publishDialog.setFixedWidth(300);

            const auto updateOkButtonReadOnly = [dialogButtonBox, lineEdit]() -> void {
                dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!lineEdit->text().isEmpty());
            };

            connect(lineEdit, &QLineEdit::textChanged, this, updateOkButtonReadOnly);

            updateOkButtonReadOnly();

            if (publishDialog.exec() == QDialog::Accepted)
                publishPrivateAction(privateAction, lineEdit->text());
        }
        else {
            if (privateAction->isPublished())
                throw std::runtime_error("Action is already published");

            auto publicAction = privateAction->getPublicCopy();

            makeActionPublic(publicAction);

            if (publicAction == nullptr)
                throw std::runtime_error("Public copy not created");

            publicAction->setText(name);

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
