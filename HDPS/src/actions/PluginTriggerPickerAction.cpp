#include "PluginTriggerPickerAction.h"

#include <Application.h>
#include <CoreInterface.h>

#include <QHBoxLayout>

namespace hdps {

namespace gui {

PluginTriggerPickerAction::PluginTriggerPickerAction(QObject* parent, const QString& title /*= ""*/) :
    TriggerAction(parent),
    _pluginTriggerActions(),
    _selectTriggerAction(this, "Pick plugin")
{
    setText(title);

    _selectTriggerAction.setToolTip("Select plugin to create");
    _selectTriggerAction.setPlaceHolderString("Pick conversion algorithm");

    connect(&_selectTriggerAction, &OptionAction::currentIndexChanged, this, [this](const std::int32_t& currentIndex) -> void {
        if (currentIndex == 0)
            _selectTriggerAction.setCurrentIndex(-1);

        emit currentPluginTriggerActionChanged(getCurrentPluginTriggerAction());
    });
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

PluginTriggerAction* PluginTriggerPickerAction::getPluginTriggerAction(const QString& sha)
{
    for (const auto& pluginTriggerAction : _pluginTriggerActions)
        if (pluginTriggerAction->getSha() == sha)
            return pluginTriggerAction;

    return nullptr;
}

PluginTriggerActions PluginTriggerPickerAction::getPluginTriggerActions()
{
    return _pluginTriggerActions;
}

PluginTriggerAction* PluginTriggerPickerAction::getCurrentPluginTriggerAction()
{
    if (!_selectTriggerAction.hasSelection())
        return nullptr;

    return _pluginTriggerActions[_selectTriggerAction.getCurrentIndex() - 1];
}

void PluginTriggerPickerAction::setCurrentPluginTriggerAction(const QString& sha)
{
    for (const auto& pluginTriggerAction : _pluginTriggerActions) {
        if (pluginTriggerAction->getSha() == sha) {
            setCurrentPluginTriggerAction(pluginTriggerAction);
            return;
        }
    }
     
    reset();
}

void PluginTriggerPickerAction::setCurrentPluginTriggerAction(PluginTriggerAction* pluginTriggerAction)
{
    if (_pluginTriggerActions.contains(pluginTriggerAction))
        _selectTriggerAction.setCurrentIndex(_pluginTriggerActions.indexOf(pluginTriggerAction) + 1);
    else
        reset();
}

OptionAction& PluginTriggerPickerAction::getSelectTriggerAction()
{
    return _selectTriggerAction;
}

void PluginTriggerPickerAction::reset()
{
    _selectTriggerAction.setCurrentIndex(-1);
}

PluginTriggerPickerAction::Widget::Widget(QWidget* parent, PluginTriggerPickerAction* pluginTriggerPickerAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, pluginTriggerPickerAction, widgetFlags),
    _pluginTriggerPickerAction(pluginTriggerPickerAction),
    _configurationToolButton(this, nullptr)
{
    auto layout = new QHBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);

    layout->addWidget(pluginTriggerPickerAction->getSelectTriggerAction().createWidget(this));
    layout->addWidget(&_configurationToolButton);

    const auto updateSelectTriggerAction = [this, pluginTriggerPickerAction]() -> void {
        const auto pluginTriggerActions = _pluginTriggerPickerAction->getPluginTriggerActions();

        QStringList options { "None" };

        for (const auto& pluginTriggerAction : pluginTriggerActions)
            options << pluginTriggerAction->text();

        pluginTriggerPickerAction->getSelectTriggerAction().setOptions(options);
        pluginTriggerPickerAction->getSelectTriggerAction().setCurrentIndex(-1);

        _configurationToolButton.getToolButton().setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    };

    const auto updateConfigurationToolButton = [this, pluginTriggerPickerAction]() -> void {
        const auto pluginTriggerActions  = _pluginTriggerPickerAction->getPluginTriggerActions();

        if (pluginTriggerPickerAction->getSelectTriggerAction().getCurrentIndex() >= 0)
            _configurationToolButton.setWidgetAction(pluginTriggerActions[pluginTriggerPickerAction->getSelectTriggerAction().getCurrentIndex() - 1]->getConfigurationAction());
        else
            _configurationToolButton.setWidgetAction(nullptr);

        _configurationToolButton.setEnabled(_configurationToolButton.getWidgetAction());
        _configurationToolButton.getToolButton().setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    };

    connect(pluginTriggerPickerAction, &PluginTriggerPickerAction::pluginTriggerActionsChanged, this, updateSelectTriggerAction);
    connect(&pluginTriggerPickerAction->getSelectTriggerAction(), &OptionAction::currentIndexChanged, this, updateConfigurationToolButton);

    updateSelectTriggerAction();
    updateConfigurationToolButton();

    setLayout(layout);
}

}
}