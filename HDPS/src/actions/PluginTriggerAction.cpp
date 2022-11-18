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
    _title(title),
    _sha(),
    _datasets(datasets),
    _configurationAction(nullptr)
{
}

PluginTriggerAction::PluginTriggerAction(QObject* parent, const plugin::PluginFactory* pluginFactory, const QString& title, const Datasets& datasets /*= Datasets()*/) :
    TriggerAction(parent),
    _pluginKind(),
    _pluginFactory(pluginFactory),
    _title(title),
    _sha(),
    _datasets(datasets),
    _configurationAction(nullptr)
{
}

const hdps::plugin::PluginFactory* PluginTriggerAction::getPluginFactory() const
{
    return _pluginFactory;
}

QString PluginTriggerAction::getTitle() const
{
    return _title;
}

void PluginTriggerAction::setTitle(const QString& title)
{
    _title = title;
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
        Application::core()->getPluginManager().getPluginFactory(_pluginKind);

    _sha = QString(QCryptographicHash::hash(QString("%1_%2").arg(_pluginFactory->getKind(), getTitle()).toUtf8(), QCryptographicHash::Sha1).toHex());

    switch (_pluginFactory->getType())
    {
        case plugin::Type::ANALYSIS:
            _title.insert(0, "Analyze/");
            break;

        case plugin::Type::DATA:
            _title.insert(0, "Data/");
            break;

        case plugin::Type::LOADER:
            _title.insert(0, "Import/");
            break;

        case plugin::Type::TRANSFORMATION:
            _title.insert(0, "Transform/");
            break;

        case plugin::Type::VIEW:
            _title.insert(0, "View/");
            break;

        case plugin::Type::WRITER:
            _title.insert(0, "Export/");
            break;

        default:
            break;
    }

    setText(_title.split("/").last());
    setIcon(_pluginFactory->getIcon());
}

}
