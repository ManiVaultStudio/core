#include "ActionsManager.h"

#include "actions/WidgetAction.h"
#include "util/Exception.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps
{

ActionsManager::ActionsManager(QObject* parent /*= nullptr*/) :
    QObject(parent),
    _publicActions()
{
}

void ActionsManager::addPublicAction(WidgetAction* publicAction)
{
    try
    {
        if (publicAction == nullptr)
            throw std::runtime_error("Action is not valid");

        _publicActions << publicAction;
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to add public action ", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to add public action ");
    }
}

void ActionsManager::removePublicAction(WidgetAction* publicAction)
{
    try
    {
        if (publicAction == nullptr)
            throw std::runtime_error("Action is not valid");

        if (!_publicActions.contains(publicAction))
            throw std::runtime_error("Action is not public");

        const auto index = _publicActions.indexOf(publicAction);

        delete publicAction;

        _publicActions.removeAt(index);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to remove public action ", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to remove public action ");
    }
}

}
