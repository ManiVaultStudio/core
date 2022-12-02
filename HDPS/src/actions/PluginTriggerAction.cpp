#include "PluginTriggerAction.h"
#include "Application.h"
#include "AbstractPluginManager.h"

#include "pluginFactory.h"

#include <QCryptographicHash>

namespace hdps::gui {

PluginTriggerAction::PluginTriggerAction(QObject* parent, const QString pluginKind, const QString& title, const Datasets& datasets /*= Datasets()*/) :
    TriggerAction(parent),
    _pluginKind(pluginKind),
    _pluginFactory(nullptr),
    _location(),
    _sha(),
    _datasets(datasets),
    _configurationAction(nullptr)
{
    setText(title);
}

PluginTriggerAction::PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const Datasets& datasets /*= Datasets()*/) :
    TriggerAction(parent),
    _pluginKind(),
    _pluginFactory(pluginFactory),
    _location(),
    _sha(),
    _datasets(datasets),
    _configurationAction(nullptr)
{
    setText(title);
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

Datasets PluginTriggerAction::getDatasets() const
{
    return _datasets;
}

void PluginTriggerAction::setDatasets(const Datasets& datasets)
{
    _datasets = datasets;
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
    if (_pluginFactory == nullptr)
        _pluginFactory = Application::core()->getPluginManager().getPluginFactory(_pluginKind);

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

}
