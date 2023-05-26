#include "WidgetActionMimeData.h"

namespace hdps::gui {

WidgetActionMimeData::WidgetActionMimeData(WidgetAction* action) :
    QMimeData(),
    _action(action),
    _actionsListModel(this),
    _actionsFilterModel(this)
{
    _actionsFilterModel.setSourceModel(&_actionsListModel);
    _actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Private" });
    _actionsFilterModel.getTypeFilterAction().setString(getAction()->getTypeString());

    for (int rowIndex = 0; rowIndex < _actionsFilterModel.rowCount(); ++rowIndex) {
        auto action = _actionsFilterModel.getAction(rowIndex);

        if (action == getAction())
            continue;

        _actionsFilterModel.getAction(rowIndex)->highlight();
    }
}

WidgetActionMimeData::~WidgetActionMimeData()
{
    for (int rowIndex = 0; rowIndex < _actionsFilterModel.rowCount(); ++rowIndex) {
        auto action = _actionsFilterModel.getAction(rowIndex);

        if (action == getAction())
            continue;

        _actionsFilterModel.getAction(rowIndex)->unHighlight();
    }
}

QStringList WidgetActionMimeData::formats() const
{
    return { format() };
}

}