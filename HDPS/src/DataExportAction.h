#pragma once

#include "actions/Actions.h"

namespace hdps {

class Core;

class DataExportAction : public hdps::gui::WidgetAction
{
public:
    DataExportAction(QObject* parent, Core* core, const QString& datasetName);

    QMenu* getContextMenu();

protected:
    hdps::gui::TriggerAction    _binaryAction;
    hdps::gui::TriggerAction    _csvAction;
};

}
