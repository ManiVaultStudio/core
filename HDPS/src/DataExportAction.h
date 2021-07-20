#pragma once

#include "actions/Actions.h"

class DataExportAction : public hdps::gui::WidgetAction
{
public:
    DataExportAction(QObject* parent);

    QMenu* getContextMenu();

protected:
    hdps::gui::TriggerAction    _binaryAction;
    hdps::gui::TriggerAction    _csvAction;
};