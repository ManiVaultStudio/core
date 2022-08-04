#include "PluginTriggerPickerAction.h"

#include <Application.h>
#include <CoreInterface.h>

#include <QHBoxLayout>

namespace hdps {

namespace gui {

PluginTriggerPickerAction::PluginTriggerPickerAction(QObject* parent) :
    TriggerAction(parent),
    _datasets(),
    _pluginTriggerActions()
{
}

QWidget* PluginTriggerPickerAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new Widget(parent, this, widgetFlags);
}

void PluginTriggerPickerAction::initialize(const QString& title, const plugin::Type& pluginType, const Datasets& datasets)
{
    setText(title);

    _datasets               = datasets;
    _pluginTriggerActions   = Application::core()->getPluginTriggerActionsByPluginTypeAndDatasets(pluginType, datasets);

    emit pluginTriggerActionsChanged(_pluginTriggerActions);
}

void PluginTriggerPickerAction::initialize(const QString& title, const QString& pluginKind, const Datasets& datasets)
{
    setText(title);

    _datasets               = datasets;
    _pluginTriggerActions   = Application::core()->getPluginTriggerActionsByPluginKindAndDatasets(pluginKind, datasets);

    emit pluginTriggerActionsChanged(_pluginTriggerActions);
}

QList<PluginTriggerAction*> PluginTriggerPickerAction::getPluginTriggerActions()
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