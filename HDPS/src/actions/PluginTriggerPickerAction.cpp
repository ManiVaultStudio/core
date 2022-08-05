#include "PluginTriggerPickerAction.h"

#include <Application.h>
#include <CoreInterface.h>

#include <QHBoxLayout>

namespace hdps {

namespace gui {

PluginTriggerPickerAction::PluginTriggerPickerAction(QObject* parent, const QString& title /*= ""*/) :
    TriggerAction(parent),
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

    emit pluginTriggerActionsChanged(_pluginTriggerActions);
}

void PluginTriggerPickerAction::initialize(const plugin::Type& pluginType, const DataTypes& dataTypes)
{
    _pluginTriggerActions = Application::core()->getPluginTriggerActions(pluginType, dataTypes);

    emit pluginTriggerActionsChanged(_pluginTriggerActions);
}

void PluginTriggerPickerAction::initialize(const QString& pluginKind, const Datasets& datasets)
{
    _pluginTriggerActions = Application::core()->getPluginTriggerActions(pluginKind, datasets);

    emit pluginTriggerActionsChanged(_pluginTriggerActions);
}

void PluginTriggerPickerAction::initialize(const QString& pluginKind, const DataTypes& dataTypes)
{
    _pluginTriggerActions = Application::core()->getPluginTriggerActions(pluginKind, dataTypes);

    emit pluginTriggerActionsChanged(_pluginTriggerActions);
}

PluginTriggerActions PluginTriggerPickerAction::getPluginTriggerActions()
{
    return _pluginTriggerActions;
}

PluginTriggerPickerAction::Widget::Widget(QWidget* parent, PluginTriggerPickerAction* pluginTriggerPickerAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, pluginTriggerPickerAction, widgetFlags),
    _pluginTriggerPickerAction(pluginTriggerPickerAction),
    _selectTriggerAction(this, "Pick plugin"),
    _configurationToolButton(this, nullptr)
{
    _selectTriggerAction.setToolTip("Select plugin to create");
    _selectTriggerAction.setPlaceHolderString("Pick conversion algorithm");

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

        _selectTriggerAction.setOptions(options);
        _selectTriggerAction.setCurrentIndex(-1);

        _configurationToolButton.getToolButton().setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    };

    const auto updateConfigurationToolButton = [this]() -> void {
        const auto pluginTriggerActions  = _pluginTriggerPickerAction->getPluginTriggerActions();

        if (_selectTriggerAction.getCurrentIndex() >= 0)
            _configurationToolButton.setWidgetAction(pluginTriggerActions[_selectTriggerAction.getCurrentIndex()]->getConfigurationAction());
        else
            _configurationToolButton.setWidgetAction(nullptr);

        _configurationToolButton.setEnabled(_configurationToolButton.getWidgetAction());
        _configurationToolButton.getToolButton().setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    };

    connect(pluginTriggerPickerAction, &PluginTriggerPickerAction::pluginTriggerActionsChanged, this, updateSelectTriggerAction);
    connect(&_selectTriggerAction, &OptionAction::currentIndexChanged, this, updateConfigurationToolButton);

    updateSelectTriggerAction();
    updateConfigurationToolButton();

    setLayout(layout);
}

}
}