#include "SavePresetAction.h"
#include "PresetsAction.h"
#include "Application.h"

namespace hdps {

namespace gui {

SavePresetAction::SavePresetAction(PresetsAction* presetsAction) :
    WidgetAction(presetsAction),
    _presetsAction(presetsAction),
    _presetName(this, "Preset name"),
    _completer(),
    _saveAction(this, "Save")
{
    setText("Save preset");
    setToolTip("Save preset");
    setIcon(Application::getIconFont("FontAwesome").getIcon("save"));

    _completer.setModel(&_presetsAction->getPresetsFilterModel());

    _presetName.setCompleter(&_completer);

    _saveAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _saveAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
}

SavePresetAction::Widget::Widget(QWidget* parent, SavePresetAction* savePresetAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, savePresetAction, widgetFlags),
    _layout()
{
    if (widgetFlags & PopupLayout) {
        _layout.addWidget(savePresetAction->getPresetNameAction().createLabelWidget(this));
        _layout.addWidget(savePresetAction->getPresetNameAction().createWidget(this));
        _layout.addWidget(savePresetAction->getSaveAction().createWidget(this));

        setPopupLayout(&_layout);
    }
    else {
        setLayout(&_layout);
    }
}

}
}