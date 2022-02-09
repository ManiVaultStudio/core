#include "PresetsAction.h"
#include "Application.h"

#include <QMenu>

using namespace hdps::util;

namespace hdps {

namespace gui {

PresetsAction::PresetsAction(QObject* parent) :
    WidgetAction(parent),
    _widgetAction(nullptr),
    _presetsModel(this),
    _presetsFilterModel(this),
    _loadPresetAction(this),
    _savePresetAction(this),
    _loadAction(this, "Load"),
    _saveAction(this, "Save"),
    _ignoreResettableSignals(false)
{
    setText("Preset");
    setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setSerializable(false);

    _presetsFilterModel.setSourceModel(&_presetsModel);

    _loadAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _saveAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));

    // Load default when triggered
    connect(&_loadAction, &TriggerAction::triggered, this, [this]() -> void {
        _widgetAction->fromJsonFile();
    });

    // Save default when triggered
    connect(&_saveAction, &TriggerAction::triggered, this, [this]() -> void {
        _widgetAction->toJsonFile();
    });
}

QMenu* PresetsAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    // Create main context menu
    auto contextMenu = new QMenu();

    // Add load/save default actions
    contextMenu->addAction(&_loadPresetAction);
    contextMenu->addAction(&_savePresetAction);

    contextMenu->addSeparator();

    // Create context menu for additional options
    auto optionsMenu = new QMenu("Options");

    // Configure the context menu and add reset to factory default action
    //optionsMenu->setEnabled(_widgetAction->isFactoryResettable());
    optionsMenu->setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));

    // Add the options menu to the main context menu
    contextMenu->addMenu(optionsMenu);

    return contextMenu;
}

void PresetsAction::updateActions()
{
#ifdef _DEBUG
    qDebug() << "Update actions";
#endif

    Q_ASSERT(_widgetAction != nullptr);

    _loadPresetAction.setEnabled(_widgetAction->isResettable());
    _savePresetAction.setEnabled(_widgetAction->canSaveDefault());
}

void PresetsAction::setWidgetAction(hdps::gui::WidgetAction* widgetAction)
{
    // Disconnect from previous widget action
    if (_widgetAction) {
        disconnect(_widgetAction, &WidgetAction::resettableChanged, this, nullptr);
        disconnect(_widgetAction, &WidgetAction::factoryResettableChanged, this, nullptr);
    }

    _widgetAction = widgetAction;

    //_presetsModel.setWidgetAction(widgetAction);

    // Update actions when the (factory) resettable-ness of the widget action changed
    //if (_widgetAction) {
    //    connect(_widgetAction, &WidgetAction::resettableChanged, this, &PresetsAction::updateActions);
    //    connect(_widgetAction, &WidgetAction::factoryResettableChanged, this, &PresetsAction::updateActions);
    //}
}

hdps::util::PresetsFilterModel& PresetsAction::getPresetsFilterModel()
{
    return _presetsFilterModel;
}

PresetsAction::Widget::Widget(QWidget* parent, PresetsAction* presetsAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, presetsAction, widgetFlags)
{
    _layout.setMargin(0);
    _layout.setSpacing(4);

    _layout.addWidget(presetsAction->getLoadAction().createWidget(this));
    _layout.addWidget(presetsAction->getSaveAction().createWidget(this));

    setLayout(&_layout);
}

}
}