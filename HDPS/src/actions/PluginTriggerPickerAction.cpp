#include "PluginTriggerPickerAction.h"

#include <Application.h>
#include <CoreInterface.h>

#include <QHBoxLayout>

namespace hdps {

namespace gui {

PluginTriggerPickerAction::PluginTriggerPickerAction(QObject* parent, const QString& title /*= ""*/) :
    TriggerAction(parent),
    _datasets(),
    _pluginTriggerActions()
{
    setText(title);
}

QWidget* PluginTriggerPickerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this, widgetFlags);
}

void PluginTriggerPickerAction::initialize(const plugin::Type& pluginType, const Datasets& datasets)
{
    _pluginTriggerActions = Application::core()->getPluginTriggerActions(pluginType, datasets);

    setInputDatasets(datasets);

    emit pluginTriggerActionsChanged(_pluginTriggerActions);
}

void PluginTriggerPickerAction::initialize(const QString& pluginKind, const Datasets& datasets)
{
    _pluginTriggerActions = Application::core()->getPluginTriggerActions(pluginKind, datasets);

    emit pluginTriggerActionsChanged(_pluginTriggerActions);
}

void PluginTriggerPickerAction::setInputDatasets(const Datasets& datasets)
{
    _datasets = datasets;
}

PluginTriggerActions PluginTriggerPickerAction::getPluginTriggerActions()
{
    return _pluginTriggerActions;
}

void PluginTriggerPickerAction::updatePluginTriggerActions()
{

}

PluginTriggerPickerAction::Widget::Widget(QWidget* parent, PluginTriggerPickerAction* pluginTriggerPickerAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, pluginTriggerPickerAction, widgetFlags),
    _pluginTriggerPickerAction(pluginTriggerPickerAction),
    _selectTriggerAction(this, "Pick plugin"),
    _configurationToolButton(this, nullptr)
{
    _selectTriggerAction.setToolTip("Select plugin to create");

    _configurationToolButton.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));

    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    layout->addWidget(_selectTriggerAction.createWidget(this));
    layout->addWidget(&_configurationToolButton);

    const auto updateSelectTriggerAction = [this]() -> void {
        const auto pluginTriggerActions = _pluginTriggerPickerAction->getPluginTriggerActions();

        QStringList options;

        for (const auto& pluginTriggerAction : pluginTriggerActions)
            options << pluginTriggerAction->text();

        qDebug() << options;

        _selectTriggerAction.setOptions(options);
    };

    const auto updateConfigurationToolButton = [this]() -> void {
        const auto pluginTriggerActions  = _pluginTriggerPickerAction->getPluginTriggerActions();

        if (_selectTriggerAction.getCurrentIndex() >= 0)
            _configurationToolButton.setWidgetAction(pluginTriggerActions[_selectTriggerAction.getCurrentIndex()]->getConfigurationAction());
        else
            _configurationToolButton.setWidgetAction(nullptr);

        _configurationToolButton.setEnabled(_configurationToolButton.getWidgetAction());
    };

    connect(pluginTriggerPickerAction, &PluginTriggerPickerAction::pluginTriggerActionsChanged, this, updateSelectTriggerAction);
    connect(&_selectTriggerAction, &OptionAction::currentIndexChanged, this, updateConfigurationToolButton);

    updateSelectTriggerAction();
    updateConfigurationToolButton();

    setLayout(layout);
}

}
}