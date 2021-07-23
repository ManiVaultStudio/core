#pragma once

#include "actions/Actions.h"

namespace hdps {

class Core;

class ExportDataAction : public hdps::gui::WidgetAction
{
public:
    ExportDataAction(QObject* parent, Core* core, const QString& datasetName);

    QMenu* getContextMenu();

protected:
    hdps::gui::TriggerAction    _binaryAction;
    hdps::gui::TriggerAction    _csvAction;
};

}
