#include "PluginTriggerAction.h"
#include "Application.h"
#include "AbstractPluginManager.h"

#include "pluginFactory.h"

#include <QCryptographicHash>

namespace hdps::gui {

PluginTriggerAction::PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const QString& tooltip, const QIcon& icon) :
    TriggerAction(parent),
    _pluginFactory(pluginFactory),
    _location(),
    _sha(),
    _configurationAction(nullptr),
    _requestPluginCallback()
{
    setText(title);
    setToolTip(tooltip);
    setIcon(icon);

    connect(this, &TriggerAction::triggered, this, &PluginTriggerAction::requestPlugin);

    setRequestPluginCallback([this](PluginTriggerAction& pluginTriggerAction) -> void {
        Application::core()->getPluginManager().requestPlugin(_pluginFactory->getKind());
    });
}

PluginTriggerAction::PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const QString& tooltip, const QIcon& icon, RequestPluginCallback requestPluginCallback) :
    TriggerAction(parent),
    _pluginFactory(pluginFactory),
    _location(),
    _sha(),
    _configurationAction(nullptr),
    _requestPluginCallback(requestPluginCallback)
{
    setText(title);
    setToolTip(tooltip);
    setIcon(icon);

    connect(this, &TriggerAction::triggered, this, &PluginTriggerAction::requestPlugin);
}

PluginTriggerAction::PluginTriggerAction(const PluginTriggerAction& pluginTriggerAction) :
    TriggerAction(pluginTriggerAction.parent()),
    _pluginFactory(pluginTriggerAction.getPluginFactory()),
    _location(),
    _sha(),
    _configurationAction(nullptr),
    _requestPluginCallback()
{
    setText(pluginTriggerAction.text());
    setToolTip(pluginTriggerAction.toolTip());
    setIcon(pluginTriggerAction.icon());

    connect(this, &TriggerAction::triggered, this, &PluginTriggerAction::requestPlugin);
}

const hdps::plugin::PluginFactory* PluginTriggerAction::getPluginFactory() const
{
    return _pluginFactory;
}

QString PluginTriggerAction::getLocation() const
{
    return _location;
}

void PluginTriggerAction::setLocation(const QString& location)
{
    _location = location;
}

QString PluginTriggerAction::getSha() const
{
    return _sha;
}

WidgetAction* PluginTriggerAction::getConfigurationAction()
{
    return _configurationAction;
}

void PluginTriggerAction::setConfigurationAction(WidgetAction* configurationAction)
{
    _configurationAction = configurationAction;
}

void PluginTriggerAction::initialize()
{
    _sha = QString(QCryptographicHash::hash(QString("%1_%2").arg(_pluginFactory->getKind(), getLocation()).toUtf8(), QCryptographicHash::Sha1).toHex());

    setIcon(_pluginFactory->getIcon());
}

void PluginTriggerAction::setText(const QString& text)
{
    QAction::setText(text);

    switch (_pluginFactory->getType())
    {
        case plugin::Type::ANALYSIS:
            _location = "Analyze";
            break;

        case plugin::Type::DATA:
            _location = "Data";
            break;

        case plugin::Type::LOADER:
            _location = "Import";
            break;

        case plugin::Type::TRANSFORMATION:
            _location = "Transform";
            break;

        case plugin::Type::VIEW:
            _location = "View";
            break;

        case plugin::Type::WRITER:
            _location = "Export";
            break;

        default:
            break;
    }

    _location.append("/");
    _location.append(this->text());
}

void PluginTriggerAction::setRequestPluginCallback(RequestPluginCallback requestPluginCallback)
{
    _requestPluginCallback = requestPluginCallback;
}

void PluginTriggerAction::requestPlugin()
{
    if (_requestPluginCallback)
        _requestPluginCallback(*this);
}

}
