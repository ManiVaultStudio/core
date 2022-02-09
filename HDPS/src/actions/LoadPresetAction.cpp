#include "LoadPresetAction.h"
#include "PresetsAction.h"
#include "Application.h"

namespace hdps {

namespace gui {

LoadPresetAction::LoadPresetAction(PresetsAction* presetsAction) :
    WidgetAction(presetsAction),
    _presetsAction(presetsAction),
    _selectionAction(this, "Select preset")
{
    setText("Load preset");
    setToolTip("Load preset");
    setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
}

LoadPresetAction::Widget::Widget(QWidget* parent, LoadPresetAction* loadPresetAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, loadPresetAction, widgetFlags),
    _layout()
{
    if (widgetFlags & PopupLayout) {
        _layout.addWidget(loadPresetAction->getSelectionAction().createLabelWidget(this));
        _layout.addWidget(loadPresetAction->getSelectionAction().createWidget(this));

        setPopupLayout(&_layout);
    }
    else {
        setLayout(&_layout);
    }
}

}
}